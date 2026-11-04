#pragma once

#include <optional>
#include <vector>

#include "domain/Move.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "ports/GameService.hpp"

namespace chess::adapters::raylib {

// Lo que significa cada click sobre el tablero, sin nada de raylib (se testea sin ventana):
//   1. click en una pieza propia → queda seleccionada y se resaltan sus jugadas (legalMovesFrom);
//   2. click en una casilla resaltada → se hace la jugada; si es una promoción, primero se espera
//      a que el jugador elija la pieza entre promotionChoices() (D-19);
//   3. cualquier otro click → se deselecciona.
// Solo conoce el puerto GameService, igual que ConsoleUI.
class BoardInteraction {
public:
    explicit BoardInteraction(ports::GameService& game) : game_(game) {}

    void click(domain::Position square);

    // Elige la pieza de la promoción pendiente y hace la jugada.
    void choosePromotion(const domain::PieceTypeId& type);

    // Descarta la selección y la promoción pendiente.
    void cancel();

    bool undo();
    bool redo();

    const std::optional<domain::Position>& selected() const { return selected_; }
    const std::vector<domain::Move>& highlightedMoves() const { return highlighted_; }
    bool awaitingPromotion() const { return pendingPromotion_.has_value(); }
    std::vector<domain::Piece> promotionChoices() const { return game_.promotionChoices(); }

private:
    void select(domain::Position square);

    ports::GameService& game_;
    std::optional<domain::Position> selected_;
    std::vector<domain::Move> highlighted_;
    std::optional<domain::Move> pendingPromotion_;  // la jugada de promoción esperando la pieza
};

}  // namespace chess::adapters::raylib
