#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include "adapters/console/ConsoleRenderer.hpp"
#include "adapters/console/ConsoleUI.hpp"
#include "application/ChessGame.hpp"
#include "domain/GameState.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/RulesEngine.hpp"
#include "domain/StandardPieces.hpp"
#include "domain/StandardStartingPosition.hpp"

namespace {

void showUtf8AccentsInWindowsConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

}

int main() {
    showUtf8AccentsInWindowsConsole();

    using namespace chess;

    domain::PieceRegistry registry;
    domain::StandardPieces::registerAll(registry);
    const domain::PieceFactory factory(registry);
    const domain::StandardStartingPosition startingPosition;
    const domain::RulesEngine rules(registry);

    adapters::console::ConsoleRenderer renderer(std::cout);

    application::ChessGame game(rules, domain::GameState(startingPosition.createInitialBoard(factory)));
    game.addObserver(renderer);

    ports::GameService& service = game;
    std::cout << "TPO Ajedrez\n";
    renderer.render(service.state());
    adapters::console::ConsoleUI(service, std::cin, std::cout).run();
    return 0;
}
