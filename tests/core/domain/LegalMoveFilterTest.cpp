#include "domain/LegalMoveFilter.hpp"

#include "domain/AttackDetector.hpp"
#include "domain/MoveApplier.hpp"
#include "domain/MoveGenerator.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::AttackDetector;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::LegalMoveFilter;
using chess::domain::Move;
using chess::domain::MoveApplier;
using chess::domain::MoveGenerator;
using chess::testing::destinations;
using chess::testing::sq;
using chess::testing::squares;

namespace {

class LegalMoveFilterTest : public chess::testing::StandardPiecesFixture {
protected:
    LegalMoveFilterTest() : filter_(applier_, detector_) {}

    std::vector<Move> legalMovesFrom(const GameState& state, const std::string& from) const {
        return filter_.filter(state, generator_.movesFrom(state, sq(from)));
    }

    MoveGenerator generator_;
    MoveApplier applier_{factory_};
    AttackDetector detector_;
    LegalMoveFilter filter_;
};

}

TEST_F(LegalMoveFilterTest, KeepsEveryMoveWhenNothingIsAtStake) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("g1", kKnight, Color::White)
                              .place("e8", kKing, Color::Black)
                              .build());

    EXPECT_EQ(destinations(legalMovesFrom(state, "g1")), squares({"e2", "f3", "h3"}));
}

TEST_F(LegalMoveFilterTest, PinnedPieceCannotLeaveThePinLine) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e2", kKnight, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_TRUE(legalMovesFrom(state, "e2").empty());
}

TEST_F(LegalMoveFilterTest, PinnedSliderCanStillMoveAlongThePinLine) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e2", kRook, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_EQ(destinations(legalMovesFrom(state, "e2")), squares({"e3", "e4", "e5", "e6", "e7", "e8"}));
}

TEST_F(LegalMoveFilterTest, KingCannotStepIntoCheck) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("d8", kRook, Color::Black)
                              .build());

    EXPECT_EQ(destinations(legalMovesFrom(state, "e1")), squares({"e2", "f1", "f2"}));
}

TEST_F(LegalMoveFilterTest, KingCannotCaptureADefendedPiece) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e2", kQueen, Color::Black)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_TRUE(legalMovesFrom(state, "e1").empty());
}

TEST_F(LegalMoveFilterTest, KingCanCaptureAnUndefendedAttacker) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e2", kQueen, Color::Black)
                              .build());

    const auto moves = legalMovesFrom(state, "e1");
    EXPECT_EQ(destinations(moves), squares({"e2"}));
}

TEST_F(LegalMoveFilterTest, WhenInCheckOnlyMovesThatResolveItRemain) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("a2", kRook, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_EQ(destinations(legalMovesFrom(state, "a2")), squares({"e2"}));
}

TEST_F(LegalMoveFilterTest, CapturingTheCheckingPieceResolvesTheCheck) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("h8", kRook, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_EQ(destinations(legalMovesFrom(state, "h8")), squares({"e8"}));
}

TEST_F(LegalMoveFilterTest, UsesTheColorOfTheMovingPieceNotTheSideToMove) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e2", kKnight, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_TRUE(legalMovesFrom(state, "e2").empty());
}

namespace {

chess::testing::BoardBuilder readyToCastle(chess::testing::BoardBuilder builder) {
    builder.place("e1", chess::domain::StandardPieces::kKing, Color::White)
        .place("a1", chess::domain::StandardPieces::kRook, Color::White)
        .place("h1", chess::domain::StandardPieces::kRook, Color::White)
        .place("a8", chess::domain::StandardPieces::kKing, Color::Black);
    return builder;
}

}

TEST_F(LegalMoveFilterTest, BothCastlingsAreLegalWhenNothingIsAttacked) {
    const GameState state(readyToCastle(builder()).build());

    const auto moves = legalMovesFrom(state, "e1");

    EXPECT_NE(chess::testing::findMoveTo(moves, sq("g1")), nullptr);
    EXPECT_NE(chess::testing::findMoveTo(moves, sq("c1")), nullptr);
}

TEST_F(LegalMoveFilterTest, CannotCastleOutOfCheck) {
    const GameState state(readyToCastle(builder()).place("e8", kRook, Color::Black).build());

    const auto moves = legalMovesFrom(state, "e1");

    EXPECT_EQ(chess::testing::findMoveTo(moves, sq("g1")), nullptr);
    EXPECT_EQ(chess::testing::findMoveTo(moves, sq("c1")), nullptr);
}

TEST_F(LegalMoveFilterTest, CannotCastleThroughAnAttackedSquare) {
    const GameState state(readyToCastle(builder()).place("f8", kRook, Color::Black).build());

    const auto moves = legalMovesFrom(state, "e1");

    EXPECT_EQ(chess::testing::findMoveTo(moves, sq("g1")), nullptr);
    EXPECT_NE(chess::testing::findMoveTo(moves, sq("c1")), nullptr);
}

TEST_F(LegalMoveFilterTest, CannotCastleIntoCheck) {
    const GameState state(readyToCastle(builder()).place("g8", kRook, Color::Black).build());

    EXPECT_EQ(chess::testing::findMoveTo(legalMovesFrom(state, "e1"), sq("g1")), nullptr);
}

TEST_F(LegalMoveFilterTest, AnAttackedSquareTheKingDoesNotCrossDoesNotMatter) {
    const GameState state(readyToCastle(builder()).place("b8", kRook, Color::Black).build());

    EXPECT_NE(chess::testing::findMoveTo(legalMovesFrom(state, "e1"), sq("c1")), nullptr);
}

TEST_F(LegalMoveFilterTest, EnPassantThatExposesTheKingOnTheRankIsIllegal) {
    const GameState state(builder()
                              .place("a5", kKing, Color::White)
                              .placeMoved("b5", kPawn, Color::White)
                              .placeMoved("c5", kPawn, Color::Black)
                              .place("h5", kRook, Color::Black)
                              .place("e8", kKing, Color::Black)
                              .build(),
                          Color::White, sq("c6"));

    EXPECT_EQ(destinations(legalMovesFrom(state, "b5")), squares({"b6"}));
}
