#pragma once

#include <optional>

#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

enum class MoveKind {
    Normal,
    Capture,
    DoublePawnPush,
    EnPassant,   // fase 7
    Castling,    // fase 7
    Promotion,
};

// Value object inmutable: una jugada de `from` a `to`.
// En una Promotion la regla NO elige la pieza: la elige el jugador con withPromotion (D-08).
// Los datos del enroque y del al paso (rookFrom, capturedSquare…) se agregan en la fase 7.
class Move {
public:
    Move(Position from, Position to, MoveKind kind = MoveKind::Normal) : from_(from), to_(to), kind_(kind) {}

    Position from() const { return from_; }
    Position to() const { return to_; }
    MoveKind kind() const { return kind_; }
    const std::optional<PieceTypeId>& promotion() const { return promotion_; }

    Move withPromotion(PieceTypeId type) const;

    friend bool operator==(const Move& a, const Move& b) {
        return a.from_ == b.from_ && a.to_ == b.to_ && a.kind_ == b.kind_ && a.promotion_ == b.promotion_;
    }
    friend bool operator!=(const Move& a, const Move& b) { return !(a == b); }

private:
    Position from_;
    Position to_;
    MoveKind kind_;
    std::optional<PieceTypeId> promotion_;
};

}  // namespace chess::domain
