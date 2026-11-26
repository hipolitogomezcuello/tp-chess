#pragma once

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/PieceFactory.hpp"

namespace chess::domain {

class MoveApplier {
public:
    explicit MoveApplier(const PieceFactory& factory) : factory_(factory) {}

    GameState apply(const GameState& state, const Move& move) const;

private:
    const PieceFactory& factory_;
};

}
