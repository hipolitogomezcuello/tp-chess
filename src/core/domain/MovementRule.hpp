#pragma once

#include <vector>

namespace chess::domain {

class Move;
class MoveContext;
class Position;

class MovementRule {
public:
    virtual ~MovementRule() = default;

    virtual std::vector<Move> candidateMoves(const MoveContext& context) const = 0;

    virtual std::vector<Position> attackedSquares(const MoveContext& context) const = 0;
};

}
