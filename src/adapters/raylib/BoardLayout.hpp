#pragma once

#include <optional>

#include "domain/BoardDimensions.hpp"
#include "domain/Position.hpp"

namespace chess::adapters::raylib {

class BoardLayout {
public:
    struct Point {
        int x;
        int y;

        friend bool operator==(Point a, Point b) { return a.x == b.x && a.y == b.y; }
    };

    BoardLayout(domain::BoardDimensions dimensions, int originX, int originY, int boardPixels);

    int squareSize() const { return squareSize_; }
    Point topLeftOf(domain::Position position) const;

    std::optional<domain::Position> squareAt(int x, int y) const;

private:
    bool isLeftOfOrAboveOrigin(int x, int y) const;

    domain::BoardDimensions dimensions_;
    int originX_;
    int originY_;
    int squareSize_;
};

}
