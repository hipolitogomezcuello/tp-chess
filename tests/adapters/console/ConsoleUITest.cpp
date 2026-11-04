#include "adapters/console/ConsoleUI.hpp"

#include <sstream>
#include <string>

#include "application/ChessGame.hpp"
#include "domain/RulesEngine.hpp"
#include "support/StandardFixture.hpp"

using chess::adapters::console::ConsoleUI;
using chess::application::ChessGame;
using chess::domain::Color;
using chess::domain::RulesEngine;
using chess::testing::sq;

namespace {

// La UI solo conoce el puerto GameService; acá se le da el ChessGame real para probar de punta a punta.
class ConsoleUITest : public chess::testing::StandardPiecesFixture {
protected:
    void run(const std::string& input) {
        std::istringstream in(input);
        ConsoleUI(game_, in, out_).run();
    }

    bool printed(const std::string& text) const { return out_.str().find(text) != std::string::npos; }

    RulesEngine rules_;
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
