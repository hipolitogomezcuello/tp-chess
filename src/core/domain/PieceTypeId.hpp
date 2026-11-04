#pragma once

#include <string>
#include <utility>

namespace chess::domain {

// Value object: identificador del tipo de pieza ("pawn", "rook", "archbishop"…).
// A propósito NO es un enum cerrado: agregar una pieza nueva = usar un id nuevo,
// sin modificar ningún switch existente (ver D-03).
class PieceTypeId {
public:
    explicit PieceTypeId(std::string value) : value_(std::move(value)) {}

    const std::string& value() const { return value_; }

    friend bool operator==(const PieceTypeId& a, const PieceTypeId& b) { return a.value_ == b.value_; }
    friend bool operator!=(const PieceTypeId& a, const PieceTypeId& b) { return !(a == b); }
    friend bool operator<(const PieceTypeId& a, const PieceTypeId& b) { return a.value_ < b.value_; }

private:
    std::string value_;
};

}  // namespace chess::domain
