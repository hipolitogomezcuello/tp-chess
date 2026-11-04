#include "domain/GameStatusEvaluator.hpp"

#include "domain/AttackDetector.hpp"
#include "domain/GameStatus.hpp"
#include "domain/LegalMoveFilter.hpp"
#include "domain/MoveApplier.hpp"
#include "domain/MoveGenerator.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::AttackDetector;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::GameStatusEvaluator;
using chess::domain::LegalMoveFilter;
using chess::domain::MoveApplier;
using chess::domain::MoveGenerator;

namespace {

class GameStatusEvaluatorTest : public chess::testing::StandardPiecesFixture {
protected:
    GameStatusEvaluatorTest() : filter_(applier_, detector_), evaluator_(detector_, generator_, filter_) {}

    MoveGenerator generator_;
    MoveApplier applier_;
    AttackDetector detector_;
    LegalMoveFilter filter_;
    GameStatusEvaluator evaluator_;
};

}  // namespace

TEST_F(GameStatusEvaluatorTest, InitialPositionIsInProgress) {
    EXPECT_EQ(evaluator_.evaluate(initialState()), GameStatus::InProgress);
}

TEST_F(GameStatusEvaluatorTest, SideToMoveInCheckIsCheck) {
    const GameState state(builder()
                              .place("a1", kKing, Color::White)
                              .place("e8", kKing, Color::Black)
                              .place("e2", kRook, Color::White)
                              .build(),
                          Color::Black);

    EXPECT_EQ(evaluator_.evaluate(state), GameStatus::Check);
}

TEST_F(GameStatusEvaluatorTest, OnlyTheSideToMoveIsEvaluated) {
    // El rey negro está atacado, pero le toca a las blancas: para ellas la partida sigue normal.
    const GameState state(builder()
                              .place("e1", kKing, Color::White)
                              .place("e8", kKing, Color::Black)
                              .place("a8", kRook, Color::White)
                              .build(),
                          Color::White);

    EXPECT_EQ(evaluator_.evaluate(state), GameStatus::InProgress);
}

TEST_F(GameStatusEvaluatorTest, CheckWithAnEscapeIsOnlyCheck) {
    // La torre da jaque, pero el rey negro puede capturarla (no está defendida).
    const GameState state(builder()
                              .place("a1", kKing, Color::White)
                              .place("e8", kKing, Color::Black)
                              .place("e7", kRook, Color::White)
                              .build(),
                          Color::Black);

    EXPECT_EQ(evaluator_.evaluate(state), GameStatus::Check);
}

TEST_F(GameStatusEvaluatorTest, CheckWithoutLegalMovesIsCheckmate) {
    // Mate del pasillo: la torre en a8 da jaque y los peones propios le cierran la salida al rey.
    const GameState state(builder()
                              .place("a1", kKing, Color::White)
                              .place("a8", kRook, Color::White)
                              .place("h8", kKing, Color::Black)
                              .place("g7", kPawn, Color::Black)
                              .place("h7", kPawn, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(evaluator_.evaluate(state), GameStatus::Checkmate);
}

TEST_F(GameStatusEvaluatorTest, NoCheckAndNoLegalMovesIsStalemate) {
    // Ahogado clásico: el rey negro en a8 no está en jaque, pero la dama cubre a7, b7 y b8.
    const GameState state(builder()
                              .place("h1", kKing, Color::White)
                              .place("b6", kQueen, Color::White)
                              .place("a8", kKing, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(evaluator_.evaluate(state), GameStatus::Stalemate);
}

TEST_F(GameStatusEvaluatorTest, ABlockedKingIsNotStalemateIfAnotherPieceCanMove) {
    // Misma posición, pero las negras tienen un peón que puede avanzar.
    const GameState state(builder()
                              .place("h1", kKing, Color::White)
                              .place("b6", kQueen, Color::White)
                              .place("a8", kKing, Color::Black)
                              .place("h7", kPawn, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(evaluator_.evaluate(state), GameStatus::InProgress);
}
