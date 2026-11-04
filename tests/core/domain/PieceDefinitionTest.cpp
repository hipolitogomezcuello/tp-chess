#include <gtest/gtest.h>

#include "support/PieceTestSupport.hpp"

using chess::domain::PieceTraits;
using chess::domain::PieceTypeId;
using chess::testing::makeBareDefinition;

TEST(PieceDefinitionTest, ExposesIdSymbolAndTraits) {
    PieceTraits traits;
    traits.isRoyal = true;
    const auto king = makeBareDefinition("king", 'K', traits);

    EXPECT_EQ(king->id(), PieceTypeId("king"));
    EXPECT_EQ(king->symbol(), 'K');
    EXPECT_TRUE(king->traits().isRoyal);
    EXPECT_FALSE(king->traits().canCastleWith);
    EXPECT_FALSE(king->traits().isPromotable);
    EXPECT_TRUE(king->rules().empty());
}

TEST(PieceDefinitionTest, TraitsDefaultToFalse) {
    const PieceTraits traits;
    EXPECT_FALSE(traits.isRoyal);
    EXPECT_FALSE(traits.canCastleWith);
    EXPECT_FALSE(traits.isPromotable);
}
