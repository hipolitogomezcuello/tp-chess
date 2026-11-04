#include "application/MoveCommand.hpp"

namespace chess::application {

void MoveCommand::execute(domain::GameState& state) {
    before_ = state;
    state = rules_.apply(state, move_);
}

void MoveCommand::undo(domain::GameState& state) const {
    if (before_) {
        state = *before_;
    }
}

}  // namespace chess::application
