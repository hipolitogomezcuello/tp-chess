#pragma once

#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/Offset.hpp"

namespace chess::domain {

class LeaperRule : public MovementRule {
public:
    explicit LeaperRule(std::vector<Offset> offsets);

    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;

private:
    std::vector<Offset> offsets_;
};

}
