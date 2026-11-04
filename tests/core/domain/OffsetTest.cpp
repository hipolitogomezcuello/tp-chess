#include <gtest/gtest.h>

#include "domain/Offset.hpp"

using chess::domain::Offset;

TEST(OffsetTest, StoresFileAndRankDeltas) {
    const Offset offset(2, -1);
    EXPECT_EQ(offset.dFile(), 2);
    EXPECT_EQ(offset.dRank(), -1);
}

TEST(OffsetTest, EqualOffsetsCompareEqual) {
    EXPECT_EQ(Offset(1, 1), Offset(1, 1));
    EXPECT_NE(Offset(1, 1), Offset(1, -1));
}
