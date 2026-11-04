#include <gtest/gtest.h>

#include <set>

#include "domain/PieceTypeId.hpp"

using chess::domain::PieceTypeId;

TEST(PieceTypeIdTest, StoresItsValue) {
    EXPECT_EQ(PieceTypeId("rook").value(), "rook");
}

TEST(PieceTypeIdTest, IdsWithTheSameValueAreEqual) {
    EXPECT_EQ(PieceTypeId("rook"), PieceTypeId("rook"));
    EXPECT_NE(PieceTypeId("rook"), PieceTypeId("knight"));
}

TEST(PieceTypeIdTest, IsOrderedSoItCanBeAMapKey) {
    const std::set<PieceTypeId> ids{PieceTypeId("rook"), PieceTypeId("archbishop"), PieceTypeId("rook")};
    EXPECT_EQ(ids.size(), 2u);
    EXPECT_EQ(ids.begin()->value(), "archbishop");
}
