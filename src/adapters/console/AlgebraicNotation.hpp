#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "domain/BoardDimensions.hpp"
#include "domain/Position.hpp"

namespace chess::adapters::console {

// Traduce "e4" ↔ Position(4, 3). Es una cuestión de presentación, por eso vive en el adaptador y
// no en el dominio (arquitectura §6.1). Columna = letra desde 'a', fila = número desde 1: con las
// dimensiones del tablero acepta también "j10" en un 10×10 (D-09).
class AlgebraicNotation {
public:
    AlgebraicNotation() = delete;

    // nullopt si el texto está mal formado o la casilla queda fuera del tablero.
    static std::optional<domain::Position> parse(std::string_view text, const domain::BoardDimensions& dims);

    static std::string format(domain::Position position);
};

}  // namespace chess::adapters::console
