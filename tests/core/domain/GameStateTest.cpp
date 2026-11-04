#include <gtest/gtest.h>

#include "domain/Board.hpp"
#include "domain/GameState.hpp"
#include "support/Squares.hpp"

using chess::domain::Board;
using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::GameState;
using chess::testing::sq;

TEST(GameStateTest, WhiteMovesFirstAndThereIsNoEnPassantByDefault) {
    const GameState state{Board(BoardDimensions())};
    EXPECT_EQ(state.sideToMove(), Color::White);
    EXPECT_FALSE(state.enPassantTarget().has_value());
    EXPECT_EQ(state.board().dimensions(), BoardDimensions());
}

TEST(GameStateTest, StoresSideToMoveAndEnPassantTarget) {
    const GameState state(Board(BoardDimensions(10, 10)), Color::Black, sq("e3"));
    EXPECT_EQ(state.sideToMove(), Color::Black);
    EXPECT_EQ(state.enPassantTarget(), sq("e3"));
    EXPECT_EQ(state.board().dimensions(), BoardDimensions(10, 10));
}
