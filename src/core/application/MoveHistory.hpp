#pragma once

#include <vector>

#include "application/MoveCommand.hpp"
#include "domain/GameState.hpp"

namespace chess::application {

// Patrón: Command — ver docs/arquitectura.md §3.8
// Dos pilas de comandos ya ejecutados: deshacer saca el último de la pila de deshacer, lo revierte y
// lo pasa a la de rehacer; rehacer hace lo contrario. Una jugada nueva vacía la pila de rehacer
// (la historia alternativa se pierde, como en cualquier editor).
class MoveHistory {
public:
    // Anota una jugada que ya se ejecutó.
    void record(MoveCommand executed);

    // Devuelven false (y no tocan el estado) si no hay nada para deshacer o rehacer.
    bool undo(domain::GameState& state);
    bool redo(domain::GameState& state);

    bool canUndo() const { return !undoStack_.empty(); }
    bool canRedo() const { return !redoStack_.empty(); }

private:
    std::vector<MoveCommand> undoStack_;
    std::vector<MoveCommand> redoStack_;
};

}  // namespace chess::application
