#include "domain/AttackDetector.hpp"

#include <algorithm>

#include "domain/MoveContext.hpp"
#include "domain/MovementRule.hpp"

namespace chess::domain {

bool AttackDetector::isSquareAttacked(const GameState& state, Position square, Color byColor) const {
    for (const Position attacker : state.board().positionsOf(byColor)) {
        const MoveContext context(state, attacker);
        for (const auto& rule : context.piece().definition().rules()) {
            const auto attacked = rule->attackedSquares(context);
            if (std::find(attacked.begin(), attacked.end(), square) != attacked.end()) {
                return true;
            }
        }
    }
    return false;
}

bool AttackDetector::isInCheck(const GameState& state, Color color) const {
    const Board& board = state.board();
    for (const Position position : board.positionsOf(color)) {
        if (board.pieceAt(position)->traits().isRoyal
            && isSquareAttacked(state, position, opposite(color))) {
            return true;
        }
    }
    return false;
}

}  // namespace chess::domain
