#pragma once

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"

namespace chess::ports {

// Patrón: Observer — ver docs/arquitectura.md §3.9
// Puerto de salida (driven): eventos que el juego emite sin saber quién escucha
// (la UI de consola, la de raylib, un logger…). Se suscribe con GameService::addObserver.
class GameObserver {
public:
    virtual ~GameObserver() = default;

    // Se hizo una jugada; `state` es el estado resultante (ya le toca al otro bando).
    virtual void onMoveMade(const domain::Move& move, const domain::GameState& state) = 0;

    // Después de una jugada, el bando `colorInCheck` (el que mueve ahora) quedó en jaque.
    // Si la jugada además es mate, no se avisa onCheck: se avisa onGameOver.
    virtual void onCheck(domain::Color colorInCheck) = 0;

    // Después de una jugada la partida terminó: `result` es Checkmate o Stalemate y `sideToMove`
    // el bando que se quedó sin jugadas (en un mate, el que perdió).
    virtual void onGameOver(domain::GameStatus result, domain::Color sideToMove) = 0;

    // Se deshizo o rehizo una jugada; `state` es el estado restaurado.
    virtual void onUndoRedo(const domain::GameState& state) = 0;
};

}  // namespace chess::ports
