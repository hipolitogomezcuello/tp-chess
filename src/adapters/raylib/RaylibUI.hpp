#pragma once

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

class RaylibUI : public ports::GameObserver {
public:
    explicit RaylibUI(const SpriteCatalog& sprites) : sprites_(sprites) {}

    void run(ports::GameService& game);

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;
    void onUndoRedo(const domain::GameState& state) override;

private:
    const SpriteCatalog& sprites_;
    std::optional<domain::Move> lastMove_;
    std::string message_;
};

}
