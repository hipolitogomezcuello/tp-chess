#pragma once

#include <optional>
#include <utility>

#include "domain/Move.hpp"
#include "ports/MoveError.hpp"

namespace chess::ports {

class MoveResult {
public:
    static MoveResult success(domain::Move move) { return MoveResult(std::move(move), std::nullopt); }
    static MoveResult failure(MoveError error) { return MoveResult(std::nullopt, error); }

    bool ok() const { return move_.has_value(); }
    const std::optional<domain::Move>& move() const { return move_; }
    std::optional<MoveError> error() const { return error_; }

private:
    MoveResult(std::optional<domain::Move> move, std::optional<MoveError> error)
        : move_(std::move(move)), error_(error) {}

    std::optional<domain::Move> move_;
    std::optional<MoveError> error_;
};

}
