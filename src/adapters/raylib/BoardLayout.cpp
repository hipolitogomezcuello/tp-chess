#include "adapters/raylib/BoardLayout.hpp"

#include <algorithm>

namespace chess::adapters::raylib {

using domain::Position;

BoardLayout::BoardLayout(domain::BoardDimensions dimensions, int originX, int originY, int boardPixels)
    : dimensions_(dimensions),
      originX_(originX),
      originY_(originY),
      squareSize_(boardPixels / std::max(dimensions.files(), dimensions.ranks())) {}

BoardLayout::Point BoardLayout::topLeftOf(Position position) const {
    const int rowFromTop = dimensions_.ranks() - 1 - position.rank();
    return {originX_ + position.file() * squareSize_, originY_ + rowFromTop * squareSize_};
}

std::optional<Position> BoardLayout::squareAt(int x, int y) const {
    if (x < originX_ || y < originY_) {
        return std::nullopt;  // antes de dividir: la división entera redondea hacia cero
    }
    const int file = (x - originX_) / squareSize_;
    const int rowFromTop = (y - originY_) / squareSize_;
    const Position position(file, dimensions_.ranks() - 1 - rowFromTop);
    if (!dimensions_.contains(position)) {
        return std::nullopt;
    }
    return position;
}

}  // namespace chess::adapters::raylib
