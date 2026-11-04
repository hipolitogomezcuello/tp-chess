#include "ports/MoveResult.hpp"

#include <gtest/gtest.h>

#include "domain/Move.hpp"
#include "ports/MoveError.hpp"
#include "support/Squares.hpp"

using chess::domain::Move;
using chess::ports::MoveError;
using chess::ports::MoveResult;
using chess::testing::sq;

TEST(MoveResultTest, SuccessIsOkAndCarriesTheMove) {
    const Move move(sq("e2"), sq("e4"));
    const MoveResult result = MoveResult::success(move);

    EXPECT_TRUE(result.ok());
    ASSERT_TRUE(result.move().has_value());
    EXPECT_EQ(*result.move(), move);
    EXPECT_FALSE(result.error().has_value());
}

TEST(MoveResultTest, FailureIsNotOkAndCarriesTheError) {
    const MoveResult result = MoveResult::failure(MoveError::IllegalMove);

    EXPECT_FALSE(result.ok());
    EXPECT_EQ(result.error(), MoveError::IllegalMove);
    EXPECT_FALSE(result.move().has_value());
}
