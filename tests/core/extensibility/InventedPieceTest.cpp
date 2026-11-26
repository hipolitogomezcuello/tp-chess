#include <algorithm>
#include <vector>

#include "application/ChessGame.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Position.hpp"
#include "domain/RulesEngine.hpp"
#include "ports/MoveResult.hpp"
#include "support/InventedPieces.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::application::ChessGame;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::Position;
using chess::domain::RulesEngine;
using chess::testing::destinations;
using chess::testing::kArchbishop;
using chess::testing::sorted;
using chess::testing::sq;
using chess::testing::squares;

namespace {

class InventedPieceTest : public chess::testing::StandardPiecesFixture {
protected:
    InventedPieceTest() { chess::testing::registerInventedPieces(registry_); }

    RulesEngine rules_{registry_};
};

}

TEST_F(InventedPieceTest, TheArchbishopMovesLikeABishopPlusAKnight) {
    const GameState state(builder()
                              .place("d4", kArchbishop, Color::White)
                              .place("h1", kKing, Color::White)
                              .place("a8", kKing, Color::Black)
                              .build());

    const auto diagonalTargets = squares({"a1", "b2", "c3", "e5", "f6", "g7", "h8", "a7", "b6", "c5", "e3", "f2", "g1"});
    const auto knightJumpTargets = squares({"b3", "b5", "c2", "c6", "e2", "e6", "f3", "f5"});
    std::vector<Position> expected = diagonalTargets;
    expected.insert(expected.end(), knightJumpTargets.begin(), knightJumpTargets.end());

    EXPECT_EQ(destinations(rules_.legalMoves(state, sq("d4"))), sorted(expected));
}

TEST_F(InventedPieceTest, TheArchbishopGivesCheckWithItsKnightJump) {
    const GameState state(builder()
                              .place("d6", kArchbishop, Color::White)
                              .place("a1", kKing, Color::White)
                              .place("e8", kKing, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(rules_.status(state), GameStatus::Check);
}

TEST_F(InventedPieceTest, TheEnemyKingCannotStepOntoSquaresTheArchbishopAttacks) {
    const GameState state(builder()
                              .place("f6", kArchbishop, Color::White)
                              .place("a1", kKing, Color::White)
                              .place("h7", kKing, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(rules_.status(state), GameStatus::Check);
    EXPECT_EQ(destinations(rules_.legalMoves(state, sq("h7"))), squares({"g6", "h6"}));
}

TEST_F(InventedPieceTest, ItCanDeliverCheckmate) {
    const GameState state(builder()
                              .place("f7", kArchbishop, Color::White)
                              .place("g6", kKing, Color::White)
                              .place("h8", kKing, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(rules_.status(state), GameStatus::Checkmate);
}

TEST_F(InventedPieceTest, APawnCanPromoteToTheArchbishopBecauseOfItsTrait) {
    ChessGame game(rules_, GameState(builder()
                                         .place("a1", kKing, Color::White)
                                         .place("h8", kKing, Color::Black)
                                         .placeMoved("c7", kPawn, Color::White)
                                         .build()));

    ASSERT_TRUE(game.makeMove(sq("c7"), sq("c8"), kArchbishop).ok());

    EXPECT_EQ(game.state().board().pieceAt(sq("c8"))->type(), kArchbishop);
    EXPECT_EQ(game.state().board().pieceAt(sq("c8"))->symbol(), 'A');
}

TEST_F(InventedPieceTest, PromotionChoicesIncludeTheInventedPieces) {
    ChessGame game(rules_, initialState());

    const auto choices = game.promotionChoices();

    EXPECT_EQ(choices.size(), 6u);
    const bool hasArchbishop = std::any_of(choices.begin(), choices.end(),
                                           [](const auto& piece) { return piece.type() == kArchbishop; });
    EXPECT_TRUE(hasArchbishop);
}
