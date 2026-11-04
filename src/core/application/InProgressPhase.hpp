#pragma once

#include "application/GamePhase.hpp"

namespace chess::application {

// Patrón: State — ver docs/arquitectura.md §3.7
// La partida sigue (InProgress o Check): la jugada se valida y se hace (ChessGame::performMove).
class InProgressPhase : public GamePhase {
public:
    ports::MoveResult tryMove(ChessGame& game, domain::Position from, domain::Position to,
                              const std::optional<domain::PieceTypeId>& promotion) const override;
};

}  // namespace chess::application
