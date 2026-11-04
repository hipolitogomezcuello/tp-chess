#include "domain/RulesEngine.hpp"

namespace chess::domain {

RulesEngine::RulesEngine() : filter_(applier_, detector_), evaluator_(detector_) {}

std::vector<Move> RulesEngine::legalMoves(const GameState& state, Position from) const {
    return filter_.filter(state, generator_.movesFrom(state, from));
}

GameState RulesEngine::apply(const GameState& state, const Move& move) const {
    return applier_.apply(state, move);
}

GameStatus RulesEngine::status(const GameState& state) const {
    return evaluator_.evaluate(state);
}

}  // namespace chess::domain
