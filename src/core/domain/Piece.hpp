#pragma once

#include <memory>

#include "domain/Color.hpp"
#include "domain/PieceDefinition.hpp"

namespace chess::domain {

class Piece {
public:
    Piece(std::shared_ptr<const PieceDefinition> definition, Color color, bool hasMoved = false);

    const PieceTypeId& type() const { return definition_->id(); }
    Color color() const { return color_; }
    char symbol() const { return definition_->symbol(); }
    const PieceTraits& traits() const { return definition_->traits(); }
    const PieceDefinition& definition() const { return *definition_; }
    bool hasMoved() const { return hasMoved_; }

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

}
