// Composition root de la UI por consola (arquitectura §3.10, D-17).
// Es el único lugar que conoce el dominio, la aplicación y el adaptador: arma el grafo de objetos
// a mano, por constructor, y le pasa el control a la UI. No hay singletons ni estado global.
//
// Extender en la defensa se hace acá, sin tocar ninguna clase existente:
//   - pieza nueva:   registry.add(...) después de StandardPieces::registerAll y otra disposición;
//   - tablero 10×10: reemplazar StandardStartingPosition por otro StartingPositionProvider.
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

int main() {
#ifdef _WIN32
    // Los mensajes tienen tildes y signos (¡) en UTF-8: que la consola de Windows los muestre bien.
    SetConsoleOutputCP(CP_UTF8);
#endif

    using namespace chess;

    // Dominio: catálogo de piezas, fábrica, disposición inicial y motor de reglas.
    domain::PieceRegistry registry;
    domain::StandardPieces::registerAll(registry);
    const domain::PieceFactory factory(registry);
    const domain::StandardStartingPosition startingPosition;
    const domain::RulesEngine rules(registry);

    // Adaptador de salida. Se declara antes que el juego: el observer tiene que vivir más que él.
    adapters::console::ConsoleRenderer renderer(std::cout);

    // Aplicación: el caso de uso, detrás del puerto GameService.
    application::ChessGame game(rules, domain::GameState(startingPosition.createInitialBoard(factory)));
    game.addObserver(renderer);

    // Adaptador de entrada: solo ve el puerto.
    ports::GameService& service = game;
    std::cout << "TPO Ajedrez\n";
    renderer.render(service.state());
    adapters::console::ConsoleUI(service, std::cin, std::cout).run();
    return 0;
}
