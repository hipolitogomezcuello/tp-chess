#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "application/GamePhase.hpp"
#include "application/MoveHistory.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "domain/RulesEngine.hpp"
#include "ports/GameObserver.hpp"
#include "ports/GameService.hpp"
#include "ports/MoveResult.hpp"

namespace chess::application {

class ChessGame : public ports::GameService {
public:
    ChessGame(const domain::RulesEngine& rules, domain::GameState initialState);

    ports::MoveResult makeMove(domain::Position from, domain::Position to,
                               std::optional<domain::PieceTypeId> promotion = std::nullopt) override;
    std::vector<domain::Move> legalMovesFrom(domain::Position from) const override;
    const domain::GameState& state() const override { return state_; }
    domain::GameStatus status() const override;
    std::vector<domain::Piece> promotionChoices() const override;
    bool undo() override;
    bool redo() override;
    void addObserver(ports::GameObserver& observer) override;

    ports::MoveResult performMove(domain::Position from, domain::Position to,
                                  const std::optional<domain::PieceTypeId>& promotion);

private:
    bool hasPieceOfSideToMove(domain::Position from) const;
    ports::MoveResult withPromotionChoice(const domain::Move& move,
                                          const std::optional<domain::PieceTypeId>& promotion) const;
    void updatePhase();
    void notifyAfterMove(const domain::Move& move);
    void notifyUndoRedo();

    const domain::RulesEngine& rules_;
    domain::GameState state_;
    MoveHistory history_;
    std::unique_ptr<const GamePhase> phase_;
    std::vector<std::reference_wrapper<ports::GameObserver>> observers_;
};

}
