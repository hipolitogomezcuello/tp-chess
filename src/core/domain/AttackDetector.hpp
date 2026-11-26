#pragma once

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

class AttackDetector {
public:
    bool isSquareAttacked(const GameState& state, Position square, Color byColor) const;

    bool isInCheck(const GameState& state, Color color) const;
};

}
