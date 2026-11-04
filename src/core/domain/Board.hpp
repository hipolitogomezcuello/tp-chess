#pragma once

#include <optional>
#include <vector>

#include "domain/BoardDimensions.hpp"
#include "domain/Color.hpp"
#include "domain/Piece.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Grilla de casillas (cada una con una pieza o vacía) del tamaño que indiquen las dimensiones.
// No conoce reglas de juego: solo guarda y mueve piezas. Es copiable por valor, así el motor
// simula jugadas sobre una copia (LegalMoveFilter) sin tocar el original.
//
// Las posiciones fuera del tablero son errores de programación: lanzan std::out_of_range.
// Las reglas preguntan antes con dimensions().contains().
class Board {
public:
    explicit Board(BoardDimensions dimensions);

    const BoardDimensions& dimensions() const { return dimensions_; }

    std::optional<Piece> pieceAt(Position position) const;
    bool isEmpty(Position position) const;

    // Pone la pieza en la casilla, reemplazando la que hubiera.
    void place(Position position, Piece piece);
    void remove(Position position);

    // Lleva la pieza de from a to; lo que hubiera en to desaparece (captura).
    // No marca hasMoved: eso es una regla de juego (MoveApplier).
    // Lanza std::invalid_argument si from está vacía.
    void move(Position from, Position to);

    std::vector<Position> positionsOf(Color color) const;

private:
    std::size_t indexOf(Position position) const;

    BoardDimensions dimensions_;
    std::vector<std::optional<Piece>> squares_;
};

}  // namespace chess::domain
