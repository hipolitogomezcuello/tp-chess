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
// En el enroque mueve también la torre (rookFrom → rookTo); al paso, saca al peón de capturedSquare.
// Después de un doble avance marca la casilla cruzada como enPassantTarget; cualquier otra jugada la
// borra, así el al paso solo vale en la jugada siguiente.
class MoveApplier {
public:
    // Guarda una referencia: la fábrica la posee RulesEngine (o el test).
    explicit MoveApplier(const PieceFactory& factory) : factory_(factory) {}

    GameState apply(const GameState& state, const Move& move) const;

private:
    const PieceFactory& factory_;
};

}  // namespace chess::domain
