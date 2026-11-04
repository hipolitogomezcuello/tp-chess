#include "domain/RulesEngine.hpp"

namespace chess::domain {

RulesEngine::RulesEngine(const PieceRegistry& registry)
    : registry_(registry),
      factory_(registry),
      applier_(factory_),
      filter_(applier_, detector_),
      evaluator_(detector_, generator_, filter_) {}

std::vector<Move> RulesEngine::legalMoves(const GameState& state, Position from) const {
    return filter_.filter(state, generator_.movesFrom(state, from));
}

GameState RulesEngine::apply(const GameState& state, const Move& move) const {
    return applier_.apply(state, move);
}

GameStatus RulesEngine::status(const GameState& state) const {
    return evaluator_.evaluate(state);
}

bool RulesEngine::canPromoteTo(const PieceTypeId& type) const {
    const auto definition = registry_.find(type);
    return definition && definition->traits().isPromotable;
}

std::vector<Piece> RulesEngine::promotionChoices(Color color) const {
    std::vector<Piece> result;
    for (const PieceTypeId& type : registry_.promotableTypes()) {
        result.push_back(factory_.create(type, color));
    }
    return result;
}

}  // namespace chess::domain
