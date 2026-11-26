#pragma once

#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/Offset.hpp"

namespace chess::domain {

class SlidingRule : public MovementRule {
public:
    explicit SlidingRule(std::vector<Offset> directions);

    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;

private:
    std::vector<Offset> directions_;
};

}
