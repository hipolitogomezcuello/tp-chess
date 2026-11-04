#pragma once

#include <vector>

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Primer paso del pipeline de validación (D-04): junta las jugadas PSEUDO-LEGALES recorriendo las
// MovementRule de la pieza. No pregunta el tipo de pieza: una pieza nueva funciona sin tocar esto.
class MoveGenerator {
public:
    // Jugadas de la pieza en `from` (vacío si la casilla está vacía), sin importar el turno.
    std::vector<Move> movesFrom(const GameState& state, Position from) const;

    // Jugadas de todas las piezas del bando que mueve.
    std::vector<Move> allMoves(const GameState& state) const;
};

}  // namespace chess::domain
