#include "domain/Board.hpp"

#include <stdexcept>
#include <utility>

namespace chess::domain {

Board::Board(BoardDimensions dimensions)
    : dimensions_(dimensions),
      squares_(static_cast<std::size_t>(dimensions.files()) * static_cast<std::size_t>(dimensions.ranks())) {}

std::optional<Piece> Board::pieceAt(Position position) const {
    return squares_[indexOf(position)];
}

bool Board::isEmpty(Position position) const {
    return !squares_[indexOf(position)].has_value();
}

void Board::place(Position position, Piece piece) {
    squares_[indexOf(position)] = std::move(piece);
}

void Board::remove(Position position) {
    squares_[indexOf(position)].reset();
}

void Board::move(Position from, Position to) {
    auto& origin = squares_[indexOf(from)];
    if (!origin) {
        throw std::invalid_argument("Board::move: no hay pieza en la casilla de origen");
    }
    if (from == to) {
        return;
    }
    squares_[indexOf(to)] = std::move(origin);
    origin.reset();
}

std::vector<Position> Board::positionsOf(Color color) const {
    std::vector<Position> result;
    for (int file = 0; file < dimensions_.files(); ++file) {
        for (int rank = 0; rank < dimensions_.ranks(); ++rank) {
            const Position position(file, rank);
            const auto& square = squares_[indexOf(position)];
            if (square && square->color() == color) {
                result.push_back(position);
            }
        }
    }
    return result;
}

std::size_t Board::indexOf(Position position) const {
    if (!dimensions_.contains(position)) {
        throw std::out_of_range("Board: posición fuera del tablero");
    }
    return static_cast<std::size_t>(position.rank()) * static_cast<std::size_t>(dimensions_.files())
         + static_cast<std::size_t>(position.file());
}

}  // namespace chess::domain
