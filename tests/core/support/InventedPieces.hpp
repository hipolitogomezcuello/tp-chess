#pragma once

// Piezas inventadas para los tests de extensibilidad (ensayo de la defensa, docs/arquitectura.md §9).
// Se arman SOLO componiendo reglas que ya existen y se registran desde afuera: ninguna clase del
// núcleo se modifica para agregarlas.
//   Arzobispo (A) = Alfil + Caballo        Canciller (C) = Torre + Caballo
#include <memory>

#include "domain/Directions.hpp"
#include "domain/LeaperRule.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/PieceTraits.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/SlidingRule.hpp"
#include "support/PieceTestSupport.hpp"

namespace chess::testing {

inline const domain::PieceTypeId kArchbishop{"archbishop"};
inline const domain::PieceTypeId kChancellor{"chancellor"};

inline void registerInventedPieces(domain::PieceRegistry& registry) {
    using domain::Directions;
    using domain::LeaperRule;
    using domain::SlidingRule;

    domain::PieceTraits promotable;
    promotable.isPromotable = true;

    registry.add(makeDefinition(kArchbishop.value(), 'A',
                                rulesOf(std::make_unique<SlidingRule>(Directions::diagonals()),
                                        std::make_unique<LeaperRule>(Directions::knightJumps())),
                                promotable));
    registry.add(makeDefinition(kChancellor.value(), 'C',
                                rulesOf(std::make_unique<SlidingRule>(Directions::orthogonals()),
                                        std::make_unique<LeaperRule>(Directions::knightJumps())),
                                promotable));
}

}  // namespace chess::testing
