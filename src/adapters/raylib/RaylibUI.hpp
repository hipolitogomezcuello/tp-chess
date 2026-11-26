#pragma once

#include <optional>
#include <string>

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "ports/GameObserver.hpp"
#include "ports/GameService.hpp"

namespace chess::adapters::raylib {

class RaylibUI : public ports::GameObserver {
public:
    void run(ports::GameService& game);

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;
    void onUndoRedo(const domain::GameState& state) override;

private:
    std::optional<domain::Move> lastMove_;
    std::string message_;
};

}
