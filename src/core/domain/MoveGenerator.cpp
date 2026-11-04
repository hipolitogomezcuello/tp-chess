#include "domain/MoveGenerator.hpp"

#include "domain/MoveContext.hpp"
#include "domain/MovementRule.hpp"

namespace chess::domain {

std::vector<Move> MoveGenerator::movesFrom(const GameState& state, Position from) const {
    if (state.board().isEmpty(from)) {
        return {};
    }
    const MoveContext context(state, from);
    std::vector<Move> result;
    for (const auto& rule : context.piece().definition().rules()) {
        const auto moves = rule->candidateMoves(context);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    return result;
}

std::vector<Move> MoveGenerator::allMoves(const GameState& state) const {
    std::vector<Move> result;
    for (const Position from : state.board().positionsOf(state.sideToMove())) {
        const auto moves = movesFrom(state, from);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    return result;
}

}  // namespace chess::domain
