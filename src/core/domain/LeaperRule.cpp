#include "domain/LeaperRule.hpp"

#include <utility>

#include "domain/MoveContext.hpp"
#include "domain/RuleHelpers.hpp"

namespace chess::domain {

LeaperRule::LeaperRule(std::vector<Offset> offsets) : offsets_(std::move(offsets)) {}

std::vector<Position> LeaperRule::attackedSquares(const MoveContext& context) const {
    std::vector<Position> result;
    for (const Offset offset : offsets_) {
        const Position target = context.from() + offset;
        if (context.board().dimensions().contains(target)) {
            result.push_back(target);
        }
    }
    return result;
}

std::vector<Move> LeaperRule::candidateMoves(const MoveContext& context) const {
    return movesOntoReachableSquares(context, attackedSquares(context));
}

}  // namespace chess::domain
