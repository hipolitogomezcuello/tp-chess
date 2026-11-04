#include "domain/RulesEngine.hpp"

#include "domain/GameStatus.hpp"
#include "support/MoveAssertions.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::Move;
using chess::domain::MoveKind;
using chess::domain::RulesEngine;
using chess::testing::destinations;
using chess::testing::findMoveTo;
using chess::testing::sq;
using chess::testing::squares;

namespace {

class RulesEngineTest : public chess::testing::StandardPiecesFixture {
protected:
    // Busca la jugada legal from→to (como hará ChessGame) y la aplica.
    GameState play(const GameState& state, const std::string& from, const std::string& to) const {
        const auto moves = engine_.legalMoves(state, sq(from));
        const Move* move = findMoveTo(moves, sq(to));
        EXPECT_NE(move, nullptr) << from << "-" << to << " debería ser legal";
        return move ? engine_.apply(state, *move) : state;
    }

    RulesEngine engine_{registry_};
};

}  // namespace

TEST_F(RulesEngineTest, LegalMovesOfAPawnInTheInitialPosition) {
    EXPECT_EQ(destinations(engine_.legalMoves(initialState(), sq("e2"))), squares({"e3", "e4"}));
}

TEST_F(RulesEngineTest, PiecesBlockedInTheInitialPositionHaveNoMoves) {
    EXPECT_TRUE(engine_.legalMoves(initialState(), sq("a1")).empty());
    EXPECT_TRUE(engine_.legalMoves(initialState(), sq("d1")).empty());
}

TEST_F(RulesEngineTest, EmptySquareHasNoLegalMoves) {
    EXPECT_TRUE(engine_.legalMoves(initialState(), sq("e4")).empty());
}

TEST_F(RulesEngineTest, LegalMovesExcludeThoseThatLeaveTheKingInCheck) {
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e2", kKnight, Color::White)
                              .place("e8", kRook, Color::Black)
                              .build());

    EXPECT_TRUE(engine_.legalMoves(state, sq("e2")).empty());
}

TEST_F(RulesEngineTest, ApplyReturnsTheNextStateWithoutTouchingTheCurrentOne) {
    const GameState initial = initialState();
    const GameState next = engine_.apply(initial, Move(sq("e2"), sq("e4"), MoveKind::DoublePawnPush));

    EXPECT_FALSE(next.board().isEmpty(sq("e4")));
    EXPECT_EQ(next.sideToMove(), Color::Black);
    EXPECT_TRUE(initial.board().isEmpty(sq("e4")));
}

TEST_F(RulesEngineTest, StatusIsInProgressAtTheStart) {
    EXPECT_EQ(engine_.status(initialState()), GameStatus::InProgress);
}

TEST_F(RulesEngineTest, QueenCheckAfterAFewMovesIsDetected) {
    // 1. e4 f6  2. Dh5+
    GameState state = initialState();
    state = play(state, "e2", "e4");
    state = play(state, "f7", "f6");
    state = play(state, "d1", "h5");

    EXPECT_EQ(engine_.status(state), GameStatus::Check);
    // La única defensa es interponer el peón: g7-g6.
    EXPECT_EQ(destinations(engine_.legalMoves(state, sq("g7"))), squares({"g6"}));
    EXPECT_TRUE(engine_.legalMoves(state, sq("e8")).empty());
}

TEST_F(RulesEngineTest, FoolsMateIsCheckmate) {
    // 1. f3 e5  2. g4 Dh4#
    GameState state = initialState();
    state = play(state, "f2", "f3");
    state = play(state, "e7", "e5");
    state = play(state, "g2", "g4");
    state = play(state, "d8", "h4");

    EXPECT_EQ(engine_.status(state), GameStatus::Checkmate);
}

// --- Promoción (D-08): la decide el trait isPromotable del registro ---

TEST_F(RulesEngineTest, CanPromoteOnlyToPromotablePieces) {
    EXPECT_TRUE(engine_.canPromoteTo(kQueen));
    EXPECT_TRUE(engine_.canPromoteTo(kRook));
    EXPECT_TRUE(engine_.canPromoteTo(kBishop));
    EXPECT_TRUE(engine_.canPromoteTo(kKnight));
    EXPECT_FALSE(engine_.canPromoteTo(kKing));
    EXPECT_FALSE(engine_.canPromoteTo(kPawn));
    EXPECT_FALSE(engine_.canPromoteTo(chess::domain::PieceTypeId("unicorn")));
}

TEST_F(RulesEngineTest, PromotionChoicesArePiecesOfTheGivenColor) {
    const auto choices = engine_.promotionChoices(Color::Black);

    ASSERT_EQ(choices.size(), 4u);
    for (const auto& piece : choices) {
        EXPECT_EQ(piece.color(), Color::Black);
        EXPECT_TRUE(piece.traits().isPromotable);
    }
}

TEST_F(RulesEngineTest, ApplyingAPromotionCreatesTheChosenPiece) {
    const GameState state(builder()
                              .place("a1", kKing, Color::White)
                              .place("h8", kKing, Color::Black)
                              .placeMoved("c7", kPawn, Color::White)
                              .build());
    const auto moves = engine_.legalMoves(state, sq("c7"));
    const Move* promotion = findMoveTo(moves, sq("c8"));
    ASSERT_NE(promotion, nullptr);

    const GameState next = engine_.apply(state, promotion->withPromotion(kRook));

    EXPECT_EQ(next.board().pieceAt(sq("c8"))->type(), kRook);
}
