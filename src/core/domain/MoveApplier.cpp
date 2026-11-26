#include "domain/MoveApplier.hpp"

#include <optional>
#include <utility>

namespace chess::domain {

namespace {

Position squareBetween(Position from, Position to) {
    return Position((from.file() + to.file()) / 2, (from.rank() + to.rank()) / 2);
}

}

GameState MoveApplier::apply(const GameState& state, const Move& move) const {
    Board board = state.board();
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
        enPassantTarget = squareBetween(move.from(), move.to());
    }
    return GameState(std::move(board), opposite(state.sideToMove()), enPassantTarget);
}

}
