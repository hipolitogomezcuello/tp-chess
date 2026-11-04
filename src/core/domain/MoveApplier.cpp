#include "domain/MoveApplier.hpp"

#include <utility>

namespace chess::domain {

GameState MoveApplier::apply(const GameState& state, const Move& move) const {
    Board board = state.board();  // copia: el estado original queda intacto
    board.move(move.from(), move.to());
    board.place(move.to(), board.pieceAt(move.to())->withMoved());
    return GameState(std::move(board), opposite(state.sideToMove()));
}

}  // namespace chess::domain
