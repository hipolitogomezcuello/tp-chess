#pragma once

#include "domain/AttackDetector.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"

namespace chess::domain {

// Último paso del pipeline: ¿en qué estado está la partida para el bando que mueve?
// Por ahora distingue InProgress y Check. En la fase 7 recibe también MoveGenerator y
// LegalMoveFilter para detectar Checkmate (en jaque y sin jugadas) y Stalemate (sin jaque y sin jugadas).
class GameStatusEvaluator {
public:
    explicit GameStatusEvaluator(const AttackDetector& detector) : detector_(detector) {}

    GameStatus evaluate(const GameState& state) const;

private:
    const AttackDetector& detector_;
};

}  // namespace chess::domain
