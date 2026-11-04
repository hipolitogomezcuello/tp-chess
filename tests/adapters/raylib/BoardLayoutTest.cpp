#include "adapters/raylib/BoardLayout.hpp"

#include <gtest/gtest.h>

#include "domain/BoardDimensions.hpp"
#include "support/Squares.hpp"

using chess::adapters::raylib::BoardLayout;
using chess::domain::BoardDimensions;
using chess::testing::sq;

namespace {

// Tablero de 8×8 dibujado en un cuadrado de 400 px que empieza en (20, 30): casillas de 50 px.
const BoardLayout kLayout(BoardDimensions(), 20, 30, 400);

}  // namespace

TEST(BoardLayoutTest, SquaresSplitTheAvailableSpaceByTheLargestSide) {
    EXPECT_EQ(kLayout.squareSize(), 50);
    EXPECT_EQ(BoardLayout(BoardDimensions(10, 10), 0, 0, 400).squareSize(), 40);
}

TEST(BoardLayoutTest, BlackIsDrawnOnTopSoRankOneIsTheBottomRow) {
    EXPECT_EQ(kLayout.topLeftOf(sq("a8")), BoardLayout::Point({20, 30}));
    EXPECT_EQ(kLayout.topLeftOf(sq("a1")), BoardLayout::Point({20, 380}));
    EXPECT_EQ(kLayout.topLeftOf(sq("h1")), BoardLayout::Point({370, 380}));
}

TEST(BoardLayoutTest, APixelInsideASquareMapsToThatSquare) {
    EXPECT_EQ(kLayout.squareAt(20, 30), sq("a8"));
    EXPECT_EQ(kLayout.squareAt(69, 79), sq("a8"));
    EXPECT_EQ(kLayout.squareAt(70, 79), sq("b8"));
    EXPECT_EQ(kLayout.squareAt(419, 429), sq("h1"));
    EXPECT_EQ(kLayout.squareAt(245, 255), sq("e4"));
}

TEST(BoardLayoutTest, PixelsOutsideTheBoardMapToNoSquare) {
    EXPECT_FALSE(kLayout.squareAt(19, 100).has_value());
    EXPECT_FALSE(kLayout.squareAt(100, 29).has_value());
    EXPECT_FALSE(kLayout.squareAt(420, 100).has_value());
    EXPECT_FALSE(kLayout.squareAt(100, 430).has_value());
}

TEST(BoardLayoutTest, WorksForBoardsOfAnySize) {
    const BoardLayout layout(BoardDimensions(10, 10), 0, 0, 400);

    EXPECT_EQ(layout.squareAt(399, 0), sq("j10"));
    EXPECT_EQ(layout.topLeftOf(sq("a1")), BoardLayout::Point({0, 360}));
}
