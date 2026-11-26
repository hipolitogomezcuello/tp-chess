#pragma once

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"

namespace chess::ports {

class GameObserver {
public:
    virtual ~GameObserver() = default;

    virtual void onMoveMade(const domain::Move& move, const domain::GameState& state) = 0;

    virtual void onCheck(domain::Color colorInCheck) = 0;

    virtual void onGameOver(domain::GameStatus result, domain::Color sideToMove) = 0;

    virtual void onUndoRedo(const domain::GameState& state) = 0;
};

}
