#pragma once

#include <vector>

#include "domain/Offset.hpp"

namespace chess::domain {

class Directions {
public:
    Directions() = delete;

    static std::vector<Offset> orthogonals();
    static std::vector<Offset> diagonals();
    static std::vector<Offset> all();
    static std::vector<Offset> knightJumps();
};

}
