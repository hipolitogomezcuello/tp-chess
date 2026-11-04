#pragma once

#include <vector>

#include "domain/Offset.hpp"

namespace chess::domain {

// Helpers sin estado para componer reglas: SlidingRule(Directions::diagonals()),
// LeaperRule(Directions::knightJumps())… Una pieza nueva reutiliza estas listas.
class Directions {
public:
    Directions() = delete;

    static std::vector<Offset> orthogonals();
    static std::vector<Offset> diagonals();
    static std::vector<Offset> all();  // ortogonales + diagonales (Reina; o las 8 vecinas del Rey)
    static std::vector<Offset> knightJumps();
};

}  // namespace chess::domain
