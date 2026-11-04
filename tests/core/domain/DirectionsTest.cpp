#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>

#include "domain/Directions.hpp"

using chess::domain::Directions;
using chess::domain::Offset;

namespace {

bool containsOffset(const std::vector<Offset>& offsets, Offset offset) {
    return std::find(offsets.begin(), offsets.end(), offset) != offsets.end();
}

}  // namespace

TEST(DirectionsTest, OrthogonalsAreTheFourStraightLines) {
    const auto offsets = Directions::orthogonals();
    ASSERT_EQ(offsets.size(), 4u);
    EXPECT_TRUE(containsOffset(offsets, Offset(1, 0)));
    EXPECT_TRUE(containsOffset(offsets, Offset(-1, 0)));
    EXPECT_TRUE(containsOffset(offsets, Offset(0, 1)));
    EXPECT_TRUE(containsOffset(offsets, Offset(0, -1)));
}

TEST(DirectionsTest, DiagonalsAreTheFourDiagonalLines) {
    const auto offsets = Directions::diagonals();
    ASSERT_EQ(offsets.size(), 4u);
    EXPECT_TRUE(containsOffset(offsets, Offset(1, 1)));
    EXPECT_TRUE(containsOffset(offsets, Offset(1, -1)));
    EXPECT_TRUE(containsOffset(offsets, Offset(-1, 1)));
    EXPECT_TRUE(containsOffset(offsets, Offset(-1, -1)));
}

TEST(DirectionsTest, AllCombinesOrthogonalsAndDiagonals) {
    EXPECT_EQ(Directions::all().size(), 8u);
}

TEST(DirectionsTest, KnightJumpsAreTheEightLShapes) {
    const auto offsets = Directions::knightJumps();
    ASSERT_EQ(offsets.size(), 8u);
    for (const auto offset : offsets) {
        const int a = std::abs(offset.dFile());
        const int b = std::abs(offset.dRank());
        EXPECT_TRUE((a == 1 && b == 2) || (a == 2 && b == 1));
    }
    EXPECT_TRUE(containsOffset(offsets, Offset(-2, -1)));
}
