#pragma once

#include "domain/Board.hpp"
#include "domain/PieceFactory.hpp"

namespace chess::domain {

// Patrón: Strategy — ver docs/arquitectura.md §3.5
// Arma el tablero inicial. Cambiar el tamaño o la disposición (10×10, Chess960…) = escribir otra
// implementación e inyectarla en el composition root; Board y el motor no cambian.
class StartingPositionProvider {
public:
    virtual ~StartingPositionProvider() = default;

    virtual Board createInitialBoard(const PieceFactory& factory) const = 0;
};

}  // namespace chess::domain
