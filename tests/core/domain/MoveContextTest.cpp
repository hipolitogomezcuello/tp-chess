#include "support/RuleTestFixture.hpp"

using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::MoveContext;
using chess::testing::RuleTest;
using chess::testing::sq;

using MoveContextTest = RuleTest;

TEST_F(MoveContextTest, ExposesStateBoardOriginAndThePieceThere) {
    const GameState state(builder().place("d4", kPiece, Color::Black).build(), Color::Black);
    const MoveContext context(state, sq("d4"));

    EXPECT_EQ(&context.state(), &state);
    EXPECT_EQ(&context.board(), &state.board());
    EXPECT_EQ(context.from(), sq("d4"));
    EXPECT_EQ(context.piece().color(), Color::Black);
    EXPECT_EQ(context.piece().type(), kPiece);
}

TEST_F(MoveContextTest, RequiresAPieceAtTheOrigin) {
    const GameState state(builder().build());
    EXPECT_THROW(MoveContext(state, sq("d4")), std::invalid_argument);
}
