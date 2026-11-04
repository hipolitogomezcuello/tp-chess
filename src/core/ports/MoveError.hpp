#pragma once

namespace chess::ports {

// Motivo por el que el juego rechazó una jugada (D-05: es un resultado esperado, no una excepción).
enum class MoveError {
    NoPieceAtOrigin,    // la casilla de origen está vacía
    NotYourTurn,        // la pieza es del bando que no mueve
    IllegalMove,        // la pieza no puede llegar a ese destino (o deja a su rey en jaque)
    GameOver,           // la partida ya terminó (mate o ahogado): GameOverPhase rechaza todo
    PromotionRequired,  // la jugada es una promoción y no se eligió la pieza
    InvalidPromotion,   // se eligió una pieza no promovible, o una pieza en una jugada que no promueve
};

}  // namespace chess::ports
