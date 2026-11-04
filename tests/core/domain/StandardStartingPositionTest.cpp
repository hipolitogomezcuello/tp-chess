#include <gtest/gtest.h>

#include <string>

#include "domain/Board.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/StandardPieces.hpp"
#include "domain/StandardStartingPosition.hpp"
#include "domain/StartingPositionProvider.hpp"
#include "support/Squares.hpp"

using chess::domain::Board;
using chess::domain::BoardDimensions;
using chess::domain::Color;
using chess::domain::PieceFactory;
using chess::domain::PieceRegistry;
using chess::domain::PieceTypeId;
using chess::domain::Position;
using chess::domain::StandardPieces;
using chess::domain::StandardStartingPosition;
using chess::domain::StartingPositionProvider;
using chess::testing::sq;

namespace {

class StandardStartingPositionTest : public ::testing::Test {
protected:
    StandardStartingPositionTest() : factory_(registry_), board_(createBoard()) {}

    Board createBoard() {
        StandardPieces::registerAll(registry_);
        const StandardStartingPosition standard;
        const StartingPositionProvider& provider = standard;  // se usa a través de la interfaz
        return provider.createInitialBoard(factory_);
    }

    void expectPiece(const std::string& square, const PieceTypeId& type, Color color) const {
        const auto piece = board_.pieceAt(sq(square));
        ASSERT_TRUE(piece.has_value()) << square;
        EXPECT_EQ(piece->type(), type) << square;
        EXPECT_EQ(piece->color(), color) << square;
        EXPECT_FALSE(piece->hasMoved()) << square;
    }

    PieceRegistry registry_;
    PieceFactory factory_;
    Board board_;
};

}  // namespace

TEST_F(StandardStartingPositionTest, IsAnEightByEightBoard) {
    EXPECT_EQ(board_.dimensions(), BoardDimensions(8, 8));
}

TEST_F(StandardStartingPositionTest, HasSixteenPiecesPerSide) {
    EXPECT_EQ(board_.positionsOf(Color::White).size(), 16u);
    EXPECT_EQ(board_.positionsOf(Color::Black).size(), 16u);
}

TEST_F(StandardStartingPositionTest, BackRanksFollowRookKnightBishopQueenKing) {
    const std::string files = "abcdefgh";
    const PieceTypeId backRank[] = {StandardPieces::kRook,  StandardPieces::kKnight, StandardPieces::kBishop,
                                    StandardPieces::kQueen, StandardPieces::kKing,   StandardPieces::kBishop,
                                    StandardPieces::kKnight, StandardPieces::kRook};
    for (std::size_t i = 0; i < files.size(); ++i) {
        expectPiece(std::string(1, files[i]) + "1", backRank[i], Color::White);
        expectPiece(std::string(1, files[i]) + "8", backRank[i], Color::Black);
    }
}

TEST_F(StandardStartingPositionTest, PawnsFillTheSecondAndSeventhRanks) {
    for (const char file : std::string("abcdefgh")) {
        expectPiece(std::string(1, file) + "2", StandardPieces::kPawn, Color::White);
        expectPiece(std::string(1, file) + "7", StandardPieces::kPawn, Color::Black);
    }
}

TEST_F(StandardStartingPositionTest, QueenStartsOnHerOwnColor) {
    expectPiece("d1", StandardPieces::kQueen, Color::White);
    expectPiece("d8", StandardPieces::kQueen, Color::Black);
    expectPiece("e1", StandardPieces::kKing, Color::White);
    expectPiece("e8", StandardPieces::kKing, Color::Black);
}

TEST_F(StandardStartingPositionTest, MiddleRanksAreEmpty) {
    for (int file = 0; file < 8; ++file) {
        for (int rank = 2; rank <= 5; ++rank) {
            EXPECT_TRUE(board_.isEmpty(Position(file, rank)));
        }
    }
}
