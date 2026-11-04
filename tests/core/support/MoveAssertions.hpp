#pragma once

// Helpers para comparar conjuntos de jugadas o casillas sin depender del orden de generación.
#include <algorithm>
#include <initializer_list>
#include <string>
#include <vector>

#include "domain/Move.hpp"
#include "domain/Position.hpp"
#include "support/Squares.hpp"

namespace chess::testing {

inline std::vector<domain::Position> sorted(std::vector<domain::Position> positions) {
    std::sort(positions.begin(), positions.end());
    return positions;
}

// Casillas de destino de las jugadas, ordenadas.
inline std::vector<domain::Position> destinations(const std::vector<domain::Move>& moves) {
    std::vector<domain::Position> result;
    for (const auto& move : moves) {
        result.push_back(move.to());
    }
    return sorted(result);
}

// squares({"a1", "b2"}) → posiciones ordenadas, para comparar con destinations(...).
inline std::vector<domain::Position> squares(std::initializer_list<std::string> names) {
    std::vector<domain::Position> result;
    for (const auto& name : names) {
        result.push_back(sq(name));
    }
    return sorted(result);
}

// La jugada que va a `to`, o nullptr.
inline const domain::Move* findMoveTo(const std::vector<domain::Move>& moves, domain::Position to) {
    const auto it = std::find_if(moves.begin(), moves.end(), [to](const domain::Move& m) { return m.to() == to; });
    return it == moves.end() ? nullptr : &*it;
}

}  // namespace chess::testing
