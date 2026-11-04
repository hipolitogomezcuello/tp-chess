#pragma once

#include <vector>

#include "domain/Move.hpp"
#include "domain/MoveContext.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Convierte las casillas que una pieza alcanza en jugadas: vacía → Normal, enemiga → Capture,
// propia → se descarta. Lo comparten las reglas cuyo movimiento coincide con su ataque
// (SlidingRule, LeaperRule); el peón no, porque avanza distinto de como captura.
std::vector<Move> movesOntoReachableSquares(const MoveContext& context, const std::vector<Position>& squares);

}  // namespace chess::domain
