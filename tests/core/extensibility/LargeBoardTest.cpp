// Extensibilidad 2: un tablero de 10×10 sin modificar ninguna clase (consigna §9, arquitectura §9).
// Lo único nuevo es TenByTenStartingPosition (otra Strategy de disposición inicial). Las reglas
// usan BoardDimensions para los bordes, la última fila y el enroque, y hasMoved para el doble
// avance (D-09), así que la partida entera funciona igual.
#include "application/ChessGame.hpp"
#include "domain/BoardDimensions.hpp"
#include "domain/GameStatus.hpp"
#include "domain/RulesEngine.hpp"
#include "ports/MoveResult.hpp"
#include "support/InventedPieces.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"
#include "support/TenByTenStartingPosition.hpp"

using chess::application::ChessGame;
using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::MoveKind;
using chess::domain::RulesEngine;
using chess::ports::MoveResult;
using chess::testing::destinations;
using chess::testing::kArchbishop;
using chess::testing::kChancellor;
using chess::testing::sq;
using chess::testing::squares;
using chess::testing::TenByTenStartingPosition;

namespace {

class LargeBoardTest : public chess::testing::StandardPiecesFixture {
protected:
    LargeBoardTest() { chess::testing::registerInventedPieces(registry_); }

    GameState tenByTen() const { return GameState(TenByTenStartingPosition().createInitialBoard(factory_)); }

    chess::testing::BoardBuilder builder10() const { return builder(BoardDimensions(10, 10)); }

    static MoveResult move(ChessGame& game, const std::string& from, const std::string& to) {
        return game.makeMove(sq(from), sq(to));
    }

    RulesEngine rules_{registry_};
};

}  // namespace

TEST_F(LargeBoardTest, TheInitialPositionHasFortyPiecesInTheirPlaces) {
    const GameState state = tenByTen();
    const auto& board = state.board();

    EXPECT_EQ(board.dimensions(), BoardDimensions(10, 10));
    EXPECT_EQ(board.positionsOf(Color::White).size(), 20u);
    EXPECT_EQ(board.positionsOf(Color::Black).size(), 20u);
    EXPECT_EQ(board.pieceAt(sq("f1"))->type(), kKing);
    EXPECT_EQ(board.pieceAt(sq("f10"))->type(), kKing);
    EXPECT_EQ(board.pieceAt(sq("c1"))->type(), kArchbishop);
    EXPECT_EQ(board.pieceAt(sq("h10"))->type(), kChancellor);
    EXPECT_EQ(board.pieceAt(sq("j9"))->type(), kPawn);
}

TEST_F(LargeBoardTest, TheGameStartsInProgressAndPawnsCanAdvanceTwo) {
    ChessGame game(rules_, tenByTen());

    EXPECT_EQ(game.status(), GameStatus::InProgress);
    EXPECT_EQ(destinations(game.legalMovesFrom(sq("j2"))), squares({"j3", "j4"}));

    const MoveResult result = move(game, "j2", "j4");
    ASSERT_TRUE(result.ok());
    EXPECT_EQ(result.move()->kind(), MoveKind::DoublePawnPush);
    EXPECT_TRUE(move(game, "a9", "a7").ok());
}

TEST_F(LargeBoardTest, InventedPiecesMoveFromTheStartingPosition) {
    ChessGame game(rules_, tenByTen());

    // El Canciller de h1 salta como caballo por encima de los peones.
    EXPECT_EQ(destinations(game.legalMovesFrom(sq("h1"))), squares({"g3", "i3"}));
    EXPECT_EQ(destinations(game.legalMovesFrom(sq("c1"))), squares({"b3", "d3"}));
}

TEST_F(LargeBoardTest, CastlingWorksWithTheKingOnTheFFile) {
    ChessGame game(rules_, GameState(builder10()
                                         .place("f1", kKing, Color::White)
                                         .place("a1", kRook, Color::White)
                                         .place("j1", kRook, Color::White)
                                         .place("f10", kKing, Color::Black)
                                         .build()));

    ASSERT_TRUE(move(game, "f1", "h1").ok());

    EXPECT_EQ(game.state().board().pieceAt(sq("h1"))->type(), kKing);
    EXPECT_EQ(game.state().board().pieceAt(sq("g1"))->type(), kRook);
}

TEST_F(LargeBoardTest, PawnsPromoteOnTheTenthRank) {
    ChessGame game(rules_, GameState(builder10()
                                         .place("a1", kKing, Color::White)
                                         .place("j10", kKing, Color::Black)
                                         .placeMoved("e8", kPawn, Color::White)
                                         .build()));

    EXPECT_TRUE(move(game, "e8", "e9").ok());  // la fila 9 todavía no es la última
    move(game, "j10", "i10");

    ASSERT_TRUE(game.makeMove(sq("e9"), sq("e10"), kChancellor).ok());
    EXPECT_EQ(game.state().board().pieceAt(sq("e10"))->type(), kChancellor);
}

TEST_F(LargeBoardTest, CheckmateIsDetectedOnTheLargerBoard) {
    // Mate de la escalera: la torre de a9 corta la fila 9 y la de b1 sube a dar mate en la 10.
    ChessGame game(rules_, GameState(builder10()
                                         .place("a1", kKing, Color::White)
                                         .place("a9", kRook, Color::White)
                                         .place("b1", kRook, Color::White)
                                         .place("j10", kKing, Color::Black)
                                         .build()));

    ASSERT_TRUE(move(game, "b1", "b10").ok());

    EXPECT_EQ(game.status(), GameStatus::Checkmate);
}
