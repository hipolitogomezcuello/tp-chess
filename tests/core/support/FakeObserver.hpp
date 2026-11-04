#pragma once

// Observer de prueba: registra los eventos que recibe para que los tests los inspeccionen
// (docs/arquitectura.md §8). No dibuja nada: reemplaza a la UI en los tests del núcleo.
#include <vector>

#include "domain/Color.hpp"
#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "ports/GameObserver.hpp"

namespace chess::testing {

// Un aviso de fin de partida: el resultado y el bando que quedó sin jugadas.
struct GameOverEvent {
    domain::GameStatus result;
    domain::Color sideToMove;
};

class FakeObserver : public ports::GameObserver {
public:
    void onMoveMade(const domain::Move& move, const domain::GameState& state) override {
        movesMade.push_back(move);
        statesAfterMove.push_back(state);
    }

    void onCheck(domain::Color colorInCheck) override { checks.push_back(colorInCheck); }

    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override {
        gameOvers.push_back({result, sideToMove});
    }

    void onUndoRedo(const domain::GameState& state) override { statesAfterUndoRedo.push_back(state); }

    std::vector<domain::Move> movesMade;
    std::vector<domain::GameState> statesAfterMove;
    std::vector<domain::Color> checks;
    std::vector<GameOverEvent> gameOvers;
    std::vector<domain::GameState> statesAfterUndoRedo;
};

}  // namespace chess::testing
