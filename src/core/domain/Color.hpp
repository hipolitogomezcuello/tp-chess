#pragma once

namespace chess::domain {

// Color de una pieza o del jugador al que le toca mover.
enum class Color { White, Black };

// Devuelve el color contrario. Función libre: un enum no puede tener métodos.
Color opposite(Color color);

}  // namespace chess::domain
