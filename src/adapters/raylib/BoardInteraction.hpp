#pragma once

#include <optional>
#include <vector>

#include "domain/Move.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "ports/GameService.hpp"

namespace chess::adapters::raylib {

class BoardInteraction {
public:
    explicit BoardInteraction(ports::GameService& game) : game_(game) {}

    void click(domain::Position square);

    void choosePromotion(const domain::PieceTypeId& type);

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
    std::optional<domain::Move> pendingPromotion_;
};

}
