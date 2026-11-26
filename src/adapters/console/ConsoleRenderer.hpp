#pragma once

#include <ostream>

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "ports/GameObserver.hpp"

namespace chess::adapters::console {

class ConsoleRenderer : public ports::GameObserver {
public:
    explicit ConsoleRenderer(std::ostream& out) : out_(out) {}

    void render(const domain::GameState& state);

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;
    void onUndoRedo(const domain::GameState& state) override;

private:
    std::ostream& out_;
};

}
