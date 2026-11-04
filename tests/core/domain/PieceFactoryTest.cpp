#include <gtest/gtest.h>

#include "support/PieceTestSupport.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"

using chess::domain::Color;
using chess::domain::PieceFactory;
using chess::domain::PieceRegistry;
using chess::domain::PieceTypeId;
using chess::testing::makeBareDefinition;

TEST(PieceFactoryTest, CreatesAPieceSharingTheRegisteredDefinition) {
    PieceRegistry registry;
    const auto knight = makeBareDefinition("knight", 'N');
    registry.add(knight);
    const PieceFactory factory(registry);

    const auto piece = factory.create(PieceTypeId("knight"), Color::Black);

    EXPECT_EQ(&piece.definition(), knight.get());
    EXPECT_EQ(piece.color(), Color::Black);
    EXPECT_FALSE(piece.hasMoved());
}

TEST(PieceFactoryTest, SeesDefinitionsRegisteredAfterItWasBuilt) {
    PieceRegistry registry;
    const PieceFactory factory(registry);
    registry.add(makeBareDefinition("archbishop", 'A'));

    EXPECT_EQ(factory.create(PieceTypeId("archbishop"), Color::White).symbol(), 'A');
}

TEST(PieceFactoryTest, RejectsUnknownTypes) {
    const PieceRegistry registry;
    const PieceFactory factory(registry);
    EXPECT_THROW(factory.create(PieceTypeId("dragon"), Color::White), std::invalid_argument);
}
