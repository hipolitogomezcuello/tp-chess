#pragma once

#include <cstdlib>
#include <string>

#include "domain/Position.hpp"

namespace chess::testing {

inline domain::Position sq(const std::string& square) {
    const int file = square.at(0) - 'a';
    const int rank = std::atoi(square.c_str() + 1) - 1;
    return domain::Position(file, rank);
}

}
