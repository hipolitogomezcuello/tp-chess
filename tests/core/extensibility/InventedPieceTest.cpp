// Extensibilidad 1: una pieza nueva sin modificar ninguna clase (consigna §9, arquitectura §9).
// El Arzobispo (Alfil + Caballo) se registra desde el test componiendo reglas existentes. Todo lo
// demás (generación, jaque, mate, promoción) funciona porque el motor nunca pregunta por el tipo.
#include <algorithm>

#include "application/ChessGame.hpp"
#include "domain/GameStatus.hpp"
#include "domain/RulesEngine.hpp"
#include "ports/MoveResult.hpp"
#include "support/InventedPieces.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::application::ChessGame;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::RulesEngine;
using chess::testing::destinations;
using chess::testing::kArchbishop;
using chess::testing::sq;
using chess::testing::squares;

namespace {

class InventedPieceTest : public chess::testing::StandardPiecesFixture {
protected:
    InventedPieceTest() { chess::testing::registerInventedPieces(registry_); }

    RulesEngine rules_{registry_};
};

}  // namespace

TEST_F(InventedPieceTest, TheArchbishopMovesLikeABishopPlusAKnight) {
    const GameState state(builder()
                              .place("d4", kArchbishop, Color::White)
                              .place("h1", kKing, Color::White)
                              .place("a8", kKing, Color::Black)
                              .build());

    EXPECT_EQ(destinations(rules_.legalMoves(state, sq("d4"))),
              squares({// diagonales
                       "a1", "b2", "c3", "e5", "f6", "g7", "h8", "a7", "b6", "c5", "e3", "f2", "g1",
                       // saltos de caballo
                       "b3", "b5", "c2", "c6", "e2", "e6", "f3", "f5"}));
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
    // Desde f6 da jaque a h7 con el salto, y además cubre g8 (salto) y g7/h8 (diagonal).
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
    // El Arzobispo en f7 da jaque con su salto a h8 y cubre g8 en diagonal; el rey blanco cubre g7 y h7.
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

    // Dama, torre, alfil, caballo + Arzobispo y Canciller.
    EXPECT_EQ(choices.size(), 6u);
    const bool hasArchbishop = std::any_of(choices.begin(), choices.end(),
                                           [](const auto& piece) { return piece.type() == kArchbishop; });
    EXPECT_TRUE(hasArchbishop);
}
