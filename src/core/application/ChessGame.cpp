#include "application/ChessGame.hpp"

#include <algorithm>
#include <utility>

#include "application/GameOverPhase.hpp"
#include "application/InProgressPhase.hpp"
#include "application/MoveCommand.hpp"
#include "ports/MoveError.hpp"

namespace chess::application {

namespace {

bool isGameOver(domain::GameStatus status) {
    return status == domain::GameStatus::Checkmate || status == domain::GameStatus::Stalemate;
}

}  // namespace

using domain::GameStatus;
using domain::Move;
using domain::Position;
using ports::MoveError;
using ports::MoveResult;

ChessGame::ChessGame(const domain::RulesEngine& rules, domain::GameState initialState)
    : rules_(rules), state_(std::move(initialState)) {
    updatePhase();
}

MoveResult ChessGame::makeMove(Position from, Position to, std::optional<domain::PieceTypeId> promotion) {
    return phase_->tryMove(*this, from, to, promotion);
}

MoveResult ChessGame::performMove(Position from, Position to, const std::optional<domain::PieceTypeId>& promotion) {
    const domain::Board& board = state_.board();
    if (!board.dimensions().contains(from) || board.isEmpty(from)) {
        return MoveResult::failure(MoveError::NoPieceAtOrigin);
    }
    if (!hasPieceOfSideToMove(from)) {
        return MoveResult::failure(MoveError::NotYourTurn);
    }

    // El dominio ya filtró las jugadas que dejan al rey en jaque: si `to` no está, es ilegal.
    const std::vector<Move> legal = rules_.legalMoves(state_, from);
    const auto chosen = std::find_if(legal.begin(), legal.end(), [to](const Move& m) { return m.to() == to; });
    if (chosen == legal.end()) {
        return MoveResult::failure(MoveError::IllegalMove);
    }

    const MoveResult completed = withPromotionChoice(*chosen, promotion);
    if (!completed.ok()) {
        return completed;
    }

    const Move move = *completed.move();
    MoveCommand command(rules_, move);
    command.execute(state_);
    history_.record(std::move(command));
    updatePhase();
    notifyAfterMove(move);
    return MoveResult::success(move);
}

std::vector<Move> ChessGame::legalMovesFrom(Position from) const {
    if (!hasPieceOfSideToMove(from)) {
        return {};
    }
    return rules_.legalMoves(state_, from);
}

GameStatus ChessGame::status() const {
    return rules_.status(state_);
}

std::vector<domain::Piece> ChessGame::promotionChoices() const {
    return rules_.promotionChoices(state_.sideToMove());
}

bool ChessGame::undo() {
    if (!history_.undo(state_)) {
        return false;
    }
    updatePhase();  // deshacer un mate vuelve a InProgressPhase (D-10)
    notifyUndoRedo();
    return true;
}

bool ChessGame::redo() {
    if (!history_.redo(state_)) {
        return false;
    }
    updatePhase();
    notifyUndoRedo();
    return true;
}

void ChessGame::addObserver(ports::GameObserver& observer) {
    observers_.emplace_back(observer);
}

bool ChessGame::hasPieceOfSideToMove(Position from) const {
    const domain::Board& board = state_.board();
    if (!board.dimensions().contains(from)) {
        return false;
    }
    const auto piece = board.pieceAt(from);
    return piece && piece->color() == state_.sideToMove();
}

MoveResult ChessGame::withPromotionChoice(const Move& move,
                                          const std::optional<domain::PieceTypeId>& promotion) const {
    if (move.kind() != domain::MoveKind::Promotion) {
        return promotion ? MoveResult::failure(MoveError::InvalidPromotion) : MoveResult::success(move);
    }
    if (!promotion) {
        return MoveResult::failure(MoveError::PromotionRequired);
    }
    if (!rules_.canPromoteTo(*promotion)) {
        return MoveResult::failure(MoveError::InvalidPromotion);
    }
    return MoveResult::success(move.withPromotion(*promotion));
}

void ChessGame::updatePhase() {
    if (isGameOver(status())) {
        phase_ = std::make_unique<GameOverPhase>();
    } else {
        phase_ = std::make_unique<InProgressPhase>();
    }
}

void ChessGame::notifyAfterMove(const Move& move) {
    const GameStatus current = status();
    for (ports::GameObserver& observer : observers_) {
        observer.onMoveMade(move, state_);
        if (current == GameStatus::Check) {
            observer.onCheck(state_.sideToMove());
        } else if (isGameOver(current)) {
            observer.onGameOver(current, state_.sideToMove());
        }
    }
}

void ChessGame::notifyUndoRedo() {
    for (ports::GameObserver& observer : observers_) {
        observer.onUndoRedo(state_);
    }
}

}  // namespace chess::application
