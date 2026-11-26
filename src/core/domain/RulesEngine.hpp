#pragma once

#include <vector>

#include "domain/AttackDetector.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/GameStatusEvaluator.hpp"
#include "domain/LegalMoveFilter.hpp"
#include "domain/Move.hpp"
#include "domain/MoveApplier.hpp"
#include "domain/MoveGenerator.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

class RulesEngine {
public:
    explicit RulesEngine(const PieceRegistry& registry);

    RulesEngine(const RulesEngine&) = delete;
    RulesEngine& operator=(const RulesEngine&) = delete;

    std::vector<Move> legalMoves(const GameState& state, Position from) const;

    GameState apply(const GameState& state, const Move& move) const;

    GameStatus status(const GameState& state) const;

    bool canPromoteTo(const PieceTypeId& type) const;

    std::vector<Piece> promotionChoices(Color color) const;

private:
    const PieceRegistry& registry_;
    PieceFactory factory_;
    MoveGenerator generator_;
    MoveApplier applier_;
    AttackDetector detector_;
    LegalMoveFilter filter_;
    GameStatusEvaluator evaluator_;
};

}
