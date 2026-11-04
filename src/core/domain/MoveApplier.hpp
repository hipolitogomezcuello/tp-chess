#pragma once

#include "domain/GameState.hpp"
#include "domain/Move.hpp"

namespace chess::domain {

// Aplica una jugada y devuelve el estado siguiente. Es puro: no modifica el estado recibido,
// así LegalMoveFilter puede "probar" jugadas y la aplicación guardar el estado previo (Memento).
// No valida la jugada: eso ya lo hizo el pipeline (MoveGenerator → LegalMoveFilter).
// Fase 7: recibirá la PieceFactory para la promoción y resolverá enroque y al paso.
class MoveApplier {
public:
    GameState apply(const GameState& state, const Move& move) const;
};

}  // namespace chess::domain
