#include "application/MoveHistory.hpp"

#include <utility>

namespace chess::application {

void MoveHistory::record(MoveCommand executed) {
    undoStack_.push_back(std::move(executed));
    redoStack_.clear();
}

bool MoveHistory::undo(domain::GameState& state) {
    if (!canUndo()) {
        return false;
    }
    MoveCommand command = std::move(undoStack_.back());
    undoStack_.pop_back();
    command.undo(state);
    redoStack_.push_back(std::move(command));
    return true;
}

bool MoveHistory::redo(domain::GameState& state) {
    if (!canRedo()) {
        return false;
    }
    MoveCommand command = std::move(redoStack_.back());
    redoStack_.pop_back();
    command.execute(state);
    undoStack_.push_back(std::move(command));
    return true;
}

}  // namespace chess::application
