#pragma once

#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/Offset.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.2
// Salta directo a cada destino fijo, sin importar lo que haya en el medio.
// Caballo = knightJumps, Rey = all (las 8 vecinas).
class LeaperRule : public MovementRule {
public:
    explicit LeaperRule(std::vector<Offset> offsets);

    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;

private:
    std::vector<Offset> offsets_;
};

}  // namespace chess::domain
