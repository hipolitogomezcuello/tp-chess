#include "application/MoveCommand.hpp"

#include "domain/Move.hpp"
#include "domain/RulesEngine.hpp"
#include "support/StandardFixture.hpp"

using chess::application::MoveCommand;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::Move;
using chess::domain::MoveKind;
using chess::domain::RulesEngine;
using chess::testing::sq;

namespace {

class MoveCommandTest : public chess::testing::StandardPiecesFixture {
protected:
    RulesEngine rules_{registry_};
};

}  // namespace

TEST_F(MoveCommandTest, ExecuteAppliesTheMove) {
    GameState state = initialState();
    MoveCommand command(rules_, Move(sq("e2"), sq("e4"), MoveKind::DoublePawnPush));

    command.execute(state);

    EXPECT_FALSE(state.board().isEmpty(sq("e4")));
    EXPECT_EQ(state.sideToMove(), Color::Black);
    EXPECT_EQ(state.enPassantTarget(), sq("e3"));
}

TEST_F(MoveCommandTest, UndoRestoresTheWholePreviousState) {
    // El Memento es el GameState completo: tablero, turno y casilla de al paso.
    GameState state(initialState().board(), Color::White, sq("d6"));
    MoveCommand command(rules_, Move(sq("g1"), sq("f3")));
    command.execute(state);

    command.undo(state);

    EXPECT_FALSE(state.board().isEmpty(sq("g1")));
    EXPECT_FALSE(state.board().pieceAt(sq("g1"))->hasMoved());
    EXPECT_TRUE(state.board().isEmpty(sq("f3")));
    EXPECT_EQ(state.sideToMove(), Color::White);
    EXPECT_EQ(state.enPassantTarget(), sq("d6"));
}

TEST_F(MoveCommandTest, KnowsItsMove) {
    const Move move(sq("g1"), sq("f3"));

    EXPECT_EQ(MoveCommand(rules_, move).move(), move);
}
