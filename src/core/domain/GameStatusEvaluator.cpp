#include "domain/GameStatusEvaluator.hpp"

namespace chess::domain {

GameStatus GameStatusEvaluator::evaluate(const GameState& state) const {
    return detector_.isInCheck(state, state.sideToMove()) ? GameStatus::Check : GameStatus::InProgress;
}

}  // namespace chess::domain
