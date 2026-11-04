#include "application/MoveHistory.hpp"

#include "application/MoveCommand.hpp"
#include "domain/Move.hpp"
#include "domain/RulesEngine.hpp"
#include "support/StandardFixture.hpp"

using chess::application::MoveCommand;
using chess::application::MoveHistory;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::Move;
using chess::domain::RulesEngine;
using chess::testing::sq;

namespace {

class MoveHistoryTest : public chess::testing::StandardPiecesFixture {
protected:
    // Ejecuta la jugada sobre `state` y la anota en el historial, como hace ChessGame.
    void play(GameState& state, const std::string& from, const std::string& to) {
        MoveCommand command(rules_, Move(sq(from), sq(to)));
        command.execute(state);
        history_.record(std::move(command));
    }

    RulesEngine rules_{registry_};
    MoveHistory history_;
};

}  // namespace

TEST_F(MoveHistoryTest, StartsEmpty) {
    GameState state = initialState();

    EXPECT_FALSE(history_.canUndo());
    EXPECT_FALSE(history_.canRedo());
    EXPECT_FALSE(history_.undo(state));
    EXPECT_FALSE(history_.redo(state));
}

TEST_F(MoveHistoryTest, UndoRestoresTheStateBeforeTheLastMove) {
    GameState state = initialState();
    play(state, "g1", "f3");

    EXPECT_TRUE(history_.undo(state));

    EXPECT_FALSE(state.board().isEmpty(sq("g1")));
    EXPECT_EQ(state.sideToMove(), Color::White);
    EXPECT_FALSE(history_.canUndo());
    EXPECT_TRUE(history_.canRedo());
}

TEST_F(MoveHistoryTest, RedoReappliesTheUndoneMove) {
    GameState state = initialState();
    play(state, "g1", "f3");
    history_.undo(state);

    EXPECT_TRUE(history_.redo(state));

    EXPECT_FALSE(state.board().isEmpty(sq("f3")));
    EXPECT_EQ(state.sideToMove(), Color::Black);
    EXPECT_TRUE(history_.canUndo());
    EXPECT_FALSE(history_.canRedo());
}

TEST_F(MoveHistoryTest, UndoesSeveralMovesInReverseOrder) {
    GameState state = initialState();
    play(state, "g1", "f3");
    play(state, "g8", "f6");

    history_.undo(state);
    EXPECT_FALSE(state.board().isEmpty(sq("g8")));
    EXPECT_FALSE(state.board().isEmpty(sq("f3")));

    history_.undo(state);
    EXPECT_FALSE(state.board().isEmpty(sq("g1")));
}

TEST_F(MoveHistoryTest, ANewMoveClearsTheRedoStack) {
    GameState state = initialState();
    play(state, "g1", "f3");
    history_.undo(state);

    play(state, "b1", "c3");

    EXPECT_FALSE(history_.canRedo());
    EXPECT_FALSE(history_.redo(state));
}
