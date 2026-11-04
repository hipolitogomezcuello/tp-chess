#include <gtest/gtest.h>

#include "domain/Color.hpp"

using chess::domain::Color;
using chess::domain::opposite;

TEST(ColorTest, OppositeOfWhiteIsBlack) {
    EXPECT_EQ(opposite(Color::White), Color::Black);
}

TEST(ColorTest, OppositeOfBlackIsWhite) {
    EXPECT_EQ(opposite(Color::Black), Color::White);
}
