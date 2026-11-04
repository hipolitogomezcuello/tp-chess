#pragma once

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/PieceFactory.hpp"

namespace chess::domain {

// Aplica una jugada y devuelve el estado siguiente. Es puro: no modifica el estado recibido,
// así LegalMoveFilter puede "probar" jugadas y la aplicación guardar el estado previo (Memento).
// No valida la jugada: eso ya lo hizo el pipeline (MoveGenerator → LegalMoveFilter).
// En una promoción con pieza elegida, crea la pieza nueva con la PieceFactory (D-08); sin pieza
// elegida (cuando el filtro la prueba) el peón queda en la casilla, que para el jaque da igual.
// Pendiente para las misiones 7.3 y 7.4: enroque y al paso.
class MoveApplier {
public:
    // Guarda una referencia: la fábrica la posee RulesEngine (o el test).
    explicit MoveApplier(const PieceFactory& factory) : factory_(factory) {}

    GameState apply(const GameState& state, const Move& move) const;

private:
    const PieceFactory& factory_;
};

}  // namespace chess::domain
