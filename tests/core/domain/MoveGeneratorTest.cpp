#include "domain/MoveGenerator.hpp"

#include "domain/Directions.hpp"
#include "domain/LeaperRule.hpp"
#include "domain/SlidingRule.hpp"
#include "support/MoveAssertions.hpp"
#include "support/PieceTestSupport.hpp"
#include "support/StandardFixture.hpp"

using chess::domain::Color;
using chess::domain::Directions;
using chess::domain::GameState;
using chess::domain::LeaperRule;
using chess::domain::MoveGenerator;
using chess::domain::PieceTypeId;
using chess::domain::SlidingRule;
using chess::testing::destinations;
using chess::testing::makeDefinition;
using chess::testing::rulesOf;
using chess::testing::sq;
using chess::testing::squares;

using MoveGeneratorTest = chess::testing::StandardPiecesFixture;

TEST_F(MoveGeneratorTest, EmptySquareHasNoMoves) {
    const MoveGenerator generator;
    EXPECT_TRUE(generator.movesFrom(GameState(builder().build()), sq("e4")).empty());
}

TEST_F(MoveGeneratorTest, CollectsTheMovesOfThePieceRules) {
    const MoveGenerator generator;
    const GameState state(builder().place("g1", kKnight, Color::White).build());

    EXPECT_EQ(destinations(generator.movesFrom(state, sq("g1"))), squares({"e2", "f3", "h3"}));
}

TEST_F(MoveGeneratorTest, JoinsEveryRuleOfACompositePiece) {
    // Arzobispo = alfil + caballo: el generador no pregunta el tipo, recorre las reglas.
    registry_.add(makeDefinition("archbishop", 'A',
                                 rulesOf(std::make_unique<SlidingRule>(Directions::diagonals()),
                                         std::make_unique<LeaperRule>(Directions::knightJumps()))));
    const MoveGenerator generator;
    const GameState state(builder().place("d4", PieceTypeId("archbishop"), Color::White).build());

    EXPECT_EQ(generator.movesFrom(state, sq("d4")).size(), 13u + 8u);
}

TEST_F(MoveGeneratorTest, InTheInitialPositionWhiteHasTwentyMoves) {
    const MoveGenerator generator;
    EXPECT_EQ(generator.allMoves(initialState()).size(), 20u);
}

TEST_F(MoveGeneratorTest, AllMovesOnlyIncludesTheSideToMove) {
    const MoveGenerator generator;
    const GameState state(builder()
                              .place("a1", kRook, Color::White)
                              .place("h8", kKnight, Color::Black)
                              .build(),
                          Color::Black);

    EXPECT_EQ(destinations(generator.allMoves(state)), squares({"f7", "g6"}));
}
