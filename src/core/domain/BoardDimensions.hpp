#pragma once

#include "domain/Position.hpp"

namespace chess::domain {

// Value object: tamaño del tablero (files × ranks), 8×8 por defecto.
// Es la única fuente de verdad sobre los bordes: ninguna regla asume 8×8 (ver D-09).
// Cambiar el tamaño del tablero = construir otro BoardDimensions, sin tocar ninguna clase.
class BoardDimensions {
public:
    static constexpr int kStandardSize = 8;

    // Lanza std::invalid_argument si algún lado no es positivo (error de programación).
    explicit BoardDimensions(int files = kStandardSize, int ranks = kStandardSize);

    int files() const { return files_; }
    int ranks() const { return ranks_; }

    bool contains(Position position) const;

    friend bool operator==(const BoardDimensions& a, const BoardDimensions& b) {
        return a.files_ == b.files_ && a.ranks_ == b.ranks_;
    }
    friend bool operator!=(const BoardDimensions& a, const BoardDimensions& b) { return !(a == b); }

private:
    int files_;
    int ranks_;
};

}  // namespace chess::domain
