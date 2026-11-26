#pragma once

#include "domain/AttackDetector.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/LegalMoveFilter.hpp"
#include "domain/MoveGenerator.hpp"

namespace chess::domain {

class GameStatusEvaluator {
public:
    GameStatusEvaluator(const AttackDetector& detector, const MoveGenerator& generator,
                        const LegalMoveFilter& filter)
        : detector_(detector), generator_(generator), filter_(filter) {}

    GameStatus evaluate(const GameState& state) const;

private:
    bool hasLegalMoves(const GameState& state) const;

    const AttackDetector& detector_;
    const MoveGenerator& generator_;
    const LegalMoveFilter& filter_;
};

}
