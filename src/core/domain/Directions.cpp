#include "domain/Directions.hpp"

namespace chess::domain {

std::vector<Offset> Directions::orthogonals() {
    return {Offset(1, 0), Offset(-1, 0), Offset(0, 1), Offset(0, -1)};
}

std::vector<Offset> Directions::diagonals() {
    return {Offset(1, 1), Offset(1, -1), Offset(-1, 1), Offset(-1, -1)};
}

std::vector<Offset> Directions::all() {
    auto result = orthogonals();
    const auto diagonal = diagonals();
    result.insert(result.end(), diagonal.begin(), diagonal.end());
    return result;
}

std::vector<Offset> Directions::knightJumps() {
    return {Offset(1, 2), Offset(2, 1), Offset(2, -1), Offset(1, -2),
            Offset(-1, -2), Offset(-2, -1), Offset(-2, 1), Offset(-1, 2)};
}

}  // namespace chess::domain
