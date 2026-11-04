#include "domain/MoveApplier.hpp"

#include <optional>
#include <utility>

namespace chess::domain {

GameState MoveApplier::apply(const GameState& state, const Move& move) const {
    Board board = state.board();  // copia: el estado original queda intacto
    board.move(move.from(), move.to());
    const Piece moved = *board.pieceAt(move.to());
    if (move.promotion()) {
        board.place(move.to(), factory_.create(*move.promotion(), moved.color()).withMoved());
    } else {
        board.place(move.to(), moved.withMoved());
    }
    if (move.kind() == MoveKind::Castling) {
        board.move(*move.rookFrom(), *move.rookTo());
        board.place(*move.rookTo(), board.pieceAt(*move.rookTo())->withMoved());
    }
    if (move.kind() == MoveKind::EnPassant) {
        board.remove(*move.capturedSquare());
    }

    std::optional<Position> enPassantTarget;
    if (move.kind() == MoveKind::DoublePawnPush) {
        // La casilla del medio entre el origen y el destino.
        enPassantTarget = Position((move.from().file() + move.to().file()) / 2,
                                   (move.from().rank() + move.to().rank()) / 2);
    }
    return GameState(std::move(board), opposite(state.sideToMove()), enPassantTarget);
}

}  // namespace chess::domain
