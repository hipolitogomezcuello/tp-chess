#pragma once

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/Move.hpp"

namespace chess::ports {

// Patrón: Observer — ver docs/arquitectura.md §3.9
// Puerto de salida (driven): eventos que el juego emite sin saber quién escucha
// (la UI de consola, la de raylib, un logger…). Se suscribe con GameService::addObserver.
// onGameOver (mate/ahogado) y onUndoRedo se suman en la fase 7 (D-13).
class GameObserver {
public:
    virtual ~GameObserver() = default;

    // Se hizo una jugada; `state` es el estado resultante (ya le toca al otro bando).
    virtual void onMoveMade(const domain::Move& move, const domain::GameState& state) = 0;

    // Después de una jugada, el bando `colorInCheck` (el que mueve ahora) quedó en jaque.
    virtual void onCheck(domain::Color colorInCheck) = 0;
};

}  // namespace chess::ports
