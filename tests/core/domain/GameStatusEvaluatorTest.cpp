#include "domain/GameStatusEvaluator.hpp"

#include "domain/AttackDetector.hpp"
#include "domain/GameStatus.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::AttackDetector;
using chess::domain::Color;
using chess::domain::GameState;
using chess::domain::GameStatus;
using chess::domain::GameStatusEvaluator;

namespace {

class GameStatusEvaluatorTest : public chess::testing::StandardPiecesFixture {
protected:
    GameStatusEvaluatorTest() : evaluator_(detector_) {}

    AttackDetector detector_;
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
