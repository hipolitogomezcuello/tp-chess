#pragma once

#include <vector>

#include "application/MoveCommand.hpp"
#include "domain/GameState.hpp"

namespace chess::application {

class MoveHistory {
public:
    void record(MoveCommand executed);

    bool undo(domain::GameState& state);
    bool redo(domain::GameState& state);

    bool canUndo() const { return !undoStack_.empty(); }
    bool canRedo() const { return !redoStack_.empty(); }

private:
    std::vector<MoveCommand> undoStack_;
    std::vector<MoveCommand> redoStack_;
};

}
