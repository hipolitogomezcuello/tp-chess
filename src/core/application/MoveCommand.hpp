#pragma once

#include <optional>
#include <utility>

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/RulesEngine.hpp"

namespace chess::application {

// Patrón: Command + Memento — ver docs/arquitectura.md §3.8
// Una jugada ya validada, como objeto reversible. execute guarda el GameState previo (el Memento:
// tablero, turno y casilla de al paso) y aplica la jugada; undo restaura ese estado tal cual.
// Rehacer es volver a ejecutar: el RulesEngine es determinista, así que da el mismo resultado.
class MoveCommand {
public:
    // Guarda una referencia al motor: debe vivir más que el comando (composition root).
    // Precondición: `move` es legal en el estado sobre el que se ejecute.
    MoveCommand(const domain::RulesEngine& rules, domain::Move move) : rules_(rules), move_(std::move(move)) {}

    void execute(domain::GameState& state);

    // Precondición: se ejecutó antes (si no, no hace nada).
    void undo(domain::GameState& state) const;

    const domain::Move& move() const { return move_; }

private:
    const domain::RulesEngine& rules_;
    domain::Move move_;
    std::optional<domain::GameState> before_;  // el Memento
};

}  // namespace chess::application
