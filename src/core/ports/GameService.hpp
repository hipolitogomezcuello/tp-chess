#pragma once

#include <optional>
#include <vector>

#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "ports/GameObserver.hpp"
#include "ports/MoveResult.hpp"

namespace chess::ports {

// Patrón: Ports & Adapters — ver docs/arquitectura.md §3.1
// Puerto de entrada (driving): lo único que los adaptadores (consola, raylib) conocen del núcleo.
// Nunca ven ChessGame ni el RulesEngine. Lo implementa application::ChessGame.
class GameService {
public:
    virtual ~GameService() = default;

    // Intenta mover la pieza de `from` a `to`. Si la jugada es inválida no cambia nada y
    // devuelve el motivo; si es válida la aplica y avisa a los observers.
    // `promotion` es la pieza elegida cuando la jugada es una promoción (una de promotionChoices).
    virtual MoveResult makeMove(domain::Position from, domain::Position to,
                                std::optional<domain::PieceTypeId> promotion = std::nullopt) = 0;

    // Jugadas que makeMove aceptaría desde `from` (vacío si no hay pieza o no es su turno).
    virtual std::vector<domain::Move> legalMovesFrom(domain::Position from) const = 0;

    virtual const domain::GameState& state() const = 0;
    virtual domain::GameStatus status() const = 0;

    // Piezas a las que puede promover el bando que mueve: la UI muestra su símbolo y manda su tipo.
    virtual std::vector<domain::Piece> promotionChoices() const = 0;

    // Deshacen o rehacen la última jugada y avisan onUndoRedo. false si no había nada que hacer.
    virtual bool undo() = 0;
    virtual bool redo() = 0;

    // No toma propiedad: el observer debe vivir más que el juego (composition root).
    virtual void addObserver(GameObserver& observer) = 0;
};

}  // namespace chess::ports
