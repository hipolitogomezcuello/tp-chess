#pragma once

#include <vector>

#include "domain/AttackDetector.hpp"
#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/MoveApplier.hpp"

namespace chess::domain {

class LegalMoveFilter {
public:
    LegalMoveFilter(const MoveApplier& applier, const AttackDetector& detector)
        : applier_(applier), detector_(detector) {}

    std::vector<Move> filter(const GameState& state, const std::vector<Move>& moves) const;

private:
    bool leavesOwnKingSafe(const GameState& state, const Move& move) const;
    bool kingPathIsSafe(const GameState& state, const Move& castling) const;

    const MoveApplier& applier_;
    const AttackDetector& detector_;
};

}
