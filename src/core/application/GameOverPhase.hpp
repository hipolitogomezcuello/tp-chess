#pragma once

#include "application/GamePhase.hpp"

namespace chess::application {

// Patrón: State — ver docs/arquitectura.md §3.7
// La partida terminó (Checkmate o Stalemate): toda jugada se rechaza con MoveError::GameOver.
class GameOverPhase : public GamePhase {
public:
    ports::MoveResult tryMove(ChessGame& game, domain::Position from, domain::Position to) const override;
};

}  // namespace chess::application
