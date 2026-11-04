#pragma once

#include "domain/Color.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

// Patrón: Factory — ver docs/arquitectura.md §3.4
// Crea piezas a partir de (tipo, color) consultando el registro. Nadie más arma una Piece
// a mano con su definición: así la creación queda en un solo lugar.
class PieceFactory {
public:
    // Guarda una referencia: el registro debe vivir más que la fábrica (composition root).
    explicit PieceFactory(const PieceRegistry& registry) : registry_(registry) {}

    // Lanza std::invalid_argument si el tipo no está registrado (error de programación).
    Piece create(const PieceTypeId& id, Color color) const;

private:
    const PieceRegistry& registry_;
};

}  // namespace chess::domain
