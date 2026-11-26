#pragma once

#include "domain/Board.hpp"
#include "domain/GameState.hpp"
#include "domain/Piece.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

class MoveContext {
public:
    MoveContext(const GameState& state, Position from);

    const GameState& state() const { return state_; }
    const Board& board() const { return state_.board(); }
    Position from() const { return from_; }
    const Piece& piece() const { return piece_; }

private:
    const GameState& state_;
    Position from_;
    Piece piece_;
};

}
