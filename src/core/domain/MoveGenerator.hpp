#pragma once

#include <vector>

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

class MoveGenerator {
public:
    std::vector<Move> movesFrom(const GameState& state, Position from) const;

    std::vector<Move> allMoves(const GameState& state) const;
};

}
