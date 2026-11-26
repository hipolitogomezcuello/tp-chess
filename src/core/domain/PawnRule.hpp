#pragma once

#include <vector>

#include "domain/MovementRule.hpp"

namespace chess::domain {

class PawnRule : public MovementRule {
public:
    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;
};

}
