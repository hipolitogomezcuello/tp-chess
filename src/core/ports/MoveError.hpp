#pragma once

namespace chess::ports {

enum class MoveError {
    NoPieceAtOrigin,
    NotYourTurn,
    IllegalMove,
    GameOver,
    PromotionRequired,
    InvalidPromotion,
};

}
