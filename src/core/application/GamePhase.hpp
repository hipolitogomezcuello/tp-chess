#pragma once

#include <optional>

#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "ports/MoveResult.hpp"

namespace chess::application {

class ChessGame;

class GamePhase {
public:
    virtual ~GamePhase() = default;

    virtual ports::MoveResult tryMove(ChessGame& game, domain::Position from, domain::Position to,
                                      const std::optional<domain::PieceTypeId>& promotion) const = 0;
};

}
