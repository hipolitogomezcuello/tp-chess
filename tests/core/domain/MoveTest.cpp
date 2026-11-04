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

TEST(MoveTest, CastlingCarriesTheRookMove) {
    const Move move = Move::castling(sq("e1"), sq("g1"), sq("h1"), sq("f1"));

    EXPECT_EQ(move.kind(), MoveKind::Castling);
    EXPECT_EQ(move.from(), sq("e1"));
    EXPECT_EQ(move.to(), sq("g1"));
    EXPECT_EQ(move.rookFrom(), sq("h1"));
    EXPECT_EQ(move.rookTo(), sq("f1"));
}

TEST(MoveTest, OrdinaryMovesHaveNoRookMove) {
    const Move move(sq("e1"), sq("f1"));

    EXPECT_FALSE(move.rookFrom().has_value());
    EXPECT_FALSE(move.rookTo().has_value());
}

TEST(MoveTest, CastlingMovesWithDifferentRooksAreDifferent) {
    EXPECT_NE(Move::castling(sq("e1"), sq("g1"), sq("h1"), sq("f1")),
              Move::castling(sq("e1"), sq("g1"), sq("a1"), sq("f1")));
}

TEST(MoveTest, EnPassantCarriesTheCapturedSquare) {
    const Move move = Move::enPassant(sq("e5"), sq("d6"), sq("d5"));

    EXPECT_EQ(move.kind(), MoveKind::EnPassant);
    EXPECT_EQ(move.to(), sq("d6"));
    EXPECT_EQ(move.capturedSquare(), sq("d5"));
    EXPECT_FALSE(Move(sq("e5"), sq("d6"), MoveKind::Capture).capturedSquare().has_value());
}
