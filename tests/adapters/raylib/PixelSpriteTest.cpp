#include "adapters/raylib/PixelSprite.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

using chess::adapters::raylib::PixelSprite;
using chess::adapters::raylib::SpritePixel;

namespace {

const PixelSprite kArrow({
    ".o..",
    "ofo.",
    "osfo",
    ".o..",
});

}

TEST(PixelSpriteTest, ItsSizeIsTheSizeOfTheRows) {
    EXPECT_EQ(kArrow.width(), 4);
    EXPECT_EQ(kArrow.height(), 4);
}

TEST(PixelSpriteTest, EachCharacterIsOnePixelReadFromTheTopLeft) {
    EXPECT_EQ(kArrow.pixelAt(0, 0), SpritePixel::Transparent);
    EXPECT_EQ(kArrow.pixelAt(1, 0), SpritePixel::Outline);
    EXPECT_EQ(kArrow.pixelAt(1, 1), SpritePixel::Fill);
    EXPECT_EQ(kArrow.pixelAt(1, 2), SpritePixel::Shade);
}

TEST(PixelSpriteTest, RowsOfDifferentWidthAreRejected) {
    EXPECT_THROW(PixelSprite({"oo", "o"}), std::invalid_argument);
}

TEST(PixelSpriteTest, AnEmptySpriteIsRejected) {
    EXPECT_THROW(PixelSprite({}), std::invalid_argument);
    EXPECT_THROW(PixelSprite({""}), std::invalid_argument);
}

TEST(PixelSpriteTest, AnUnknownPixelCodeIsRejected) {
    EXPECT_THROW(PixelSprite({"ox"}), std::invalid_argument);
}

TEST(PixelSpriteTest, ItScalesByTheLargestWholeFactorThatLeavesASpritePixelOfMarginOnEachSide) {
    const PixelSprite sixteen(std::vector<std::string>(16, std::string(16, 'f')));

    EXPECT_EQ(sixteen.placedIn(0, 0, 80).scale, 4);
    EXPECT_EQ(sixteen.placedIn(0, 0, 64).scale, 3);
    EXPECT_EQ(sixteen.placedIn(0, 0, 90).scale, 5);
}

TEST(PixelSpriteTest, ATinyBoxStillDrawsTheSpriteAtScaleOne) {
    EXPECT_EQ(kArrow.placedIn(0, 0, 3).scale, 1);
}

TEST(PixelSpriteTest, TheScaledSpriteIsCenteredInTheBox) {
    const PixelSprite sixteen(std::vector<std::string>(16, std::string(16, 'f')));

    const auto placement = sixteen.placedIn(100, 200, 80);

    EXPECT_EQ(placement.x, 108);
    EXPECT_EQ(placement.y, 208);
}
