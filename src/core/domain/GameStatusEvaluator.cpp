#include "domain/GameStatusEvaluator.hpp"

namespace chess::domain {

GameStatus GameStatusEvaluator::evaluate(const GameState& state) const {
    const bool inCheck = detector_.isInCheck(state, state.sideToMove());
    if (hasLegalMoves(state)) {
        return inCheck ? GameStatus::Check : GameStatus::InProgress;
    }
    return inCheck ? GameStatus::Checkmate : GameStatus::Stalemate;
}

bool GameStatusEvaluator::hasLegalMoves(const GameState& state) const {
    return !filter_.filter(state, generator_.allMoves(state)).empty();
}

}  // namespace chess::domain
