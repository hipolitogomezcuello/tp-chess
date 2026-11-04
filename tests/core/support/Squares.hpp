#pragma once

// sq("e4") → Position(4, 3). Solo para que los tests se lean como ajedrez.
// La notación algebraica "de verdad" vive en el adaptador (AlgebraicNotation), no en el dominio.
#include <cstdlib>
#include <string>

#include "domain/Position.hpp"

namespace chess::testing {

inline domain::Position sq(const std::string& square) {
    const int file = square.at(0) - 'a';
    const int rank = std::atoi(square.c_str() + 1) - 1;
    return domain::Position(file, rank);
}

}  // namespace chess::testing
