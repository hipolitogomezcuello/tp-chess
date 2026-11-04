#include "domain/SlidingRule.hpp"

#include <utility>

#include "domain/MoveContext.hpp"
#include "domain/RuleHelpers.hpp"

namespace chess::domain {

SlidingRule::SlidingRule(std::vector<Offset> directions) : directions_(std::move(directions)) {}

std::vector<Position> SlidingRule::attackedSquares(const MoveContext& context) const {
    const Board& board = context.board();
    std::vector<Position> result;
    for (const Offset direction : directions_) {
        // Los bordes salen de las dimensiones reales del tablero (D-09).
        for (Position square = context.from() + direction; board.dimensions().contains(square);
             square = square + direction) {
            result.push_back(square);
            if (!board.isEmpty(square)) {
                break;  // la primera pieza (de cualquier color) corta la línea
            }
        }
    }
    return result;
}

std::vector<Move> SlidingRule::candidateMoves(const MoveContext& context) const {
    return movesOntoReachableSquares(context, attackedSquares(context));
}

}  // namespace chess::domain
