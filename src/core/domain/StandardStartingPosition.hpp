#pragma once

#include "domain/StartingPositionProvider.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.5
// Disposición clásica de 8×8: torre, caballo, alfil, reina, rey, alfil, caballo, torre en la
// primera fila de cada bando y una fila de peones delante. Requiere StandardPieces registradas.
class StandardStartingPosition : public StartingPositionProvider {
public:
    Board createInitialBoard(const PieceFactory& factory) const override;
};

}  // namespace chess::domain
