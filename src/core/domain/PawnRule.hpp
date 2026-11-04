#pragma once

#include <vector>

#include "domain/MovementRule.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.2
// Movimiento del peón: avanza recto (una casilla, o dos si nunca se movió) y captura en
// diagonal hacia adelante. "Adelante" depende del color: las blancas suben, las negras bajan.
// El doble avance usa Piece::hasMoved, no "estar en la segunda fila" (D-09).
// Pendiente para la fase 7: captura al paso y marcar la promoción en la última fila (D-08).
class PawnRule : public MovementRule {
public:
    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;
};

}  // namespace chess::domain
