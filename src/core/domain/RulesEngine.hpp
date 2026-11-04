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

// Patrón: Facade — ver docs/arquitectura.md §3.6
// Única puerta del dominio para la capa de aplicación: legalMoves / apply / status.
// Esconde el pipeline interno MoveGenerator → LegalMoveFilter (MoveApplier + AttackDetector)
// → GameStatusEvaluator, que crea y posee. Con el PieceRegistry arma la PieceFactory que usa la
// promoción y responde qué piezas son promovibles (trait isPromotable, D-08).
class RulesEngine {
public:
    // Guarda una referencia al registro: debe vivir más que el motor (composition root).
    explicit RulesEngine(const PieceRegistry& registry);

    // Los componentes internos se referencian entre sí: no se copia ni se mueve.
    RulesEngine(const RulesEngine&) = delete;
    RulesEngine& operator=(const RulesEngine&) = delete;

    // Jugadas legales de la pieza en `from` (vacío si no hay pieza). No mira de quién es el turno:
    // eso lo decide la aplicación (ChessGame → MoveError::NotYourTurn).
    std::vector<Move> legalMoves(const GameState& state, Position from) const;

    // Estado siguiente. Precondición: `move` salió de legalMoves.
    GameState apply(const GameState& state, const Move& move) const;

    GameStatus status(const GameState& state) const;

    // ¿Puede un peón promover a este tipo? Solo si está registrado y tiene el trait isPromotable.
    bool canPromoteTo(const PieceTypeId& type) const;

    // Las piezas de `color` a las que se puede promover (la UI las ofrece; tienen símbolo y tipo).
    std::vector<Piece> promotionChoices(Color color) const;

private:
    // El orden de declaración importa: cada miembro referencia solo a los declarados antes.
    const PieceRegistry& registry_;
    PieceFactory factory_;
    MoveGenerator generator_;
    MoveApplier applier_;
    AttackDetector detector_;
    LegalMoveFilter filter_;
    GameStatusEvaluator evaluator_;
};

}  // namespace chess::domain
