#include "support/RuleTestFixture.hpp"
#include "domain/Directions.hpp"
#include "domain/LeaperRule.hpp"
#include "support/MoveAssertions.hpp"

using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::Directions;
using chess::domain::GameState;
using chess::domain::LeaperRule;
using chess::domain::MoveKind;
using chess::testing::destinations;
using chess::testing::findMoveTo;
using chess::testing::RuleTest;
using chess::testing::sorted;
using chess::testing::sq;
using chess::testing::squares;

using LeaperRuleTest = RuleTest;

TEST_F(LeaperRuleTest, KnightInTheCenterHasEightJumps) {
    const LeaperRule knight(Directions::knightJumps());
    const GameState state(builder().place("d4", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(knight, state, "d4")),
              squares({"b3", "b5", "c2", "c6", "e2", "e6", "f3", "f5"}));
}

TEST_F(LeaperRuleTest, JumpsOutsideTheBoardAreDiscarded) {
    const LeaperRule knight(Directions::knightJumps());
    const GameState state(builder().place("a1", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(knight, state, "a1")), squares({"b3", "c2"}));
}

TEST_F(LeaperRuleTest, JumpsOverPiecesInBetween) {
    const LeaperRule knight(Directions::knightJumps());
    const GameState state(builder()
                              .place("b1", kPiece, Color::White)
                              .place("b2", kPiece, Color::White)
                              .place("c2", kPiece, Color::White)
                              .place("a2", kPiece, Color::White)
                              .build());

    EXPECT_EQ(destinations(movesOf(knight, state, "b1")), squares({"a3", "c3", "d2"}));
}

TEST_F(LeaperRuleTest, CannotLandOnAnOwnPieceButCapturesAnEnemyOne) {
    const LeaperRule knight(Directions::knightJumps());
    const GameState state(builder()
                              .place("a1", kPiece, Color::White)
                              .place("b3", kPiece, Color::White)
                              .place("c2", kPiece, Color::Black)
                              .build());

    const auto moves = movesOf(knight, state, "a1");
    EXPECT_EQ(destinations(moves), squares({"c2"}));
    EXPECT_EQ(findMoveTo(moves, sq("c2"))->kind(), MoveKind::Capture);
}

TEST_F(LeaperRuleTest, KingStepsAreLeapsToTheEightNeighbours) {
    const LeaperRule king(Directions::all());
    const GameState state(builder().place("e1", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(king, state, "e1")), squares({"d1", "d2", "e2", "f2", "f1"}));
}

TEST_F(LeaperRuleTest, UsesTheRealBoardSizeNotEightByEight) {
    const LeaperRule knight(Directions::knightJumps());
    const GameState state(builder(BoardDimensions(10, 10)).place("i9", kPiece, Color::White).build());

    // En 8×8 desde i9 no habría nada; en 10×10 hay g10, g8, h7, j7.
    EXPECT_EQ(destinations(movesOf(knight, state, "i9")), squares({"g10", "g8", "h7", "j7"}));
}

TEST_F(LeaperRuleTest, AttacksEveryTargetInsideTheBoardEvenIfOccupiedByAnOwnPiece) {
    const LeaperRule knight(Directions::knightJumps());
    const GameState state(builder()
                              .place("a1", kPiece, Color::White)
                              .place("b3", kPiece, Color::White)
                              .build());

    EXPECT_EQ(sorted(attacksOf(knight, state, "a1")), squares({"b3", "c2"}));
}
