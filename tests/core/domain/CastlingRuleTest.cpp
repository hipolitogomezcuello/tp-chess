#include "domain/CastlingRule.hpp"

#include "domain/MoveContext.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::BoardDimensions;
using chess::domain::CastlingRule;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::Move;
using chess::domain::MoveContext;
using chess::domain::MoveKind;
using chess::testing::destinations;
using chess::testing::findMoveTo;
using chess::testing::sq;
using chess::testing::squares;

namespace {

// Se usan las piezas clásicas porque la regla mira el trait canCastleWith de la torre.
// La regla solo mira la geometría: el jaque lo resuelve LegalMoveFilter (D-07).
class CastlingRuleTest : public chess::testing::StandardPiecesFixture {
protected:
    std::vector<Move> movesOf(const GameState& state, const std::string& from) const {
        return rule_.candidateMoves(MoveContext(state, sq(from)));
    }

    CastlingRule rule_;
};

}  // namespace

TEST_F(CastlingRuleTest, KingAndRooksThatNeverMovedCanCastleToBothSides) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("a1", kRook, Color::White)
                              .place("h1", kRook, Color::White)
                              .build());

    const auto moves = movesOf(state, "e1");

    EXPECT_EQ(destinations(moves), squares({"c1", "g1"}));
    const Move* kingside = findMoveTo(moves, sq("g1"));
    EXPECT_EQ(kingside->kind(), MoveKind::Castling);
    EXPECT_EQ(kingside->rookFrom(), sq("h1"));
    EXPECT_EQ(kingside->rookTo(), sq("f1"));
    const Move* queenside = findMoveTo(moves, sq("c1"));
    EXPECT_EQ(queenside->rookFrom(), sq("a1"));
    EXPECT_EQ(queenside->rookTo(), sq("d1"));
}

TEST_F(CastlingRuleTest, BlackCastlesOnItsOwnBackRank) {
    const GameState state(builder()
                              .place("e8", kKing, Color::Black)
                              .place("h8", kRook, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(destinations(movesOf(state, "e8")), squares({"g8"}));
}

TEST_F(CastlingRuleTest, AKingThatMovedCannotCastle) {
    const GameState state(builder()
                              .placeMoved("e1", kKing, Color::White)
                              .place("a1", kRook, Color::White)
                              .place("h1", kRook, Color::White)
                              .build());

    EXPECT_TRUE(movesOf(state, "e1").empty());
}

TEST_F(CastlingRuleTest, ARookThatMovedCannotCastle) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("a1", kRook, Color::White)
                              .placeMoved("h1", kRook, Color::White)
                              .build());

    EXPECT_EQ(destinations(movesOf(state, "e1")), squares({"c1"}));
}

TEST_F(CastlingRuleTest, AnyPieceInBetweenBlocksThatSide) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("a1", kRook, Color::White)
                              .place("b1", kKnight, Color::White)
                              .place("h1", kRook, Color::White)
                              .build());

    EXPECT_EQ(destinations(movesOf(state, "e1")), squares({"g1"}));
}

TEST_F(CastlingRuleTest, OnlyPiecesWithCanCastleWithAndOfTheSameColorCount) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("a1", kBishop, Color::White)
                              .place("h1", kRook, Color::Black)
                              .build());

    EXPECT_TRUE(movesOf(state, "e1").empty());
}

TEST_F(CastlingRuleTest, WorksOnLargerBoards) {
    // 10×10, rey en f1: va dos casillas hacia cada torre y la torre salta al otro lado (D-09).
    const GameState state(builder(BoardDimensions(10, 10))
                              .place("f1", kKing, Color::White)
                              .place("a1", kRook, Color::White)
                              .place("j1", kRook, Color::White)
                              .build());

    const auto moves = movesOf(state, "f1");

    EXPECT_EQ(destinations(moves), squares({"d1", "h1"}));
    EXPECT_EQ(findMoveTo(moves, sq("h1"))->rookTo(), sq("g1"));
    EXPECT_EQ(findMoveTo(moves, sq("d1"))->rookTo(), sq("e1"));
}

TEST_F(CastlingRuleTest, CastlingDoesNotAttackAnything) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("h1", kRook, Color::White)
                              .build());

    EXPECT_TRUE(rule_.attackedSquares(MoveContext(state, sq("e1"))).empty());
}
