#pragma once

#include "domain/PieceRegistry.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

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

}
