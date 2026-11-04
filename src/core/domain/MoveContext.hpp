#pragma once

#include "domain/Board.hpp"
#include "domain/GameState.hpp"
#include "domain/Piece.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Lo que una MovementRule necesita leer para generar jugadas: el estado, la casilla de origen
// y la pieza que está ahí. Solo lectura. Agrupar los parámetros en un objeto permite que una
// regla nueva necesite más datos sin cambiar la firma de la interfaz.
class MoveContext {
public:
    // Guarda una referencia al estado: el contexto vive solo durante la generación.
    // Lanza std::invalid_argument si no hay pieza en `from` (error de programación).
    MoveContext(const GameState& state, Position from);

    const GameState& state() const { return state_; }
    const Board& board() const { return state_.board(); }
    Position from() const { return from_; }
    const Piece& piece() const { return piece_; }

private:
    const GameState& state_;
    Position from_;
    Piece piece_;
};

}  // namespace chess::domain
