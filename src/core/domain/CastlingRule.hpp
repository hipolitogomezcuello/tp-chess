#pragma once

#include <vector>

#include "domain/MovementRule.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.2
// Enroque: si la pieza (el rey) nunca se movió, mira a izquierda y derecha sobre su fila. Si la primera
// pieza que encuentra es propia, tiene el trait canCastleWith y tampoco se movió, el rey va dos casillas
// hacia ella y ella salta a la casilla que el rey cruzó.
// Solo mira la geometría: que el rey no esté en jaque ni cruce casillas atacadas lo verifica
// LegalMoveFilter (D-07). Los derechos se derivan de hasMoved (D-06) y no asume 8×8 (D-09).
// No ataca ninguna casilla: el enroque nunca captura.
class CastlingRule : public MovementRule {
public:
    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;
};

}  // namespace chess::domain
