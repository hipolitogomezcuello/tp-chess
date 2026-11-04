#include <gtest/gtest.h>

#include "domain/BoardDimensions.hpp"
#include "domain/Position.hpp"

using chess::domain::BoardDimensions;
using chess::domain::Position;

TEST(BoardDimensionsTest, DefaultsToEightByEight) {
    const BoardDimensions dimensions;
    EXPECT_EQ(dimensions.files(), 8);
    EXPECT_EQ(dimensions.ranks(), 8);
}

TEST(BoardDimensionsTest, AcceptsOtherSizes) {
    const BoardDimensions dimensions(10, 12);
    EXPECT_EQ(dimensions.files(), 10);
    EXPECT_EQ(dimensions.ranks(), 12);
}

TEST(BoardDimensionsTest, ContainsCornersOfTheBoard) {
    const BoardDimensions dimensions;
    EXPECT_TRUE(dimensions.contains(Position(0, 0)));
    EXPECT_TRUE(dimensions.contains(Position(7, 7)));
}

TEST(BoardDimensionsTest, DoesNotContainPositionsOutside) {
    const BoardDimensions dimensions;
    EXPECT_FALSE(dimensions.contains(Position(-1, 0)));
    EXPECT_FALSE(dimensions.contains(Position(0, -1)));
    EXPECT_FALSE(dimensions.contains(Position(8, 0)));
    EXPECT_FALSE(dimensions.contains(Position(0, 8)));
}

TEST(BoardDimensionsTest, TenByTenContainsTheExtraSquares) {
    const BoardDimensions dimensions(10, 10);
    EXPECT_TRUE(dimensions.contains(Position(9, 9)));
    EXPECT_FALSE(dimensions.contains(Position(10, 9)));
}

TEST(BoardDimensionsTest, RejectsNonPositiveSizes) {
    EXPECT_THROW(BoardDimensions(0, 8), std::invalid_argument);
    EXPECT_THROW(BoardDimensions(8, -1), std::invalid_argument);
}

TEST(BoardDimensionsTest, EqualityComparesBothSizes) {
    EXPECT_EQ(BoardDimensions(8, 8), BoardDimensions());
    EXPECT_NE(BoardDimensions(8, 8), BoardDimensions(10, 10));
}
