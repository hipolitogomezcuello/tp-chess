#pragma once

#include <optional>
#include <utility>

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/RulesEngine.hpp"

namespace chess::application {

class MoveCommand {
public:
    MoveCommand(const domain::RulesEngine& rules, domain::Move move) : rules_(rules), move_(std::move(move)) {}

    void execute(domain::GameState& state);

    void undo(domain::GameState& state) const;

    const domain::Move& move() const { return move_; }

private:
    const domain::RulesEngine& rules_;
    domain::Move move_;
    std::optional<domain::GameState> before_;
};

}
