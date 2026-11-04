#pragma once

#include <ostream>

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "ports/GameObserver.hpp"

namespace chess::adapters::console {

// Adaptador de salida: implementa el puerto GameObserver (Observer, §3.9) y dibuja en ASCII.
// Blancas en mayúscula, negras en minúscula, casilla vacía '.'. Usa Piece::symbol() y las
// dimensiones del tablero, así que una pieza nueva o un tablero de 10×10 se dibujan sin cambios.
// Escribe en un ostream inyectado (std::cout en la app, un ostringstream en los tests).
class ConsoleRenderer : public ports::GameObserver {
public:
    explicit ConsoleRenderer(std::ostream& out) : out_(out) {}

    void render(const domain::GameState& state);

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;

private:
    std::ostream& out_;
};

}  // namespace chess::adapters::console
