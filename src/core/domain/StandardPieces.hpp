#pragma once

#include "domain/PieceRegistry.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

// Patrón: Factory + Registry — ver docs/arquitectura.md §3.4
// Registra las 6 piezas clásicas componiendo reglas existentes (composición, §3.3):
//   Torre = SlidingRule(orthogonals)    Alfil = SlidingRule(diagonals)   Reina = SlidingRule(all)
//   Caballo = LeaperRule(knightJumps)   Rey = LeaperRule(all)            Peón = PawnRule
// Una pieza nueva NO se agrega acá: se registra aparte en el composition root.
class StandardPieces {
public:
    StandardPieces() = delete;

    static inline const PieceTypeId kPawn{"pawn"};
    static inline const PieceTypeId kRook{"rook"};
    static inline const PieceTypeId kKnight{"knight"};
    static inline const PieceTypeId kBishop{"bishop"};
    static inline const PieceTypeId kQueen{"queen"};
    static inline const PieceTypeId kKing{"king"};

    static void registerAll(PieceRegistry& registry);
};

}  // namespace chess::domain
