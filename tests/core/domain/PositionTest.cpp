#include <gtest/gtest.h>

#include "domain/Offset.hpp"
#include "domain/Position.hpp"

using chess::domain::Offset;
using chess::domain::Position;

TEST(PositionTest, StoresFileAndRank) {
    const Position position(4, 1);
    EXPECT_EQ(position.file(), 4);
    EXPECT_EQ(position.rank(), 1);
}

TEST(PositionTest, AddingAnOffsetMovesThePosition) {
    EXPECT_EQ(Position(4, 1) + Offset(1, 2), Position(5, 3));
    EXPECT_EQ(Position(4, 1) + Offset(-1, -1), Position(3, 0));
}

TEST(PositionTest, AddingAnOffsetCanLeaveTheBoard) {
    // Position no conoce el tablero: puede quedar "afuera". Lo decide BoardDimensions.
    const Position outside = Position(0, 0) + Offset(-1, 0);
    EXPECT_EQ(outside.file(), -1);
}

TEST(PositionTest, EqualityComparesBothCoordinates) {
    EXPECT_EQ(Position(2, 3), Position(2, 3));
    EXPECT_NE(Position(2, 3), Position(3, 2));
}

TEST(PositionTest, LessThanGivesAStrictOrderUsableInMaps) {
    EXPECT_LT(Position(0, 5), Position(1, 0));
    EXPECT_LT(Position(1, 0), Position(1, 1));
    EXPECT_FALSE(Position(1, 1) < Position(1, 1));
}
