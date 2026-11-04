#include "application/ChessGame.hpp"

#include <string>

#include "domain/GameStatus.hpp"
#include "domain/RulesEngine.hpp"
#include "ports/MoveError.hpp"
#include "support/FakeObserver.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::application::ChessGame;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::MoveKind;
using chess::domain::Position;
using chess::domain::RulesEngine;
using chess::ports::MoveError;
using chess::ports::MoveResult;
using chess::testing::destinations;
using chess::testing::FakeObserver;
using chess::testing::sq;
using chess::testing::squares;

namespace {

class ChessGameTest : public chess::testing::StandardPiecesFixture {
protected:
    static MoveResult move(ChessGame& game, const std::string& from, const std::string& to) {
        return game.makeMove(sq(from), sq(to));
    }

    RulesEngine rules_;
    FakeObserver observer_;
};

}  // namespace

// --- Estado inicial ---

TEST_F(ChessGameTest, StartsWithTheGivenStateAndWhiteToMove) {
    ChessGame game(rules_, initialState());

    EXPECT_EQ(game.state().sideToMove(), Color::White);
    EXPECT_FALSE(game.state().board().isEmpty(sq("e2")));
    EXPECT_EQ(game.status(), GameStatus::InProgress);
}

// --- Jugada válida ---

TEST_F(ChessGameTest, ValidMoveSucceedsAndUpdatesTheState) {
    ChessGame game(rules_, initialState());

    const MoveResult result = move(game, "e2", "e4");

    ASSERT_TRUE(result.ok());
    EXPECT_EQ(result.move()->kind(), MoveKind::DoublePawnPush);
    EXPECT_TRUE(game.state().board().isEmpty(sq("e2")));
    EXPECT_FALSE(game.state().board().isEmpty(sq("e4")));
}

TEST_F(ChessGameTest, TurnsAlternateAfterEachMove) {
    ChessGame game(rules_, initialState());

    move(game, "e2", "e4");
    EXPECT_EQ(game.state().sideToMove(), Color::Black);
    move(game, "e7", "e5");
    EXPECT_EQ(game.state().sideToMove(), Color::White);
}

TEST_F(ChessGameTest, ValidMoveNotifiesTheObserverWithTheNewState) {
    ChessGame game(rules_, initialState());
    game.addObserver(observer_);

    const MoveResult result = move(game, "g1", "f3");

    ASSERT_EQ(observer_.movesMade.size(), 1u);
    EXPECT_EQ(observer_.movesMade[0], *result.move());
    EXPECT_EQ(observer_.statesAfterMove[0].sideToMove(), Color::Black);
    EXPECT_FALSE(observer_.statesAfterMove[0].board().isEmpty(sq("f3")));
}

TEST_F(ChessGameTest, EveryObserverIsNotified) {
    ChessGame game(rules_, initialState());
    FakeObserver another;
    game.addObserver(observer_);
    game.addObserver(another);

    move(game, "e2", "e4");

    EXPECT_EQ(observer_.movesMade.size(), 1u);
    EXPECT_EQ(another.movesMade.size(), 1u);
}

// --- Captura ---

TEST_F(ChessGameTest, CaptureRemovesTheEnemyPiece) {
    ChessGame game(rules_, GameState(builder()
                                         .place("e1", kKing, Color::White)
                                         .place("d1", kRook, Color::White)
                                         .place("e8", kKing, Color::Black)
                                         .place("d7", kKnight, Color::Black)
                                         .build()));

    const MoveResult result = move(game, "d1", "d7");

    ASSERT_TRUE(result.ok());
    EXPECT_EQ(result.move()->kind(), MoveKind::Capture);
    EXPECT_EQ(game.state().board().pieceAt(sq("d7"))->type(), kRook);
    EXPECT_EQ(game.state().board().positionsOf(Color::Black).size(), 1u);
}

// --- Jugadas inválidas: no cambian nada ni notifican ---

TEST_F(ChessGameTest, MoveFromAnEmptySquareFailsWithNoPieceAtOrigin) {
    ChessGame game(rules_, initialState());
    game.addObserver(observer_);

    EXPECT_EQ(move(game, "e4", "e5").error(), MoveError::NoPieceAtOrigin);
    EXPECT_TRUE(observer_.movesMade.empty());
}

TEST_F(ChessGameTest, MoveFromOutsideTheBoardFailsWithNoPieceAtOrigin) {
    ChessGame game(rules_, initialState());

    EXPECT_EQ(game.makeMove(Position(8, 0), sq("e4")).error(), MoveError::NoPieceAtOrigin);
}

TEST_F(ChessGameTest, MovingAnOpponentPieceFailsWithNotYourTurn) {
    ChessGame game(rules_, initialState());
    game.addObserver(observer_);

    EXPECT_EQ(move(game, "e7", "e5").error(), MoveError::NotYourTurn);
    EXPECT_EQ(game.state().sideToMove(), Color::White);
    EXPECT_TRUE(observer_.movesMade.empty());
}

TEST_F(ChessGameTest, UnreachableDestinationFailsWithIllegalMove) {
    ChessGame game(rules_, initialState());
    game.addObserver(observer_);

    EXPECT_EQ(move(game, "e2", "e5").error(), MoveError::IllegalMove);
    EXPECT_FALSE(game.state().board().isEmpty(sq("e2")));
    EXPECT_TRUE(observer_.movesMade.empty());
}

TEST_F(ChessGameTest, DestinationOutsideTheBoardFailsWithIllegalMove) {
    ChessGame game(rules_, initialState());

    EXPECT_EQ(game.makeMove(sq("e2"), Position(4, 8)).error(), MoveError::IllegalMove);
}

TEST_F(ChessGameTest, MovingAPinnedPieceFailsWithIllegalMove) {
    ChessGame game(rules_, GameState(builder()
                                         .place("e1", kKing, Color::White)
                                         .place("e2", kBishop, Color::White)
                                         .place("e8", kRook, Color::Black)
                                         .place("a8", kKing, Color::Black)
                                         .build()));

    EXPECT_EQ(move(game, "e2", "d3").error(), MoveError::IllegalMove);
}

// --- Jaque ---

TEST_F(ChessGameTest, MoveThatGivesCheckNotifiesTheColorInCheck) {
    ChessGame game(rules_, initialState());
    game.addObserver(observer_);

    // 1. e4 f6  2. Dh5+
    move(game, "e2", "e4");
    move(game, "f7", "f6");
    EXPECT_TRUE(observer_.checks.empty());
    move(game, "d1", "h5");

    ASSERT_EQ(observer_.checks.size(), 1u);
    EXPECT_EQ(observer_.checks[0], Color::Black);
    EXPECT_EQ(game.status(), GameStatus::Check);
}

TEST_F(ChessGameTest, InCheckOnlyMovesThatResolveItAreAccepted) {
    ChessGame game(rules_, initialState());
    move(game, "e2", "e4");
    move(game, "f7", "f6");
    move(game, "d1", "h5");

    EXPECT_EQ(move(game, "a7", "a6").error(), MoveError::IllegalMove);
    EXPECT_TRUE(move(game, "g7", "g6").ok());
    EXPECT_EQ(game.status(), GameStatus::InProgress);
}

// --- Consultas para la UI ---

TEST_F(ChessGameTest, LegalMovesFromAPieceOfTheSideToMove) {
    ChessGame game(rules_, initialState());

    EXPECT_EQ(destinations(game.legalMovesFrom(sq("b1"))), squares({"a3", "c3"}));
}

TEST_F(ChessGameTest, LegalMovesFromAnOpponentPieceAreEmpty) {
    ChessGame game(rules_, initialState());

    EXPECT_TRUE(game.legalMovesFrom(sq("b8")).empty());
}

TEST_F(ChessGameTest, LegalMovesFromAnEmptyOrOutsideSquareAreEmpty) {
    ChessGame game(rules_, initialState());

    EXPECT_TRUE(game.legalMovesFrom(sq("e4")).empty());
    EXPECT_TRUE(game.legalMovesFrom(Position(-1, 0)).empty());
}
