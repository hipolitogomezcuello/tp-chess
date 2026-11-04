#include <gtest/gtest.h>

#include <algorithm>

#include "support/PieceTestSupport.hpp"
#include "domain/PieceRegistry.hpp"

using chess::domain::PieceRegistry;
using chess::domain::PieceTraits;
using chess::domain::PieceTypeId;
using chess::testing::makeBareDefinition;

TEST(PieceRegistryTest, FindsARegisteredDefinition) {
    PieceRegistry registry;
    const auto rook = makeBareDefinition("rook", 'R');
    registry.add(rook);

    EXPECT_TRUE(registry.contains(PieceTypeId("rook")));
    EXPECT_EQ(registry.find(PieceTypeId("rook")), rook);
}

TEST(PieceRegistryTest, ReturnsNullForAnUnknownType) {
    const PieceRegistry registry;
    EXPECT_FALSE(registry.contains(PieceTypeId("dragon")));
    EXPECT_EQ(registry.find(PieceTypeId("dragon")), nullptr);
}

TEST(PieceRegistryTest, RejectsDuplicatedIds) {
    PieceRegistry registry;
    registry.add(makeBareDefinition("rook", 'R'));
    EXPECT_THROW(registry.add(makeBareDefinition("rook", 'T')), std::invalid_argument);
}

TEST(PieceRegistryTest, RejectsNullDefinitions) {
    PieceRegistry registry;
    EXPECT_THROW(registry.add(nullptr), std::invalid_argument);
}

TEST(PieceRegistryTest, ListsOnlyPromotableTypes) {
    PieceRegistry registry;
    PieceTraits promotable;
    promotable.isPromotable = true;
    registry.add(makeBareDefinition("queen", 'Q', promotable));
    registry.add(makeBareDefinition("archbishop", 'A', promotable));
    registry.add(makeBareDefinition("king", 'K'));

    auto types = registry.promotableTypes();
    std::sort(types.begin(), types.end());
    ASSERT_EQ(types.size(), 2u);
    EXPECT_EQ(types[0], PieceTypeId("archbishop"));
    EXPECT_EQ(types[1], PieceTypeId("queen"));
}
