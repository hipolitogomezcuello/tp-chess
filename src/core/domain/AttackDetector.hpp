#pragma once

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Único lugar donde vive la noción de "jaque".
// Usa MovementRule::attackedSquares (no candidateMoves): el peón ataca en diagonal aunque avance recto.
// isInCheck busca las piezas con el trait isRoyal, no "el rey": una pieza real inventada funciona igual.
class AttackDetector {
public:
    bool isSquareAttacked(const GameState& state, Position square, Color byColor) const;

    // true si alguna pieza isRoyal de `color` está atacada. Sin piezas reales, nunca hay jaque.
    bool isInCheck(const GameState& state, Color color) const;
};

}  // namespace chess::domain
