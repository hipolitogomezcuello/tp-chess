#include "adapters/console/ConsoleUI.hpp"

#include <sstream>
#include <string>

#include "application/ChessGame.hpp"
#include "domain/RulesEngine.hpp"
#include "support/InventedPieces.hpp"
#include "support/StandardFixture.hpp"

using chess::adapters::console::ConsoleUI;
using chess::application::ChessGame;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::RulesEngine;
using chess::testing::sq;

namespace {

// La UI solo conoce el puerto GameService; acá se le da el ChessGame real para probar de punta a punta.
class ConsoleUITest : public chess::testing::StandardPiecesFixture {
protected:
    void run(const std::string& input) { run(game_, input); }

    void run(chess::ports::GameService& game, const std::string& input) {
        std::istringstream in(input);
        ConsoleUI(game, in, out_).run();
    }

    // Peón blanco listo para promover en c8.
    GameState aboutToPromote() const {
        return GameState(builder()
                             .place("a1", kKing, Color::White)
                             .place("h8", kKing, Color::Black)
                             .placeMoved("c7", kPawn, Color::White)
                             .build());
    }

    bool printed(const std::string& text) const { return out_.str().find(text) != std::string::npos; }

    RulesEngine rules_{registry_};
    ChessGame game_{rules_, initialState()};
    std::ostringstream out_;
};

}  // namespace

TEST_F(ConsoleUITest, AMoveCommandMakesTheMove) {
    run("e2 e4\n");

    EXPECT_FALSE(game_.state().board().isEmpty(sq("e4")));
    EXPECT_EQ(game_.state().sideToMove(), Color::Black);
}

TEST_F(ConsoleUITest, SeveralMovesInARow) {
    run("e2 e4\ne7 e5\ng1 f3\n");

    EXPECT_FALSE(game_.state().board().isEmpty(sq("f3")));
    EXPECT_EQ(game_.state().sideToMove(), Color::Black);
}

TEST_F(ConsoleUITest, ToleratesExtraSpacesAndUppercase) {
    run("   E2    E4  \n");

    EXPECT_FALSE(game_.state().board().isEmpty(sq("e4")));
}

TEST_F(ConsoleUITest, QuitStopsReadingCommands) {
    run("quit\ne2 e4\n");

    EXPECT_TRUE(game_.state().board().isEmpty(sq("e4")));
}

TEST_F(ConsoleUITest, EndOfInputStopsTheLoop) {
    run("");

    EXPECT_EQ(game_.state().sideToMove(), Color::White);
}

TEST_F(ConsoleUITest, ExplainsAnIllegalMove) {
    run("e2 e5\n");

    EXPECT_TRUE(printed("Jugada ilegal: e2-e5"));
    EXPECT_FALSE(game_.state().board().isEmpty(sq("e2")));
}

TEST_F(ConsoleUITest, ExplainsAMoveFromAnEmptySquare) {
    run("e4 e5\n");

    EXPECT_TRUE(printed("No hay ninguna pieza en e4"));
}

TEST_F(ConsoleUITest, ExplainsAMoveOutOfTurn) {
    run("e7 e5\n");

    EXPECT_TRUE(printed("Esa pieza no es tuya: juegan las blancas"));
}

TEST_F(ConsoleUITest, RejectsSquaresThatDoNotExist) {
    run("e2 e9\n");

    EXPECT_TRUE(printed("Casilla inválida: e9"));
    EXPECT_EQ(game_.state().sideToMove(), Color::White);
}

TEST_F(ConsoleUITest, RejectsUnknownCommands) {
    run("hola\n");

    EXPECT_TRUE(printed("Comando desconocido"));
}

TEST_F(ConsoleUITest, MovesCommandListsTheDestinations) {
    run("moves b1\n");

    EXPECT_TRUE(printed("Jugadas desde b1: a3 c3"));
}

TEST_F(ConsoleUITest, MovesCommandWithoutMovesSaysSo) {
    run("moves a1\n");

    EXPECT_TRUE(printed("No hay jugadas desde a1"));
}

TEST_F(ConsoleUITest, HelpListsTheCommands) {
    run("help\n");

    EXPECT_TRUE(printed("moves"));
    EXPECT_TRUE(printed("quit"));
}

TEST_F(ConsoleUITest, EmptyLinesAreIgnored) {
    run("\n\ne2 e4\n");

    EXPECT_FALSE(printed("Comando desconocido"));
    EXPECT_FALSE(game_.state().board().isEmpty(sq("e4")));
}

TEST_F(ConsoleUITest, ExplainsThatTheGameIsOver) {
    run("f2 f3\ne7 e5\ng2 g4\nd8 h4\na2 a3\n");

    EXPECT_TRUE(printed("La partida terminó"));
    EXPECT_TRUE(game_.state().board().isEmpty(sq("a3")));
}

// --- Deshacer y rehacer ---

TEST_F(ConsoleUITest, UndoTakesBackTheLastMove) {
    run("e2 e4\nundo\n");

    EXPECT_TRUE(game_.state().board().isEmpty(sq("e4")));
    EXPECT_TRUE(printed("Se deshizo la última jugada"));
}

TEST_F(ConsoleUITest, RedoReplaysTheUndoneMove) {
    run("e2 e4\nundo\nredo\n");

    EXPECT_FALSE(game_.state().board().isEmpty(sq("e4")));
    EXPECT_TRUE(printed("Se rehízo la jugada"));
}

TEST_F(ConsoleUITest, UndoAndRedoWithNothingToDoSaySo) {
    run("undo\nredo\n");

    EXPECT_TRUE(printed("No hay jugadas para deshacer"));
    EXPECT_TRUE(printed("No hay jugadas para rehacer"));
}

// --- Promoción: "c7 c8 q" (la letra es el símbolo de la pieza, o su id completo) ---

TEST_F(ConsoleUITest, AThirdWordChoosesThePromotionPiece) {
    ChessGame game(rules_, aboutToPromote());

    run(game, "c7 c8 q\n");

    EXPECT_EQ(game.state().board().pieceAt(sq("c8"))->type(), kQueen);
}

TEST_F(ConsoleUITest, ThePromotionPieceCanBeWrittenWithItsFullId) {
    ChessGame game(rules_, aboutToPromote());

    run(game, "c7 c8 knight\n");

    EXPECT_EQ(game.state().board().pieceAt(sq("c8"))->type(), kKnight);
}

TEST_F(ConsoleUITest, AsksForThePieceWhenAPromotionHasNone) {
    ChessGame game(rules_, aboutToPromote());

    run(game, "c7 c8\n");

    EXPECT_TRUE(printed("Elegí a qué pieza promover"));
    EXPECT_TRUE(printed("c7 c8 q"));
    EXPECT_EQ(game.state().board().pieceAt(sq("c7"))->type(), kPawn);
}

TEST_F(ConsoleUITest, RejectsAnUnknownPromotionPiece) {
    ChessGame game(rules_, aboutToPromote());

    run(game, "c7 c8 x\n");

    EXPECT_TRUE(printed("No se puede promover a x"));
    EXPECT_EQ(game.state().board().pieceAt(sq("c7"))->type(), kPawn);
}

TEST_F(ConsoleUITest, RejectsANonPromotablePiece) {
    ChessGame game(rules_, aboutToPromote());

    run(game, "c7 c8 k\n");

    EXPECT_TRUE(printed("No se puede promover a k"));
}

TEST_F(ConsoleUITest, RejectsAPromotionPieceOnANormalMove) {
    run("e2 e4 q\n");

    EXPECT_TRUE(printed("Esa jugada no es una promoción"));
    EXPECT_TRUE(game_.state().board().isEmpty(sq("e4")));
}

TEST_F(ConsoleUITest, AnInventedPromotablePieceIsChosenByItsSymbolWithoutTouchingTheConsole) {
    // Extensibilidad (D-19): la consola busca entre promotionChoices(), no tiene un switch de tipos.
    chess::testing::registerInventedPieces(registry_);
    ChessGame game(rules_, aboutToPromote());

    run(game, "c7 c8 a\n");

    EXPECT_EQ(game.state().board().pieceAt(sq("c8"))->type(), chess::testing::kArchbishop);
}
