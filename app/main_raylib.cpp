// Composition root de la UI gráfica (arquitectura §3.10, D-17).
// Arma exactamente el mismo núcleo que main_console.cpp y solo cambia el adaptador: el núcleo no
// sabe qué UI lo está usando. No hay singletons ni estado global.
//
// Extender en la defensa se hace acá, igual que en la consola:
//   - pieza nueva:   registry.add(...) después de StandardPieces::registerAll y otra disposición;
//   - tablero 10×10: reemplazar StandardStartingPosition por otro StartingPositionProvider.
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

    // Dominio: catálogo de piezas, fábrica, disposición inicial y motor de reglas.
    domain::PieceRegistry registry;
    domain::StandardPieces::registerAll(registry);
    const domain::PieceFactory factory(registry);
    const domain::StandardStartingPosition startingPosition;
    const domain::RulesEngine rules(registry);

    // Adaptador. Se declara antes que el juego: como observer, tiene que vivir más que él (D-16).
    adapters::raylib::RaylibUI ui;

    // Aplicación: el caso de uso, detrás del puerto GameService.
    application::ChessGame game(rules, domain::GameState(startingPosition.createInitialBoard(factory)));
    game.addObserver(ui);

    // La UI solo ve el puerto.
    ports::GameService& service = game;
    ui.run(service);
    return 0;
}
