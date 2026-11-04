#include "domain/MoveApplier.hpp"

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
    return GameState(std::move(board), opposite(state.sideToMove()));
}

}  // namespace chess::domain
