#include "adapters/console/AlgebraicNotation.hpp"

#include <gtest/gtest.h>

#include "domain/BoardDimensions.hpp"
#include "domain/Position.hpp"

using chess::adapters::console::AlgebraicNotation;
using chess::domain::BoardDimensions;
using chess::domain::Position;

TEST(AlgebraicNotationTest, ParsesTheCornersOfAStandardBoard) {
    const BoardDimensions dims;

    EXPECT_EQ(AlgebraicNotation::parse("a1", dims), Position(0, 0));
    EXPECT_EQ(AlgebraicNotation::parse("h8", dims), Position(7, 7));
    EXPECT_EQ(AlgebraicNotation::parse("e4", dims), Position(4, 3));
}

TEST(AlgebraicNotationTest, AcceptsUppercaseFiles) {
    EXPECT_EQ(AlgebraicNotation::parse("E2", BoardDimensions()), Position(4, 1));
}

TEST(AlgebraicNotationTest, RejectsSquaresOutsideTheBoard) {
    const BoardDimensions dims;

    EXPECT_FALSE(AlgebraicNotation::parse("i1", dims).has_value());
    EXPECT_FALSE(AlgebraicNotation::parse("a9", dims).has_value());
    EXPECT_FALSE(AlgebraicNotation::parse("a0", dims).has_value());
}

TEST(AlgebraicNotationTest, RejectsMalformedText) {
    const BoardDimensions dims;

    EXPECT_FALSE(AlgebraicNotation::parse("", dims).has_value());
    EXPECT_FALSE(AlgebraicNotation::parse("e", dims).has_value());
    EXPECT_FALSE(AlgebraicNotation::parse("44", dims).has_value());
    EXPECT_FALSE(AlgebraicNotation::parse("e4x", dims).has_value());
    EXPECT_FALSE(AlgebraicNotation::parse("e-4", dims).has_value());
}

TEST(AlgebraicNotationTest, ParsesTwoDigitRanksOnABiggerBoard) {
    const BoardDimensions dims(10, 10);

    EXPECT_EQ(AlgebraicNotation::parse("j10", dims), Position(9, 9));
    EXPECT_FALSE(AlgebraicNotation::parse("j10", BoardDimensions()).has_value());
}

TEST(AlgebraicNotationTest, FormatsPositions) {
    EXPECT_EQ(AlgebraicNotation::format(Position(0, 0)), "a1");
    EXPECT_EQ(AlgebraicNotation::format(Position(4, 3)), "e4");
    EXPECT_EQ(AlgebraicNotation::format(Position(9, 9)), "j10");
}
