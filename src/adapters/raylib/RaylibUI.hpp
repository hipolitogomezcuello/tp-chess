#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "adapters/raylib/SpriteCatalog.hpp"
#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "ports/GameObserver.hpp"
#include "ports/GameService.hpp"

namespace chess::adapters::raylib {

using GameFactory = std::function<std::unique_ptr<ports::GameService>()>;

class RaylibUI : public ports::GameObserver {
public:
    RaylibUI(const SpriteCatalog& sprites, GameFactory newGame);

    void run();

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;
    void onUndoRedo(const domain::GameState& state) override;

private:
    std::unique_ptr<ports::GameService> startGame();

    const SpriteCatalog& sprites_;
    GameFactory newGame_;
    std::optional<domain::Move> lastMove_;
    std::string message_;
};

}
