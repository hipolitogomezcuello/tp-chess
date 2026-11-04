#include "domain/MoveContext.hpp"

#include <stdexcept>

namespace chess::domain {

namespace {

Piece pieceAtOrigin(const GameState& state, Position from) {
    auto piece = state.board().pieceAt(from);
    if (!piece) {
        throw std::invalid_argument("MoveContext: no hay pieza en la casilla de origen");
    }
    return *piece;
}

}  // namespace

MoveContext::MoveContext(const GameState& state, Position from)
    : state_(state), from_(from), piece_(pieceAtOrigin(state, from)) {}

}  // namespace chess::domain
