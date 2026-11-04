#pragma once

#include "domain/AttackDetector.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/LegalMoveFilter.hpp"
#include "domain/MoveGenerator.hpp"

namespace chess::domain {

// Último paso del pipeline: ¿en qué estado está la partida para el bando que mueve?
// Dos preguntas alcanzan para los cuatro casos:
//                  con jugadas legales   sin jugadas legales
//   en jaque       Check                 Checkmate
//   sin jaque      InProgress            Stalemate
// No pregunta por tipos de pieza: el jaque lo resuelve el AttackDetector (isRoyal) y las jugadas
// el MoveGenerator + LegalMoveFilter, así que vale para cualquier pieza o tablero.
class GameStatusEvaluator {
public:
    // Referencias sin propiedad: los dueños son RulesEngine (o el test).
    GameStatusEvaluator(const AttackDetector& detector, const MoveGenerator& generator,
                        const LegalMoveFilter& filter)
        : detector_(detector), generator_(generator), filter_(filter) {}

    GameStatus evaluate(const GameState& state) const;

private:
    bool hasLegalMoves(const GameState& state) const;

    const AttackDetector& detector_;
    const MoveGenerator& generator_;
    const LegalMoveFilter& filter_;
};

}  // namespace chess::domain
