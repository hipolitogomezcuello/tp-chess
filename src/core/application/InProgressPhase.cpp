#include "application/InProgressPhase.hpp"

#include "application/ChessGame.hpp"

namespace chess::application {

ports::MoveResult InProgressPhase::tryMove(ChessGame& game, domain::Position from, domain::Position to) const {
    return game.performMove(from, to);
}

}  // namespace chess::application
