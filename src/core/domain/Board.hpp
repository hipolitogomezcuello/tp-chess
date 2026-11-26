#pragma once

#include <optional>
#include <vector>

#include "domain/BoardDimensions.hpp"
#include "domain/Color.hpp"
#include "domain/Piece.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

class Board {
public:
    explicit Board(BoardDimensions dimensions);

    const BoardDimensions& dimensions() const { return dimensions_; }

    std::optional<Piece> pieceAt(Position position) const;
    bool isEmpty(Position position) const;

    void place(Position position, Piece piece);
    void remove(Position position);

    void move(Position from, Position to);

    std::vector<Position> positionsOf(Color color) const;

private:
    std::size_t indexOf(Position position) const;

    BoardDimensions dimensions_;
    std::vector<std::optional<Piece>> squares_;
};

}
