#include "application/GameOverPhase.hpp"

#include "ports/MoveError.hpp"

namespace chess::application {

ports::MoveResult GameOverPhase::tryMove(ChessGame&, domain::Position, domain::Position,
                                         const std::optional<domain::PieceTypeId>&) const {
    return ports::MoveResult::failure(ports::MoveError::GameOver);
}

}
