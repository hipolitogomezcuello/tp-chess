#include "adapters/raylib/SpriteCatalog.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

#include "adapters/raylib/PixelSprite.hpp"
#include "adapters/raylib/StandardSprites.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/StandardPieces.hpp"

using chess::adapters::raylib::PixelSprite;
using chess::adapters::raylib::SpriteCatalog;
using chess::adapters::raylib::StandardSprites;
using chess::domain::PieceTypeId;
using chess::domain::StandardPieces;

namespace {

const PixelSprite kDot({"o"});
const PieceTypeId kArchbishop("archbishop");

}

TEST(SpriteCatalogTest, ARegisteredSpriteIsFoundByItsPieceType) {
    SpriteCatalog catalog;
    catalog.add(kArchbishop, kDot);

    ASSERT_NE(catalog.find(kArchbishop), nullptr);
    EXPECT_EQ(catalog.find(kArchbishop)->width(), 1);
}

TEST(SpriteCatalogTest, APieceWithoutASpriteIsNotFoundSoItIsDrawnWithItsLetter) {
    const SpriteCatalog catalog;

    EXPECT_EQ(catalog.find(kArchbishop), nullptr);
}

TEST(SpriteCatalogTest, RegisteringTheSameTypeTwiceIsRejected) {
    SpriteCatalog catalog;
    catalog.add(kArchbishop, kDot);

    EXPECT_THROW(catalog.add(kArchbishop, kDot), std::invalid_argument);
}

TEST(StandardSpritesTest, TheSixClassicPiecesHaveASixteenBySixteenSprite) {
    SpriteCatalog catalog;
    StandardSprites::registerAll(catalog);

    for (const PieceTypeId& type : {StandardPieces::kPawn, StandardPieces::kRook, StandardPieces::kKnight,
                                    StandardPieces::kBishop, StandardPieces::kQueen, StandardPieces::kKing}) {
        const PixelSprite* sprite = catalog.find(type);
        ASSERT_NE(sprite, nullptr) << type.value();
        EXPECT_EQ(sprite->width(), 16) << type.value();
        EXPECT_EQ(sprite->height(), 16) << type.value();
    }
}

TEST(StandardSpritesTest, AnInventedPieceIsLeftWithoutASprite) {
    SpriteCatalog catalog;
    StandardSprites::registerAll(catalog);

    EXPECT_EQ(catalog.find(kArchbishop), nullptr);
}

TEST(StandardSpritesTest, AnInventedPieceCanGetItsSpriteWithoutTouchingTheStandardOnes) {
    SpriteCatalog catalog;
    StandardSprites::registerAll(catalog);
    catalog.add(kArchbishop, kDot);

    EXPECT_NE(catalog.find(kArchbishop), nullptr);
    EXPECT_NE(catalog.find(StandardPieces::kKing), nullptr);
}
