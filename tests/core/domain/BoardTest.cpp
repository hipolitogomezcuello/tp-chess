#include <gtest/gtest.h>

#include <algorithm>

#include "domain/Board.hpp"
#include "domain/PieceRegistry.hpp"
#include "support/BoardBuilder.hpp"
#include "support/PieceTestSupport.hpp"
#include "support/Squares.hpp"

using chess::domain::Board;
using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::PieceFactory;
using chess::domain::PieceRegistry;
using chess::domain::PieceTypeId;
using chess::domain::Position;
using chess::testing::BoardBuilder;
using chess::testing::makeBareDefinition;
using chess::testing::sq;

namespace {

class BoardTest : public ::testing::Test {
protected:
    BoardTest() : factory_(registry_) {
        registry_.add(makeBareDefinition("rook", 'R'));
        registry_.add(makeBareDefinition("knight", 'N'));
    }

    const PieceTypeId kRook{"rook"};
    const PieceTypeId kKnight{"knight"};
    PieceRegistry registry_;
    PieceFactory factory_;
};

}  // namespace

TEST_F(BoardTest, StartsEmptyWithTheGivenDimensions) {
    const Board board(BoardDimensions(10, 10));
    EXPECT_EQ(board.dimensions(), BoardDimensions(10, 10));
    EXPECT_TRUE(board.isEmpty(sq("a1")));
    EXPECT_TRUE(board.isEmpty(sq("j10")));
    EXPECT_FALSE(board.pieceAt(sq("e4")).has_value());
}

TEST_F(BoardTest, PlacedPieceCanBeReadBack) {
    Board board{BoardDimensions()};
    board.place(sq("d4"), factory_.create(kRook, Color::White));

    ASSERT_TRUE(board.pieceAt(sq("d4")).has_value());
    EXPECT_EQ(board.pieceAt(sq("d4"))->type(), kRook);
    EXPECT_FALSE(board.isEmpty(sq("d4")));
}

TEST_F(BoardTest, PlacingOnAnOccupiedSquareReplacesThePiece) {
    Board board{BoardDimensions()};
    board.place(sq("d4"), factory_.create(kRook, Color::White));
    board.place(sq("d4"), factory_.create(kKnight, Color::Black));

    EXPECT_EQ(board.pieceAt(sq("d4"))->type(), kKnight);
}

TEST_F(BoardTest, RemoveEmptiesTheSquare) {
    Board board = BoardBuilder(factory_).place("d4", kRook, Color::White).build();
    board.remove(sq("d4"));
    EXPECT_TRUE(board.isEmpty(sq("d4")));
}

TEST_F(BoardTest, MoveRelocatesThePieceAndCapturesWhateverWasThere) {
    Board board = BoardBuilder(factory_)
                      .place("a1", kRook, Color::White)
                      .place("a8", kKnight, Color::Black)
                      .build();
    board.move(sq("a1"), sq("a8"));

    EXPECT_TRUE(board.isEmpty(sq("a1")));
    EXPECT_EQ(board.pieceAt(sq("a8"))->type(), kRook);
    EXPECT_EQ(board.pieceAt(sq("a8"))->color(), Color::White);
}

TEST_F(BoardTest, MoveDoesNotApplyGameRules) {
    // Board no sabe de reglas: no marca hasMoved (eso lo hace MoveApplier).
    Board board = BoardBuilder(factory_).place("a1", kRook, Color::White).build();
    board.move(sq("a1"), sq("a2"));
    EXPECT_FALSE(board.pieceAt(sq("a2"))->hasMoved());
}

TEST_F(BoardTest, MovingFromAnEmptySquareIsAProgrammingError) {
    Board board{BoardDimensions()};
    EXPECT_THROW(board.move(sq("a1"), sq("a2")), std::invalid_argument);
}

TEST_F(BoardTest, PositionsOutsideTheBoardAreRejected) {
    Board board{BoardDimensions()};
    EXPECT_THROW(board.pieceAt(Position(8, 0)), std::out_of_range);
    EXPECT_THROW(board.place(Position(-1, 0), factory_.create(kRook, Color::White)), std::out_of_range);
}

TEST_F(BoardTest, PositionsOfListsOnlyThatColor) {
    const Board board = BoardBuilder(factory_)
                            .place("a1", kRook, Color::White)
                            .place("h1", kRook, Color::White)
                            .place("b8", kKnight, Color::Black)
                            .build();

    auto white = board.positionsOf(Color::White);
    std::sort(white.begin(), white.end());
    ASSERT_EQ(white.size(), 2u);
    EXPECT_EQ(white[0], sq("a1"));
    EXPECT_EQ(white[1], sq("h1"));
    EXPECT_EQ(board.positionsOf(Color::Black).size(), 1u);
}

TEST_F(BoardTest, CopiesAreIndependent) {
    const Board original = BoardBuilder(factory_).place("a1", kRook, Color::White).build();
    Board copy = original;
    copy.move(sq("a1"), sq("a5"));

    EXPECT_FALSE(original.isEmpty(sq("a1")));
    EXPECT_TRUE(original.isEmpty(sq("a5")));
}

TEST_F(BoardTest, MovingAPieceOntoItsOwnSquareKeepsIt) {
    Board board = BoardBuilder(factory_).place("a1", kRook, Color::White).build();
    board.move(sq("a1"), sq("a1"));
    EXPECT_FALSE(board.isEmpty(sq("a1")));
}
