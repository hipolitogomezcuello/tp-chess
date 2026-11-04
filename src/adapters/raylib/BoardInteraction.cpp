#include "adapters/raylib/BoardInteraction.hpp"

#include <algorithm>

namespace chess::adapters::raylib {

using domain::Move;
using domain::Position;

void BoardInteraction::click(Position square) {
    if (awaitingPromotion()) {
        return;  // primero hay que elegir la pieza (o cancelar)
    }
    if (selected_) {
        const auto chosen = std::find_if(highlighted_.begin(), highlighted_.end(),
                                         [square](const Move& move) { return move.to() == square; });
        if (chosen != highlighted_.end()) {
            if (chosen->kind() == domain::MoveKind::Promotion) {
                pendingPromotion_ = *chosen;
                return;
            }
            game_.makeMove(chosen->from(), chosen->to());
            cancel();
            return;
        }
    }
    select(square);
}

void BoardInteraction::choosePromotion(const domain::PieceTypeId& type) {
    if (pendingPromotion_) {
        game_.makeMove(pendingPromotion_->from(), pendingPromotion_->to(), type);
    }
    cancel();
}

void BoardInteraction::cancel() {
    selected_.reset();
    highlighted_.clear();
    pendingPromotion_.reset();
}

bool BoardInteraction::undo() {
    cancel();
    return game_.undo();
}

bool BoardInteraction::redo() {
    cancel();
    return game_.redo();
}

void BoardInteraction::select(Position square) {
    // Se puede seleccionar una pieza propia aunque no tenga jugadas (queda sin resaltados).
    const domain::Board& board = game_.state().board();
    const bool ownPiece = board.dimensions().contains(square) && !board.isEmpty(square)
                       && board.pieceAt(square)->color() == game_.state().sideToMove();
    if (!ownPiece) {
        cancel();
        return;
    }
    selected_ = square;
    highlighted_ = game_.legalMovesFrom(square);
}

}  // namespace chess::adapters::raylib
