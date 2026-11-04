#pragma once

#include <optional>

#include "domain/BoardDimensions.hpp"
#include "domain/Position.hpp"

namespace chess::adapters::raylib {

// Geometría del tablero en pantalla: traduce píxeles ↔ casillas. Las negras van arriba, así que la
// fila 1 es la de abajo. No incluye raylib (es aritmética pura), por eso se testea sin abrir una ventana.
// Usa las dimensiones reales del tablero: un 10×10 entra en el mismo espacio con casillas más chicas.
class BoardLayout {
public:
    struct Point {
        int x;
        int y;

        friend bool operator==(Point a, Point b) { return a.x == b.x && a.y == b.y; }
    };

    // El tablero se dibuja en un cuadrado de `boardPixels` de lado con la esquina superior izquierda
    // en (originX, originY).
    BoardLayout(domain::BoardDimensions dimensions, int originX, int originY, int boardPixels);

    int squareSize() const { return squareSize_; }
    Point topLeftOf(domain::Position position) const;

    // La casilla bajo el píxel (x, y), o nullopt si cae fuera del tablero.
    std::optional<domain::Position> squareAt(int x, int y) const;

private:
    domain::BoardDimensions dimensions_;
    int originX_;
    int originY_;
    int squareSize_;
};

}  // namespace chess::adapters::raylib
