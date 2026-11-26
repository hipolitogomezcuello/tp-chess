#pragma once

#include <vector>

#include "domain/Move.hpp"
#include "domain/MoveContext.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

std::vector<Move> movesOntoReachableSquares(const MoveContext& context, const std::vector<Position>& squares);

}
