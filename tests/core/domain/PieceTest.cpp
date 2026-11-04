#include <gtest/gtest.h>

#include "support/PieceTestSupport.hpp"
#include "domain/Piece.hpp"

using chess::domain::Color;
using chess::domain::Piece;
using chess::domain::PieceTypeId;
using chess::testing::makeBareDefinition;

TEST(PieceTest, DelegatesTypeSymbolAndTraitsToItsDefinition) {
    const auto rook = makeBareDefinition("rook", 'R');
    const Piece piece(rook, Color::Black);

    EXPECT_EQ(piece.type(), PieceTypeId("rook"));
    EXPECT_EQ(piece.symbol(), 'R');
    EXPECT_EQ(piece.color(), Color::Black);
    EXPECT_EQ(&piece.definition(), rook.get());
    EXPECT_EQ(&piece.traits(), &rook->traits());
}

TEST(PieceTest, IsNewByDefault) {
    const Piece piece(makeBareDefinition("rook", 'R'), Color::White);
    EXPECT_FALSE(piece.hasMoved());
}

TEST(PieceTest, WithMovedReturnsAMovedCopyWithoutChangingTheOriginal) {
    const Piece piece(makeBareDefinition("rook", 'R'), Color::White);
    const Piece moved = piece.withMoved();

    EXPECT_TRUE(moved.hasMoved());
    EXPECT_FALSE(piece.hasMoved());
    EXPECT_EQ(moved.type(), piece.type());
    EXPECT_EQ(moved.color(), piece.color());
}

TEST(PieceTest, EqualityComparesTypeColorAndMovedFlag) {
    const auto rook = makeBareDefinition("rook", 'R');
    const Piece white(rook, Color::White);

    EXPECT_EQ(white, Piece(rook, Color::White));
    EXPECT_NE(white, Piece(rook, Color::Black));
    EXPECT_NE(white, white.withMoved());
    EXPECT_NE(white, Piece(makeBareDefinition("knight", 'N'), Color::White));
}

TEST(PieceTest, RejectsANullDefinition) {
    EXPECT_THROW(Piece(nullptr, Color::White), std::invalid_argument);
}
