#pragma once

#include "domain/Color.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

class PieceFactory {
public:
    explicit PieceFactory(const PieceRegistry& registry) : registry_(registry) {}

    Piece create(const PieceTypeId& id, Color color) const;

private:
    const PieceRegistry& registry_;
};

}
