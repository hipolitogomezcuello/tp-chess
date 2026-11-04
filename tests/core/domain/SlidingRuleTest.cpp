#include "support/RuleTestFixture.hpp"
#include "domain/Directions.hpp"
#include "domain/SlidingRule.hpp"
#include "support/MoveAssertions.hpp"

using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::Directions;
using chess::domain::GameState;
using chess::domain::MoveKind;
using chess::domain::SlidingRule;
using chess::testing::destinations;
using chess::testing::findMoveTo;
using chess::testing::RuleTest;
using chess::testing::sorted;
using chess::testing::sq;
using chess::testing::squares;

using SlidingRuleTest = RuleTest;

TEST_F(SlidingRuleTest, OnAnEmptyBoardSlidesUntilTheEdges) {
    const SlidingRule rook(Directions::orthogonals());
    const GameState state(builder().place("d4", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(rook, state, "d4")),
              squares({"d1", "d2", "d3", "d5", "d6", "d7", "d8",
                       "a4", "b4", "c4", "e4", "f4", "g4", "h4"}));
}

TEST_F(SlidingRuleTest, DiagonalsFromACorner) {
    const SlidingRule bishop(Directions::diagonals());
    const GameState state(builder().place("a1", kPiece, Color::White).build());

    EXPECT_EQ(destinations(movesOf(bishop, state, "a1")),
              squares({"b2", "c3", "d4", "e5", "f6", "g7", "h8"}));
}

TEST_F(SlidingRuleTest, StopsBeforeAnOwnPiece) {
    const SlidingRule rook(Directions::orthogonals());
    const GameState state(builder()
                              .place("a1", kPiece, Color::White)
                              .place("a3", kPiece, Color::White)
                              .place("c1", kPiece, Color::White)
                              .build());

    EXPECT_EQ(destinations(movesOf(rook, state, "a1")), squares({"a2", "b1"}));
}

TEST_F(SlidingRuleTest, CapturesTheFirstEnemyPieceAndStopsThere) {
    const SlidingRule rook(Directions::orthogonals());
    const GameState state(builder()
                              .place("a1", kPiece, Color::White)
                              .place("a3", kPiece, Color::Black)
                              .place("a5", kPiece, Color::Black)
                              .place("b1", kPiece, Color::White)
                              .build());

    const auto moves = movesOf(rook, state, "a1");
    EXPECT_EQ(destinations(moves), squares({"a2", "a3"}));
    EXPECT_EQ(findMoveTo(moves, sq("a2"))->kind(), MoveKind::Normal);
    EXPECT_EQ(findMoveTo(moves, sq("a3"))->kind(), MoveKind::Capture);
}

TEST_F(SlidingRuleTest, EveryMoveStartsAtTheOrigin) {
    const SlidingRule queen(Directions::all());
    const GameState state(builder().place("d4", kPiece, Color::White).build());

    const auto moves = movesOf(queen, state, "d4");
    EXPECT_EQ(moves.size(), 27u);
    for (const auto& move : moves) {
        EXPECT_EQ(move.from(), sq("d4"));
    }
}

TEST_F(SlidingRuleTest, UsesTheRealBoardSizeNotEightByEight) {
    const SlidingRule rook(Directions::orthogonals());
    const GameState state(builder(BoardDimensions(10, 10)).place("a1", kPiece, Color::White).build());

    EXPECT_EQ(movesOf(rook, state, "a1").size(), 18u);
    EXPECT_NE(findMoveTo(movesOf(rook, state, "a1"), sq("j1")), nullptr);
}

TEST_F(SlidingRuleTest, AttacksUpToAndIncludingTheFirstPieceOfAnyColor) {
    // La casilla de una pieza propia cuenta como atacada (defendida): el rey rival no puede capturarla.
    const SlidingRule rook(Directions::orthogonals());
    const GameState state(builder()
                              .place("a1", kPiece, Color::White)
                              .place("a3", kPiece, Color::White)
                              .place("c1", kPiece, Color::Black)
                              .build());

    EXPECT_EQ(sorted(attacksOf(rook, state, "a1")), squares({"a2", "a3", "b1", "c1"}));
}
