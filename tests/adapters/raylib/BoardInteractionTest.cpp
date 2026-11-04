#include "adapters/raylib/BoardInteraction.hpp"

#include "application/ChessGame.hpp"
#include "domain/RulesEngine.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::adapters::raylib::BoardInteraction;
using chess::application::ChessGame;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::RulesEngine;
using chess::testing::destinations;
using chess::testing::sq;
using chess::testing::squares;

namespace {

// La interacción solo conoce el puerto GameService; acá se le da el ChessGame real.
class BoardInteractionTest : public chess::testing::StandardPiecesFixture {
protected:
    GameState aboutToPromote() const {
        return GameState(builder()
                             .place("a1", kKing, Color::White)
                             .place("h8", kKing, Color::Black)
                             .placeMoved("c7", kPawn, Color::White)
                             .build());
    }

    RulesEngine rules_{registry_};
    ChessGame game_{rules_, initialState()};
    BoardInteraction interaction_{game_};
};

}  // namespace

TEST_F(BoardInteractionTest, StartsWithNothingSelected) {
    EXPECT_FALSE(interaction_.selected().has_value());
    EXPECT_TRUE(interaction_.highlightedMoves().empty());
    EXPECT_FALSE(interaction_.awaitingPromotion());
}

TEST_F(BoardInteractionTest, ClickingAnOwnPieceSelectsItAndHighlightsItsMoves) {
    interaction_.click(sq("g1"));

    EXPECT_EQ(interaction_.selected(), sq("g1"));
    EXPECT_EQ(destinations(interaction_.highlightedMoves()), squares({"f3", "h3"}));
}

TEST_F(BoardInteractionTest, ClickingAnOpponentPieceOrAnEmptySquareSelectsNothing) {
    interaction_.click(sq("g8"));
    EXPECT_FALSE(interaction_.selected().has_value());

    interaction_.click(sq("e4"));
    EXPECT_FALSE(interaction_.selected().has_value());
}

TEST_F(BoardInteractionTest, ClickingAHighlightedSquareMakesTheMove) {
    interaction_.click(sq("e2"));
    interaction_.click(sq("e4"));

    EXPECT_FALSE(game_.state().board().isEmpty(sq("e4")));
    EXPECT_EQ(game_.state().sideToMove(), Color::Black);
    EXPECT_FALSE(interaction_.selected().has_value());
}

TEST_F(BoardInteractionTest, ClickingAnotherOwnPieceChangesTheSelection) {
    interaction_.click(sq("e2"));
    interaction_.click(sq("g1"));

    EXPECT_EQ(interaction_.selected(), sq("g1"));
    EXPECT_EQ(game_.state().sideToMove(), Color::White);
}

TEST_F(BoardInteractionTest, ClickingANonHighlightedSquareDeselects) {
    interaction_.click(sq("e2"));
    interaction_.click(sq("e5"));

    EXPECT_FALSE(interaction_.selected().has_value());
    EXPECT_TRUE(game_.state().board().isEmpty(sq("e5")));
}

TEST_F(BoardInteractionTest, APromotionWaitsForThePlayerToChooseThePiece) {
    ChessGame game(rules_, aboutToPromote());
    BoardInteraction interaction(game);

    interaction.click(sq("c7"));
    interaction.click(sq("c8"));

    EXPECT_TRUE(interaction.awaitingPromotion());
    EXPECT_EQ(interaction.promotionChoices().size(), 4u);
    EXPECT_EQ(game.state().board().pieceAt(sq("c7"))->type(), kPawn);  // todavía no se movió

    interaction.choosePromotion(kKnight);

    EXPECT_FALSE(interaction.awaitingPromotion());
    EXPECT_EQ(game.state().board().pieceAt(sq("c8"))->type(), kKnight);
}

TEST_F(BoardInteractionTest, CancelingAPendingPromotionLeavesThePawn) {
    ChessGame game(rules_, aboutToPromote());
    BoardInteraction interaction(game);
    interaction.click(sq("c7"));
    interaction.click(sq("c8"));

    interaction.cancel();

    EXPECT_FALSE(interaction.awaitingPromotion());
    EXPECT_FALSE(interaction.selected().has_value());
    EXPECT_EQ(game.state().board().pieceAt(sq("c7"))->type(), kPawn);
}

TEST_F(BoardInteractionTest, ClicksOnTheBoardAreIgnoredWhileChoosingAPromotion) {
    ChessGame game(rules_, aboutToPromote());
    BoardInteraction interaction(game);
    interaction.click(sq("c7"));
    interaction.click(sq("c8"));

    interaction.click(sq("a1"));

    EXPECT_TRUE(interaction.awaitingPromotion());
}

TEST_F(BoardInteractionTest, UndoAndRedoGoThroughTheGameAndClearTheSelection) {
    interaction_.click(sq("e2"));
    interaction_.click(sq("e4"));
    interaction_.click(sq("e7"));

    EXPECT_TRUE(interaction_.undo());
    EXPECT_TRUE(game_.state().board().isEmpty(sq("e4")));
    EXPECT_FALSE(interaction_.selected().has_value());

    EXPECT_TRUE(interaction_.redo());
    EXPECT_FALSE(game_.state().board().isEmpty(sq("e4")));
    EXPECT_FALSE(interaction_.redo());
}
