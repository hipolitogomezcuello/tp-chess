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

// Adaptador gráfico: ventana de raylib que dibuja el tablero y traduce el mouse y el teclado a
// llamadas al puerto GameService (a través de BoardInteraction). Es a la vez adaptador de entrada
// (driving) y de salida: implementa GameObserver para mostrar la última jugada y los avisos de jaque,
// mate y ahogado. Solo conoce los puertos y los value objects del dominio, igual que la consola.
//
// Las piezas se dibujan con Piece::symbol() dentro de un círculo y el tablero con las dimensiones
// reales: una pieza nueva o un tablero de 10×10 se ven sin tocar esta clase (D-21).
// raylib solo se incluye en el .cpp: este header no la expone.
class RaylibUI : public ports::GameObserver {
public:
    // Abre la ventana y corre el loop hasta que se cierre. Antes hay que suscribir este observer al
    // juego (game.addObserver(ui)): así la UI se crea antes que el juego y vive más que él (D-16).
    void run(ports::GameService& game);

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;
    void onUndoRedo(const domain::GameState& state) override;

private:
    std::optional<domain::Move> lastMove_;
    std::string message_;
};

}  // namespace chess::adapters::raylib
