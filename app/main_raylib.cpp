#include <memory>

#include "adapters/raylib/RaylibUI.hpp"
#include "adapters/raylib/SpriteCatalog.hpp"
#include "adapters/raylib/StandardSprites.hpp"
#include "application/ChessGame.hpp"
#include "domain/GameState.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/RulesEngine.hpp"
#include "domain/StandardPieces.hpp"
#include "domain/StandardStartingPosition.hpp"
#include "ports/GameService.hpp"

int main() {
    using namespace chess;

    domain::PieceRegistry registry;
    domain::StandardPieces::registerAll(registry);
    const domain::PieceFactory factory(registry);
    const domain::StandardStartingPosition startingPosition;
    const domain::RulesEngine rules(registry);

    adapters::raylib::SpriteCatalog sprites;
    adapters::raylib::StandardSprites::registerAll(sprites);

    const adapters::raylib::GameFactory newGame = [&]() -> std::unique_ptr<ports::GameService> {
        return std::make_unique<application::ChessGame>(
            rules, domain::GameState(startingPosition.createInitialBoard(factory)));
    };

    adapters::raylib::RaylibUI ui(sprites, newGame);
    ui.run();
    return 0;
}
