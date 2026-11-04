#pragma once

namespace chess::domain {

// Estado de la partida desde el punto de vista del bando que mueve.
// Lo calcula GameStatusEvaluator. Checkmate y Stalemate terminan la partida (GameOverPhase).
enum class GameStatus {
    InProgress,
    Check,
    Checkmate,
    Stalemate,
};

}  // namespace chess::domain
