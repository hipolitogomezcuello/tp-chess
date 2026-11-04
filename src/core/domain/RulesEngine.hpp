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
#include "domain/Position.hpp"

namespace chess::domain {

// Patrón: Facade — ver docs/arquitectura.md §3.6
// Única puerta del dominio para la capa de aplicación: legalMoves / apply / status.
// Esconde el pipeline interno MoveGenerator → LegalMoveFilter (MoveApplier + AttackDetector)
// → GameStatusEvaluator, que crea y posee.
// Fase 7: recibe el PieceRegistry (PieceFactory para la promoción y canPromoteTo).
class RulesEngine {
public:
    RulesEngine();

    // Los componentes internos se referencian entre sí: no se copia ni se mueve.
    RulesEngine(const RulesEngine&) = delete;
    RulesEngine& operator=(const RulesEngine&) = delete;

    // Jugadas legales de la pieza en `from` (vacío si no hay pieza). No mira de quién es el turno:
    // eso lo decide la aplicación (ChessGame → MoveError::NotYourTurn).
    std::vector<Move> legalMoves(const GameState& state, Position from) const;

    // Estado siguiente. Precondición: `move` salió de legalMoves.
    GameState apply(const GameState& state, const Move& move) const;

    GameStatus status(const GameState& state) const;

private:
    // El orden de declaración importa: filter_ y evaluator_ referencian a los anteriores.
    MoveGenerator generator_;
    MoveApplier applier_;
    AttackDetector detector_;
    LegalMoveFilter filter_;
    GameStatusEvaluator evaluator_;
};

}  // namespace chess::domain
