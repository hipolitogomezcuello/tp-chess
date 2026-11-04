#include "domain/PieceFactory.hpp"

#include <stdexcept>

namespace chess::domain {

Piece PieceFactory::create(const PieceTypeId& id, Color color) const {
    auto definition = registry_.find(id);
    if (!definition) {
        throw std::invalid_argument("PieceFactory: tipo de pieza no registrado: " + id.value());
    }
    return Piece(std::move(definition), color);
}

}  // namespace chess::domain
