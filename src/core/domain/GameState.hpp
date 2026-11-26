#pragma once

#include <optional>
#include <utility>

#include "domain/Board.hpp"
#include "domain/Color.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

class GameState {
public:
    explicit GameState(Board board, Color sideToMove = Color::White,
                       std::optional<Position> enPassantTarget = std::nullopt)
        : board_(std::move(board)), sideToMove_(sideToMove), enPassantTarget_(enPassantTarget) {}

    const Board& board() const { return board_; }
    Color sideToMove() const { return sideToMove_; }
    std::optional<Position> enPassantTarget() const { return enPassantTarget_; }

private:
    Board board_;
    Color sideToMove_;
    std::optional<Position> enPassantTarget_;
};

}
