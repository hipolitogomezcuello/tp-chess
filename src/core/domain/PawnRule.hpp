#pragma once

#include <vector>

#include "domain/MovementRule.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.2
// Movimiento del peón: avanza recto (una casilla, o dos si nunca se movió) y captura en
// diagonal hacia adelante. "Adelante" depende del color: las blancas suben, las negras bajan.
// El doble avance usa Piece::hasMoved, no "estar en la segunda fila" (D-09).
// Si la jugada llega a la última fila (la de las dimensiones reales) se marca como Promotion, sin
// elegir la pieza: la elige el jugador y la valida RulesEngine::canPromoteTo (D-08).
// Al paso: si la casilla diagonal es GameState::enPassantTarget (la que cruzó un peón enemigo en su
// doble avance de la jugada anterior) y ese peón está al lado, captura llegando a una casilla vacía.
class PawnRule : public MovementRule {
public:
    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;
};

}  // namespace chess::domain
