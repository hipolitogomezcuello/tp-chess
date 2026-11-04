#include "domain/BoardDimensions.hpp"

#include <stdexcept>

namespace chess::domain {

BoardDimensions::BoardDimensions(int files, int ranks) : files_(files), ranks_(ranks) {
    if (files <= 0 || ranks <= 0) {
        throw std::invalid_argument("BoardDimensions: el tablero necesita al menos una casilla por lado");
    }
}

bool BoardDimensions::contains(Position position) const {
    return position.file() >= 0 && position.file() < files_
        && position.rank() >= 0 && position.rank() < ranks_;
}

}  // namespace chess::domain
