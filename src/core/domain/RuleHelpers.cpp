#include "domain/RuleHelpers.hpp"

namespace chess::domain {

std::vector<Move> movesOntoReachableSquares(const MoveContext& context, const std::vector<Position>& squares) {
    std::vector<Move> result;
    for (const Position square : squares) {
        const auto target = context.board().pieceAt(square);
        if (!target) {
            result.emplace_back(context.from(), square, MoveKind::Normal);
        } else if (target->color() != context.piece().color()) {
            result.emplace_back(context.from(), square, MoveKind::Capture);
        }
    }
    return result;
}

}  // namespace chess::domain
