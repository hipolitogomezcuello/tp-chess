#include "adapters/raylib/RaylibUI.hpp"
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

    adapters::raylib::RaylibUI ui;

    application::ChessGame game(rules, domain::GameState(startingPosition.createInitialBoard(factory)));
    game.addObserver(ui);

    ports::GameService& service = game;
    ui.run(service);
    return 0;
}
