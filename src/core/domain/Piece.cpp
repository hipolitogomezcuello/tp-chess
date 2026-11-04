#include "domain/Piece.hpp"

#include <stdexcept>
#include <utility>

namespace chess::domain {

Piece::Piece(std::shared_ptr<const PieceDefinition> definition, Color color, bool hasMoved)
    : definition_(std::move(definition)), color_(color), hasMoved_(hasMoved) {
    if (!definition_) {
        throw std::invalid_argument("Piece: la definición no puede ser nula");
    }
}

Piece Piece::withMoved() const {
    return Piece(definition_, color_, true);
}

}  // namespace chess::domain
