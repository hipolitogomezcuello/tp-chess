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
// La única excepción es el enroque (D-07): además, el rey no puede estar en jaque ni cruzar una
// casilla atacada. Vive acá y no en CastlingRule porque todo lo del jaque está en un solo lugar.
class LegalMoveFilter {
public:
    // Referencias sin propiedad: los dueños son RulesEngine (o el test).
    LegalMoveFilter(const MoveApplier& applier, const AttackDetector& detector)
        : applier_(applier), detector_(detector) {}

    std::vector<Move> filter(const GameState& state, const std::vector<Move>& moves) const;

private:
    bool leavesOwnKingSafe(const GameState& state, const Move& move) const;
    // El rey no está en jaque y ninguna casilla que cruza (sin contar el destino) está atacada.
    bool kingPathIsSafe(const GameState& state, const Move& castling) const;

    const MoveApplier& applier_;
    const AttackDetector& detector_;
};

}  // namespace chess::domain
