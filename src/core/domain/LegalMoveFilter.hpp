#pragma once

#include <vector>

#include "domain/AttackDetector.hpp"
#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/MoveApplier.hpp"

namespace chess::domain {

// Segundo paso del pipeline (D-04): de las jugadas pseudo-legales se queda con las LEGALES,
// o sea, las que no dejan en jaque al rey de quien mueve. Para eso aplica cada jugada sobre
// una copia y le pregunta al AttackDetector. Así salen gratis las piezas clavadas, el rey que no
// puede entrar en jaque y la obligación de salir del jaque, sin casos especiales.
class LegalMoveFilter {
public:
    // Referencias sin propiedad: los dueños son RulesEngine (o el test).
    LegalMoveFilter(const MoveApplier& applier, const AttackDetector& detector)
        : applier_(applier), detector_(detector) {}

    std::vector<Move> filter(const GameState& state, const std::vector<Move>& moves) const;

private:
    const MoveApplier& applier_;
    const AttackDetector& detector_;
};

}  // namespace chess::domain
