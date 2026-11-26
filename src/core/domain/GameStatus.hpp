#pragma once

namespace chess::domain {

enum class GameStatus {
    InProgress,
    Check,
    Checkmate,
    Stalemate,
};

}
