#pragma once

#include <optional>

#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "ports/MoveResult.hpp"

namespace chess::application {

class ChessGame;

// Patrón: State — ver docs/arquitectura.md §3.7
// La fase de la partida decide qué pasa con un pedido de jugada. ChessGame delega en su fase actual
// en lugar de llenarse de if (terminada): InProgressPhase juega, GameOverPhase rechaza.
// La fase no tiene datos propios: ChessGame la elige a partir del GameStatus después de cada
// cambio de estado (D-10), así deshacer un mate vuelve a InProgressPhase sin casos especiales.
class GamePhase {
public:
    virtual ~GamePhase() = default;

    virtual ports::MoveResult tryMove(ChessGame& game, domain::Position from, domain::Position to,
                                      const std::optional<domain::PieceTypeId>& promotion) const = 0;
};

}  // namespace chess::application
