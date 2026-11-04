#pragma once

namespace chess::ports {

// Motivo por el que el juego rechazó una jugada (D-05: es un resultado esperado, no una excepción).
// PromotionRequired se suma en la fase 7.2, junto con la promoción (D-13).
enum class MoveError {
    NoPieceAtOrigin,  // la casilla de origen está vacía
    NotYourTurn,      // la pieza es del bando que no mueve
    IllegalMove,      // la pieza no puede llegar a ese destino (o deja a su rey en jaque)
    GameOver,         // la partida ya terminó (mate o ahogado): GameOverPhase rechaza todo
};

}  // namespace chess::ports
