#include <gtest/gtest.h>

#include <algorithm>

#include "domain/GameState.hpp"
#include "domain/MoveContext.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/StandardPieces.hpp"
#include "support/BoardBuilder.hpp"
#include "support/MoveAssertions.hpp"

using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::Move;
using chess::domain::MoveContext;
using chess::domain::PieceFactory;
using chess::domain::PieceRegistry;
using chess::domain::PieceTypeId;
using chess::domain::StandardPieces;
using chess::testing::BoardBuilder;
using chess::testing::destinations;
using chess::testing::sq;
using chess::testing::squares;

namespace {

class StandardPiecesTest : public ::testing::Test {
protected:
    StandardPiecesTest() : factory_(registry_) { StandardPieces::registerAll(registry_); }

    // Junta las jugadas de todas las reglas de la pieza (lo que hará MoveGenerator).
    std::vector<Move> movesOf(const PieceTypeId& type, const std::string& from) const {
        const GameState state(BoardBuilder(factory_).place(from, type, Color::White).build());
        const MoveContext context(state, sq(from));
        std::vector<Move> result;
        for (const auto& rule : context.piece().definition().rules()) {
            const auto moves = rule->candidateMoves(context);
            result.insert(result.end(), moves.begin(), moves.end());
        }
        return result;
    }

    PieceRegistry registry_;
    PieceFactory factory_;
};

}  // namespace

TEST_F(StandardPiecesTest, RegistersTheSixClassicPieces) {
    for (const auto& id : {StandardPieces::kPawn, StandardPieces::kRook, StandardPieces::kKnight,
                           StandardPieces::kBishop, StandardPieces::kQueen, StandardPieces::kKing}) {
        EXPECT_TRUE(registry_.contains(id)) << id.value();
    }
}

TEST_F(StandardPiecesTest, UsesTheUsualSymbols) {
    EXPECT_EQ(registry_.find(StandardPieces::kPawn)->symbol(), 'P');
    EXPECT_EQ(registry_.find(StandardPieces::kRook)->symbol(), 'R');
    EXPECT_EQ(registry_.find(StandardPieces::kKnight)->symbol(), 'N');
    EXPECT_EQ(registry_.find(StandardPieces::kBishop)->symbol(), 'B');
    EXPECT_EQ(registry_.find(StandardPieces::kQueen)->symbol(), 'Q');
    EXPECT_EQ(registry_.find(StandardPieces::kKing)->symbol(), 'K');
}

TEST_F(StandardPiecesTest, OnlyTheKingIsRoyal) {
    EXPECT_TRUE(registry_.find(StandardPieces::kKing)->traits().isRoyal);
    EXPECT_FALSE(registry_.find(StandardPieces::kQueen)->traits().isRoyal);
    EXPECT_FALSE(registry_.find(StandardPieces::kPawn)->traits().isRoyal);
}

TEST_F(StandardPiecesTest, TheKingCastlesWithTheRook) {
    EXPECT_TRUE(registry_.find(StandardPieces::kRook)->traits().canCastleWith);
    EXPECT_FALSE(registry_.find(StandardPieces::kBishop)->traits().canCastleWith);
}

TEST_F(StandardPiecesTest, PawnsPromoteToQueenRookBishopOrKnight) {
    auto promotable = registry_.promotableTypes();
    std::sort(promotable.begin(), promotable.end());
    std::vector<PieceTypeId> expected{StandardPieces::kQueen, StandardPieces::kRook, StandardPieces::kBishop,
                                      StandardPieces::kKnight};
    std::sort(expected.begin(), expected.end());
    EXPECT_EQ(promotable, expected);
}

TEST_F(StandardPiecesTest, RookMovesAlongRanksAndFiles) {
    EXPECT_EQ(movesOf(StandardPieces::kRook, "d4").size(), 14u);
}

TEST_F(StandardPiecesTest, BishopMovesAlongDiagonals) {
    EXPECT_EQ(movesOf(StandardPieces::kBishop, "d4").size(), 13u);
}

TEST_F(StandardPiecesTest, QueenCombinesRookAndBishop) {
    EXPECT_EQ(movesOf(StandardPieces::kQueen, "d4").size(), 27u);
}

TEST_F(StandardPiecesTest, KnightJumpsInL) {
    EXPECT_EQ(destinations(movesOf(StandardPieces::kKnight, "g1")), squares({"e2", "f3", "h3"}));
}

TEST_F(StandardPiecesTest, KingStepsToItsNeighbours) {
    EXPECT_EQ(destinations(movesOf(StandardPieces::kKing, "e1")), squares({"d1", "d2", "e2", "f2", "f1"}));
}

TEST_F(StandardPiecesTest, PawnAdvancesForward) {
    EXPECT_EQ(destinations(movesOf(StandardPieces::kPawn, "e2")), squares({"e3", "e4"}));
}
