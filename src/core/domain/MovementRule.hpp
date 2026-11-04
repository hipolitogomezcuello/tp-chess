#pragma once

#include <vector>

namespace chess::domain {

class Move;
class MoveContext;
class Position;

// Patrón: Strategy — ver docs/arquitectura.md §3.2
// Una forma de moverse (deslizarse, saltar, avanzar como peón…). Una pieza compone una o
// más reglas; no hay subclases de pieza.
class MovementRule {
public:
    virtual ~MovementRule() = default;

    // Jugadas pseudo-legales: respetan la geometría y la ocupación del tablero, pero no
    // verifican si dejan al propio rey en jaque (eso es LegalMoveFilter, ver D-04).
    virtual std::vector<Move> candidateMoves(const MoveContext& context) const = 0;

    // Casillas que la pieza ataca. Separado de candidateMoves porque el peón ataca en
    // diagonal pero avanza recto, y el enroque mueve al rey sin atacar.
    virtual std::vector<Position> attackedSquares(const MoveContext& context) const = 0;
};

}  // namespace chess::domain
