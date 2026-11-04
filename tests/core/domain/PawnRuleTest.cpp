#include "support/RuleTestFixture.hpp"
#include "domain/PawnRule.hpp"
#include "support/MoveAssertions.hpp"

using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::MoveKind;
using chess::domain::PawnRule;
using chess::testing::destinations;
using chess::testing::findMoveTo;
using chess::testing::RuleTest;
using chess::testing::sorted;
using chess::testing::sq;
using chess::testing::squares;

using PawnRuleTest = RuleTest;

TEST_F(PawnRuleTest, UnmovedWhitePawnAdvancesOneOrTwoSquares) {
    const PawnRule pawn;
    const GameState state(builder().place("e2", kPiece, Color::White).build());

    const auto moves = movesOf(pawn, state, "e2");
    EXPECT_EQ(destinations(moves), squares({"e3", "e4"}));
    EXPECT_EQ(findMoveTo(moves, sq("e3"))->kind(), MoveKind::Normal);
    EXPECT_EQ(findMoveTo(moves, sq("e4"))->kind(), MoveKind::DoublePawnPush);
}

TEST_F(PawnRuleTest, BlackPawnsAdvanceTowardsRankOne) {
    const PawnRule pawn;
    const GameState state(builder().place("d7", kPiece, Color::Black).build(), Color::Black);

    EXPECT_EQ(destinations(movesOf(pawn, state, "d7")), squares({"d6", "d5"}));
}

TEST_F(PawnRuleTest, MovedPawnOnlyAdvancesOneSquare) {
    const PawnRule pawn;
    const GameState state(builder().placeMoved("e3", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(pawn, state, "e3")), squares({"e4"}));
}

TEST_F(PawnRuleTest, DoublePushDependsOnHasMovedNotOnTheRank) {
    // D-09: en un tablero de otro tamaño el peón puede arrancar en otra fila.
    const PawnRule pawn;
    const GameState state(builder(BoardDimensions(10, 10)).place("e3", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(pawn, state, "e3")), squares({"e4", "e5"}));
}

TEST_F(PawnRuleTest, APieceInFrontBlocksBothAdvances) {
    const PawnRule pawn;
    const GameState state(builder()
                              .place("e2", kPiece, Color::White)
                              .place("e3", kPiece, Color::Black)
                              .build());

    EXPECT_TRUE(movesOf(pawn, state, "e2").empty());
}

TEST_F(PawnRuleTest, APieceTwoSquaresAheadBlocksOnlyTheDoublePush) {
    const PawnRule pawn;
    const GameState state(builder()
                              .place("e2", kPiece, Color::White)
                              .place("e4", kPiece, Color::White)
                              .build());

    EXPECT_EQ(destinations(movesOf(pawn, state, "e2")), squares({"e3"}));
}

TEST_F(PawnRuleTest, CapturesEnemyPiecesDiagonallyForward) {
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e4", kPiece, Color::White)
                              .place("d5", kPiece, Color::Black)
                              .place("f5", kPiece, Color::Black)
                              .place("e5", kPiece, Color::Black)
                              .build());

    const auto moves = movesOf(pawn, state, "e4");
    EXPECT_EQ(destinations(moves), squares({"d5", "f5"}));
    EXPECT_EQ(findMoveTo(moves, sq("d5"))->kind(), MoveKind::Capture);
    EXPECT_EQ(findMoveTo(moves, sq("f5"))->kind(), MoveKind::Capture);
}

TEST_F(PawnRuleTest, DoesNotCaptureOwnPiecesNorMoveDiagonallyToEmptySquares) {
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e4", kPiece, Color::White)
                              .place("d5", kPiece, Color::White)
                              .build());

    EXPECT_EQ(destinations(movesOf(pawn, state, "e4")), squares({"e5"}));
}

TEST_F(PawnRuleTest, DoesNotCaptureBackwards) {
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e4", kPiece, Color::White)
                              .place("d3", kPiece, Color::Black)
                              .place("e5", kPiece, Color::Black)
                              .build());

    EXPECT_TRUE(movesOf(pawn, state, "e4").empty());
}

TEST_F(PawnRuleTest, AttacksBothForwardDiagonalsEvenIfEmpty) {
    const PawnRule pawn;
    const GameState state(builder().place("e2", kPiece, Color::White).build());

    EXPECT_EQ(sorted(attacksOf(pawn, state, "e2")), squares({"d3", "f3"}));
}

TEST_F(PawnRuleTest, BlackAttacksDiagonallyTowardsRankOne) {
    const PawnRule pawn;
    const GameState state(builder().place("e7", kPiece, Color::Black).build());

    EXPECT_EQ(sorted(attacksOf(pawn, state, "e7")), squares({"d6", "f6"}));
}

TEST_F(PawnRuleTest, PawnOnTheEdgeAttacksOnlyInsideTheBoard) {
    const PawnRule pawn;
    const GameState state(builder().place("a2", kPiece, Color::White).build());

    EXPECT_EQ(sorted(attacksOf(pawn, state, "a2")), squares({"b3"}));
}

TEST_F(PawnRuleTest, PawnOnTheLastRankHasNoMoves) {
    const PawnRule pawn;
    const GameState state(builder().placeMoved("e8", kPiece, Color::White).build());

    EXPECT_TRUE(movesOf(pawn, state, "e8").empty());
    EXPECT_TRUE(attacksOf(pawn, state, "e8").empty());
}

// --- Promoción: la regla la marca, no elige la pieza (D-08) ---

TEST_F(PawnRuleTest, AdvancingToTheLastRankIsAPromotion) {
    const PawnRule pawn;
    const GameState state(builder().placeMoved("e7", kPiece, Color::White).build());

    const auto moves = movesOf(pawn, state, "e7");
    ASSERT_EQ(destinations(moves), squares({"e8"}));
    EXPECT_EQ(moves[0].kind(), MoveKind::Promotion);
    EXPECT_FALSE(moves[0].promotion().has_value());
}

TEST_F(PawnRuleTest, CapturingOntoTheLastRankIsAPromotion) {
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e7", kPiece, Color::White)
                              .place("e8", kPiece, Color::Black)
                              .place("d8", kPiece, Color::Black)
                              .build());

    const auto moves = movesOf(pawn, state, "e7");
    ASSERT_EQ(destinations(moves), squares({"d8"}));
    EXPECT_EQ(moves[0].kind(), MoveKind::Promotion);
}

TEST_F(PawnRuleTest, BlackPromotesOnRankOne) {
    const PawnRule pawn;
    const GameState state(builder().placeMoved("c2", kPiece, Color::Black).build(), Color::Black);

    const auto moves = movesOf(pawn, state, "c2");
    ASSERT_EQ(destinations(moves), squares({"c1"}));
    EXPECT_EQ(moves[0].kind(), MoveKind::Promotion);
}

TEST_F(PawnRuleTest, PromotionHappensOnTheLastRankOfTheRealBoard) {
    // En un 10×10 la fila 8 es una más; la promoción es en la 10 (D-09).
    const PawnRule pawn;
    const GameState state(builder(BoardDimensions(10, 10))
                              .placeMoved("e7", kPiece, Color::White)
                              .placeMoved("g9", kPiece, Color::White)
                              .build());

    EXPECT_EQ(movesOf(pawn, state, "e7")[0].kind(), MoveKind::Normal);
    EXPECT_EQ(movesOf(pawn, state, "g9")[0].kind(), MoveKind::Promotion);
}

// --- Captura al paso: la casilla la deja MoveApplier en GameState::enPassantTarget ---

TEST_F(PawnRuleTest, CapturesEnPassantOnTheTargetSquare) {
    // Las negras acaban de jugar d7-d5: la casilla de al paso es d6.
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e5", kPiece, Color::White)
                              .placeMoved("d5", kPiece, Color::Black)
                              .build(),
                          Color::White, sq("d6"));

    const auto moves = movesOf(pawn, state, "e5");

    EXPECT_EQ(destinations(moves), squares({"d6", "e6"}));
    const auto* enPassant = findMoveTo(moves, sq("d6"));
    EXPECT_EQ(enPassant->kind(), MoveKind::EnPassant);
    EXPECT_EQ(enPassant->capturedSquare(), sq("d5"));
}

TEST_F(PawnRuleTest, BlackCapturesEnPassantTowardsRankOne) {
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("d4", kPiece, Color::Black)
                              .placeMoved("e4", kPiece, Color::White)
                              .build(),
                          Color::Black, sq("e3"));

    const auto moves = movesOf(pawn, state, "d4");
    const auto* enPassant = findMoveTo(moves, sq("e3"));

    ASSERT_NE(enPassant, nullptr);
    EXPECT_EQ(enPassant->capturedSquare(), sq("e4"));
}

TEST_F(PawnRuleTest, WithoutATargetSquareThereIsNoEnPassant) {
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e5", kPiece, Color::White)
                              .placeMoved("d5", kPiece, Color::Black)
                              .build());

    EXPECT_EQ(destinations(movesOf(pawn, state, "e5")), squares({"e6"}));
}

TEST_F(PawnRuleTest, EnPassantNeedsAnEnemyNextToThePawn) {
    // Con la casilla marcada pero sin víctima al lado (p. ej. un peón propio), no hay captura.
    const PawnRule pawn;
    const GameState state(builder()
                              .placeMoved("e5", kPiece, Color::White)
                              .placeMoved("d5", kPiece, Color::White)
                              .build(),
                          Color::White, sq("d6"));

    EXPECT_EQ(findMoveTo(movesOf(pawn, state, "e5"), sq("d6")), nullptr);
}
