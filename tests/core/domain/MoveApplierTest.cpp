#include "domain/MoveApplier.hpp"

#include "domain/Move.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::Move;
using chess::domain::MoveApplier;
using chess::domain::MoveKind;
using chess::testing::sq;

using MoveApplierTest = chess::testing::StandardPiecesFixture;

TEST_F(MoveApplierTest, MovesThePieceAndMarksItAsMoved) {
    const MoveApplier applier(factory_);
    const GameState state(builder().place("g1", kKnight, Color::White).build());

    const GameState next = applier.apply(state, Move(sq("g1"), sq("f3")));

    EXPECT_TRUE(next.board().isEmpty(sq("g1")));
    ASSERT_TRUE(next.board().pieceAt(sq("f3")).has_value());
    EXPECT_EQ(next.board().pieceAt(sq("f3"))->type(), kKnight);
    EXPECT_TRUE(next.board().pieceAt(sq("f3"))->hasMoved());
}

TEST_F(MoveApplierTest, PassesTheTurnToTheOtherSide) {
    const MoveApplier applier(factory_);
    const GameState white(builder().place("e2", kPawn, Color::White).build(), Color::White);
    const GameState black = applier.apply(white, Move(sq("e2"), sq("e4"), MoveKind::DoublePawnPush));

    EXPECT_EQ(black.sideToMove(), Color::Black);
}

TEST_F(MoveApplierTest, CaptureRemovesTheVictim) {
    const MoveApplier applier(factory_);
    const GameState state(builder()
                              .place("a1", kRook, Color::White)
                              .place("a8", kRook, Color::Black)
                              .build());

    const GameState next = applier.apply(state, Move(sq("a1"), sq("a8"), MoveKind::Capture));

    EXPECT_EQ(next.board().pieceAt(sq("a8"))->color(), Color::White);
    EXPECT_TRUE(next.board().positionsOf(Color::Black).empty());
}

TEST_F(MoveApplierTest, DoesNotModifyTheOriginalState) {
    const MoveApplier applier(factory_);
    const GameState state(builder().place("g1", kKnight, Color::White).build());

    (void)applier.apply(state, Move(sq("g1"), sq("f3")));

    EXPECT_FALSE(state.board().isEmpty(sq("g1")));
    EXPECT_TRUE(state.board().isEmpty(sq("f3")));
    EXPECT_EQ(state.sideToMove(), Color::White);
}

TEST_F(MoveApplierTest, PromotionReplacesThePawnWithTheChosenPiece) {
    const MoveApplier applier(factory_);
    const GameState state(builder().placeMoved("e7", kPawn, Color::White).build());

    const GameState next = applier.apply(state, Move(sq("e7"), sq("e8"), MoveKind::Promotion).withPromotion(kQueen));

    ASSERT_TRUE(next.board().pieceAt(sq("e8")).has_value());
    EXPECT_EQ(next.board().pieceAt(sq("e8"))->type(), kQueen);
    EXPECT_EQ(next.board().pieceAt(sq("e8"))->color(), Color::White);
    EXPECT_TRUE(next.board().pieceAt(sq("e8"))->hasMoved());
    EXPECT_TRUE(next.board().isEmpty(sq("e7")));
}

TEST_F(MoveApplierTest, PromotionCanAlsoCapture) {
    const MoveApplier applier(factory_);
    const GameState state(builder()
                              .placeMoved("b2", kPawn, Color::Black)
                              .place("a1", kRook, Color::White)
                              .build(),
                          Color::Black);

    const GameState next = applier.apply(state, Move(sq("b2"), sq("a1"), MoveKind::Promotion).withPromotion(kKnight));

    EXPECT_EQ(next.board().pieceAt(sq("a1"))->type(), kKnight);
    EXPECT_EQ(next.board().pieceAt(sq("a1"))->color(), Color::Black);
    EXPECT_TRUE(next.board().positionsOf(Color::White).empty());
}

TEST_F(MoveApplierTest, PromotionWithoutAChosenPieceLeavesThePawn) {
    // LegalMoveFilter prueba la jugada antes de que el jugador elija: el peón llega igual a la casilla.
    const MoveApplier applier(factory_);
    const GameState state(builder().placeMoved("e7", kPawn, Color::White).build());

    const GameState next = applier.apply(state, Move(sq("e7"), sq("e8"), MoveKind::Promotion));

    EXPECT_EQ(next.board().pieceAt(sq("e8"))->type(), kPawn);
}

TEST_F(MoveApplierTest, CastlingMovesTheKingAndTheRook) {
    const MoveApplier applier(factory_);
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("h1", kRook, Color::White)
                              .build());

    const GameState next = applier.apply(state, Move::castling(sq("e1"), sq("g1"), sq("h1"), sq("f1")));

    EXPECT_EQ(next.board().pieceAt(sq("g1"))->type(), kKing);
    EXPECT_EQ(next.board().pieceAt(sq("f1"))->type(), kRook);
    EXPECT_TRUE(next.board().pieceAt(sq("f1"))->hasMoved());
    EXPECT_TRUE(next.board().isEmpty(sq("e1")));
    EXPECT_TRUE(next.board().isEmpty(sq("h1")));
}
