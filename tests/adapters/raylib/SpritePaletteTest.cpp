#include "adapters/raylib/SpritePalette.hpp"

#include <gtest/gtest.h>

using chess::adapters::raylib::paletteColor;
using chess::adapters::raylib::Rgba;
using chess::adapters::raylib::SpritePixel;
using chess::domain::Color;

namespace {

int brightness(const Rgba& color) {
    return (color.r + color.g + color.b) / 3;
}

}

TEST(SpritePaletteTest, TransparentPixelsAreNotPainted) {
    EXPECT_FALSE(paletteColor(SpritePixel::Transparent, Color::White).has_value());
    EXPECT_FALSE(paletteColor(SpritePixel::Transparent, Color::Black).has_value());
}

TEST(SpritePaletteTest, BothSidesHaveADarkOutlineSoTheyReadOnLightAndDarkSquares) {
    EXPECT_LT(brightness(*paletteColor(SpritePixel::Outline, Color::White)), 60);
    EXPECT_LT(brightness(*paletteColor(SpritePixel::Outline, Color::Black)), 60);
}

TEST(SpritePaletteTest, WhitePiecesAreFilledLightAndBlackPiecesDark) {
    EXPECT_GT(brightness(*paletteColor(SpritePixel::Fill, Color::White)), 200);
    EXPECT_LT(brightness(*paletteColor(SpritePixel::Fill, Color::Black)), 100);
}

TEST(SpritePaletteTest, TheShadeIsDarkerThanTheFillButLighterThanTheOutline) {
    for (const Color side : {Color::White, Color::Black}) {
        const int shade = brightness(*paletteColor(SpritePixel::Shade, side));
        EXPECT_LT(shade, brightness(*paletteColor(SpritePixel::Fill, side)));
        EXPECT_GT(shade, brightness(*paletteColor(SpritePixel::Outline, side)));
    }
}

TEST(SpritePaletteTest, PaintedPixelsAreOpaque) {
    EXPECT_EQ(paletteColor(SpritePixel::Fill, Color::White)->a, 255);
    EXPECT_EQ(paletteColor(SpritePixel::Outline, Color::Black)->a, 255);
}
