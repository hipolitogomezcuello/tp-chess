#pragma once

#include "domain/Offset.hpp"

namespace chess::domain {

// Value object inmutable: una casilla (file, rank) con índices base 0.
// "a1" = (0, 0), "h8" = (7, 7) en un tablero estándar. La notación algebraica es
// cosa de la presentación (AlgebraicNotation, en el adaptador), no del dominio.
// Position no conoce el tamaño del tablero: si queda adentro o afuera lo decide
// BoardDimensions::contains (ver D-09).
class Position {
public:
    constexpr Position(int file, int rank) : file_(file), rank_(rank) {}

    constexpr int file() const { return file_; }
    constexpr int rank() const { return rank_; }

    constexpr Position operator+(Offset offset) const {
        return Position(file_ + offset.dFile(), rank_ + offset.dRank());
    }

    friend constexpr bool operator==(Position a, Position b) {
        return a.file_ == b.file_ && a.rank_ == b.rank_;
    }
    friend constexpr bool operator!=(Position a, Position b) { return !(a == b); }

    // Orden total (primero file, después rank) para poder usar Position en std::set/std::map.
    friend constexpr bool operator<(Position a, Position b) {
        return a.file_ != b.file_ ? a.file_ < b.file_ : a.rank_ < b.rank_;
    }

private:
    int file_;
    int rank_;
};

}  // namespace chess::domain
