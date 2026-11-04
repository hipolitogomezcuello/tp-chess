#include <gtest/gtest.h>

#include "domain/Move.hpp"
#include "support/Squares.hpp"

using chess::domain::Move;
using chess::domain::MoveKind;
using chess::domain::PieceTypeId;
using chess::testing::sq;

TEST(MoveTest, StoresOriginDestinationAndKind) {
    const Move move(sq("e2"), sq("e4"), MoveKind::DoublePawnPush);
    EXPECT_EQ(move.from(), sq("e2"));
    EXPECT_EQ(move.to(), sq("e4"));
    EXPECT_EQ(move.kind(), MoveKind::DoublePawnPush);
}

TEST(MoveTest, KindDefaultsToNormalAndHasNoPromotion) {
    const Move move(sq("g1"), sq("f3"));
    EXPECT_EQ(move.kind(), MoveKind::Normal);
    EXPECT_FALSE(move.promotion().has_value());
}

TEST(MoveTest, WithPromotionReturnsACopyWithTheChosenPiece) {
    const Move move(sq("e7"), sq("e8"), MoveKind::Promotion);
    const Move promoted = move.withPromotion(PieceTypeId("queen"));

    EXPECT_EQ(promoted.promotion(), PieceTypeId("queen"));
    EXPECT_FALSE(move.promotion().has_value());
    EXPECT_EQ(promoted.from(), move.from());
    EXPECT_EQ(promoted.to(), move.to());
    EXPECT_EQ(promoted.kind(), MoveKind::Promotion);
}

TEST(MoveTest, EqualityComparesEveryField) {
    const Move move(sq("e2"), sq("e4"), MoveKind::Normal);
    EXPECT_EQ(move, Move(sq("e2"), sq("e4"), MoveKind::Normal));
    EXPECT_NE(move, Move(sq("e2"), sq("e3"), MoveKind::Normal));
    EXPECT_NE(move, Move(sq("e2"), sq("e4"), MoveKind::Capture));
    EXPECT_NE(Move(sq("e7"), sq("e8"), MoveKind::Promotion).withPromotion(PieceTypeId("queen")),
              Move(sq("e7"), sq("e8"), MoveKind::Promotion).withPromotion(PieceTypeId("rook")));
}
