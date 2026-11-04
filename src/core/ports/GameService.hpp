#pragma once

#include <vector>

#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "domain/Position.hpp"
#include "ports/GameObserver.hpp"
#include "ports/MoveResult.hpp"

namespace chess::ports {

// Patrón: Ports & Adapters — ver docs/arquitectura.md §3.1
// Puerto de entrada (driving): lo único que los adaptadores (consola, raylib) conocen del núcleo.
// Nunca ven ChessGame ni el RulesEngine. Lo implementa application::ChessGame.
// Fase 7: el parámetro `promotion` de makeMove, undo() y redo() (D-13).
class GameService {
public:
    virtual ~GameService() = default;

    // Intenta mover la pieza de `from` a `to`. Si la jugada es inválida no cambia nada y
    // devuelve el motivo; si es válida la aplica y avisa a los observers.
    virtual MoveResult makeMove(domain::Position from, domain::Position to) = 0;

    // Jugadas que makeMove aceptaría desde `from` (vacío si no hay pieza o no es su turno).
    virtual std::vector<domain::Move> legalMovesFrom(domain::Position from) const = 0;

    virtual const domain::GameState& state() const = 0;
    virtual domain::GameStatus status() const = 0;

    // No toma propiedad: el observer debe vivir más que el juego (composition root).
    virtual void addObserver(GameObserver& observer) = 0;
};

}  // namespace chess::ports
