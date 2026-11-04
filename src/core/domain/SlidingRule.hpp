#pragma once

#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/Offset.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.2
// Avanza en cada dirección casilla por casilla hasta el borde o la primera pieza:
// si es enemiga la captura, si es propia se detiene antes.
// Torre = orthogonals, Alfil = diagonals, Reina = all.
class SlidingRule : public MovementRule {
public:
    explicit SlidingRule(std::vector<Offset> directions);

    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;

private:
    std::vector<Offset> directions_;
};

}  // namespace chess::domain
