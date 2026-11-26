#pragma once

#include "application/GamePhase.hpp"

namespace chess::application {

class GameOverPhase : public GamePhase {
public:
    ports::MoveResult tryMove(ChessGame& game, domain::Position from, domain::Position to,
                              const std::optional<domain::PieceTypeId>& promotion) const override;
};

}
