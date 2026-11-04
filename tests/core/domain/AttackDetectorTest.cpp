#include "domain/AttackDetector.hpp"

#include "domain/PieceTraits.hpp"
#include "domain/PieceTypeId.hpp"
#include "support/PieceTestSupport.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::AttackDetector;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::PieceTraits;
using chess::domain::PieceTypeId;
using chess::testing::makeBareDefinition;
using chess::testing::sq;

using AttackDetectorTest = chess::testing::StandardPiecesFixture;

TEST_F(AttackDetectorTest, SquareInARooksLineIsAttacked) {
    const AttackDetector detector;
    const GameState state(builder().place("a1", kRook, Color::Black).build());

    EXPECT_TRUE(detector.isSquareAttacked(state, sq("a7"), Color::Black));
    EXPECT_TRUE(detector.isSquareAttacked(state, sq("h1"), Color::Black));
    EXPECT_FALSE(detector.isSquareAttacked(state, sq("b2"), Color::Black));
}

TEST_F(AttackDetectorTest, OnlyCountsPiecesOfTheGivenColor) {
    const AttackDetector detector;
    const GameState state(builder().place("a1", kRook, Color::Black).build());

    EXPECT_FALSE(detector.isSquareAttacked(state, sq("a7"), Color::White));
}

TEST_F(AttackDetectorTest, BlockedLinesDoNotAttack) {
    const AttackDetector detector;
    const GameState state(builder()
                              .place("a1", kRook, Color::Black)
                              .place("a4", kPawn, Color::White)
                              .build());

    EXPECT_TRUE(detector.isSquareAttacked(state, sq("a4"), Color::Black));
    EXPECT_FALSE(detector.isSquareAttacked(state, sq("a5"), Color::Black));
}

TEST_F(AttackDetectorTest, PawnsAttackDiagonallyNotForward) {
    const AttackDetector detector;
    const GameState state(builder().place("e4", kPawn, Color::White).build());

    EXPECT_TRUE(detector.isSquareAttacked(state, sq("d5"), Color::White));
    EXPECT_TRUE(detector.isSquareAttacked(state, sq("f5"), Color::White));
    EXPECT_FALSE(detector.isSquareAttacked(state, sq("e5"), Color::White));
}

TEST_F(AttackDetectorTest, KingAttackedByAnEnemyPieceIsInCheck) {
    const AttackDetector detector;
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_TRUE(detector.isInCheck(state, Color::White));
}

TEST_F(AttackDetectorTest, KingNotAttackedIsNotInCheck) {
    const AttackDetector detector;
    EXPECT_FALSE(detector.isInCheck(initialState(), Color::White));
    EXPECT_FALSE(detector.isInCheck(initialState(), Color::Black));
}

TEST_F(AttackDetectorTest, KnightChecksJumpingOverPieces) {
    const AttackDetector detector;
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("d1", kQueen, Color::White)
                              .place("d2", kPawn, Color::White)
                              .place("e2", kPawn, Color::White)
                              .place("f3", kKnight, Color::Black)
                              .build());

    EXPECT_TRUE(detector.isInCheck(state, Color::White));
}

TEST_F(AttackDetectorTest, CheckLooksForTheRoyalTraitNotForTheKingType) {
    // Una pieza inventada con isRoyal también puede quedar en jaque; un rey sin el trait, no.
    PieceTraits royal;
    royal.isRoyal = true;
    registry_.add(makeBareDefinition("emperor", 'E', royal));
    const GameState state(builder()
                              .place("e1", PieceTypeId("emperor"), Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_TRUE(AttackDetector().isInCheck(state, Color::White));
}

TEST_F(AttackDetectorTest, WithoutARoyalPieceThereIsNoCheck) {
    const GameState state(builder()
                              .place("e1", kRook, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_FALSE(AttackDetector().isInCheck(state, Color::White));
}
