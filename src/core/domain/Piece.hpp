#pragma once

#include <memory>

#include "domain/Color.hpp"
#include "domain/PieceDefinition.hpp"

namespace chess::domain {

// Value object liviano y copiable: una pieza concreta sobre el tablero.
// Tipo (definición compartida) + color + si ya se movió. hasMoved alcanza para el doble
// avance del peón (D-09) y para derivar los derechos de enroque (D-06).
class Piece {
public:
    // Lanza std::invalid_argument si la definición es nula (error de programación).
    Piece(std::shared_ptr<const PieceDefinition> definition, Color color, bool hasMoved = false);

    const PieceTypeId& type() const { return definition_->id(); }
    Color color() const { return color_; }
    char symbol() const { return definition_->symbol(); }
    const PieceTraits& traits() const { return definition_->traits(); }
    const PieceDefinition& definition() const { return *definition_; }
    bool hasMoved() const { return hasMoved_; }

    // Copia marcada como movida. Piece es inmutable: no hay setters.
    Piece withMoved() const;

    friend bool operator==(const Piece& a, const Piece& b) {
        return a.type() == b.type() && a.color_ == b.color_ && a.hasMoved_ == b.hasMoved_;
    }
    friend bool operator!=(const Piece& a, const Piece& b) { return !(a == b); }

private:
    std::shared_ptr<const PieceDefinition> definition_;
    Color color_;
    bool hasMoved_;
};

}  // namespace chess::domain
