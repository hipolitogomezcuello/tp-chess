#include "adapters/console/ConsoleRenderer.hpp"

#include <sstream>
#include <string>

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "support/StandardFixture.hpp"

using chess::adapters::console::ConsoleRenderer;
using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::Move;
using chess::domain::MoveKind;
using chess::testing::sq;

namespace {

class ConsoleRendererTest : public chess::testing::StandardPiecesFixture {
protected:
    bool printed(const std::string& text) const { return out_.str().find(text) != std::string::npos; }

    std::ostringstream out_;
    ConsoleRenderer renderer_{out_};
};

}  // namespace

TEST_F(ConsoleRendererTest, DrawsTheInitialPositionWithWhiteInUppercase) {
    renderer_.render(initialState());

    EXPECT_TRUE(printed("8 r n b q k b n r\n"));
    EXPECT_TRUE(printed("7 p p p p p p p p\n"));
    EXPECT_TRUE(printed("4 . . . . . . . .\n"));
    EXPECT_TRUE(printed("2 P P P P P P P P\n"));
    EXPECT_TRUE(printed("1 R N B Q K B N R\n"));
    EXPECT_TRUE(printed("  a b c d e f g h\n"));
}

TEST_F(ConsoleRendererTest, BlackRanksAreDrawnAboveWhiteRanks) {
    renderer_.render(initialState());

    EXPECT_LT(out_.str().find("8 r n b"), out_.str().find("1 R N B"));
}

TEST_F(ConsoleRendererTest, SaysWhoMoves) {
    renderer_.render(initialState());
    EXPECT_TRUE(printed("Juegan las blancas"));

    out_.str("");
    renderer_.render(GameState(initialState().board(), Color::Black));
    EXPECT_TRUE(printed("Juegan las negras"));
}

TEST_F(ConsoleRendererTest, DrawsBoardsOfAnySize) {
    const GameState state(builder(BoardDimensions(10, 10)).place("j10", kKing, Color::Black).build());

    renderer_.render(state);

    EXPECT_TRUE(printed("10 . . . . . . . . . k\n"));
    EXPECT_TRUE(printed(" 1 . . . . . . . . . .\n"));
    EXPECT_TRUE(printed("   a b c d e f g h i j\n"));
}

TEST_F(ConsoleRendererTest, OnMoveMadePrintsTheMoveAndTheNewBoard) {
    const GameState after(builder().place("e4", kPawn, Color::White).build(), Color::Black);

    renderer_.onMoveMade(Move(sq("e2"), sq("e4"), MoveKind::DoublePawnPush), after);

    EXPECT_TRUE(printed("e2-e4"));
    EXPECT_TRUE(printed("4 . . . . P . . .\n"));
    EXPECT_TRUE(printed("Juegan las negras"));
}

TEST_F(ConsoleRendererTest, CapturesArePrintedWithAnX) {
    const GameState after(builder().place("d5", kPawn, Color::White).build(), Color::Black);

    renderer_.onMoveMade(Move(sq("e4"), sq("d5"), MoveKind::Capture), after);

    EXPECT_TRUE(printed("e4xd5"));
}

TEST_F(ConsoleRendererTest, OnCheckAnnouncesTheColorInCheck) {
    renderer_.onCheck(Color::Black);
    EXPECT_TRUE(printed("Jaque a las negras"));

    renderer_.onCheck(Color::White);
    EXPECT_TRUE(printed("Jaque a las blancas"));
}

TEST_F(ConsoleRendererTest, OnGameOverAnnouncesTheWinnerOfACheckmate) {
    renderer_.onGameOver(GameStatus::Checkmate, Color::White);

    EXPECT_TRUE(printed("¡Jaque mate! Ganan las negras."));
}

TEST_F(ConsoleRendererTest, OnGameOverAnnouncesAStalemateAsADraw) {
    renderer_.onGameOver(GameStatus::Stalemate, Color::Black);

    EXPECT_TRUE(printed("Ahogado: la partida termina en tablas."));
}
