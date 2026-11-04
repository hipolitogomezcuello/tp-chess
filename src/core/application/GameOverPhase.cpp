#include "application/GameOverPhase.hpp"

#include "ports/MoveError.hpp"

namespace chess::application {

ports::MoveResult GameOverPhase::tryMove(ChessGame& /*game*/, domain::Position /*from*/,
                                         domain::Position /*to*/) const {
    return ports::MoveResult::failure(ports::MoveError::GameOver);
}

}  // namespace chess::application
