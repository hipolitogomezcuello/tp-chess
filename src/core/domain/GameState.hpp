#pragma once

#include <optional>
#include <utility>

#include "domain/Board.hpp"
#include "domain/Color.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Foto completa de la partida: tablero + a quién le toca + casilla de al paso (la que cruzó el peón
// que acaba de avanzar dos; la marca MoveApplier y la lee PawnRule).
// Es un value object copiable: el motor devuelve estados nuevos en lugar de mutar (MoveApplier)
// y la aplicación lo guarda como Memento para deshacer (arquitectura §3.8).
// Los derechos de enroque no se guardan: se derivan de Piece::hasMoved (D-06).
class GameState {
public:
    explicit GameState(Board board, Color sideToMove = Color::White,
                       std::optional<Position> enPassantTarget = std::nullopt)
        : board_(std::move(board)), sideToMove_(sideToMove), enPassantTarget_(enPassantTarget) {}

    const Board& board() const { return board_; }
    Color sideToMove() const { return sideToMove_; }
    std::optional<Position> enPassantTarget() const { return enPassantTarget_; }

private:
    Board board_;
    Color sideToMove_;
    std::optional<Position> enPassantTarget_;
};

}  // namespace chess::domain
