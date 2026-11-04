#pragma once

#include <optional>

#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

enum class MoveKind {
    Normal,
    Capture,
    DoublePawnPush,
    EnPassant,   // misión 7.4
    Castling,
    Promotion,
};

// Value object inmutable: una jugada de `from` a `to`.
// En una Promotion la regla NO elige la pieza: la elige el jugador con withPromotion (D-08).
// Un Castling lleva además la jugada de la torre (rookFrom → rookTo): se crea con Move::castling.
class Move {
public:
    Move(Position from, Position to, MoveKind kind = MoveKind::Normal) : from_(from), to_(to), kind_(kind) {}

    // El rey va de kingFrom a kingTo y la pieza canCastleWith (la torre) de rookFrom a rookTo.
    static Move castling(Position kingFrom, Position kingTo, Position rookFrom, Position rookTo);

    Position from() const { return from_; }
    Position to() const { return to_; }
    MoveKind kind() const { return kind_; }
    const std::optional<PieceTypeId>& promotion() const { return promotion_; }
    std::optional<Position> rookFrom() const { return rookFrom_; }  // solo en Castling
    std::optional<Position> rookTo() const { return rookTo_; }      // solo en Castling

    Move withPromotion(PieceTypeId type) const;

    friend bool operator==(const Move& a, const Move& b) {
        return a.from_ == b.from_ && a.to_ == b.to_ && a.kind_ == b.kind_ && a.promotion_ == b.promotion_
            && a.rookFrom_ == b.rookFrom_ && a.rookTo_ == b.rookTo_;
    }
    friend bool operator!=(const Move& a, const Move& b) { return !(a == b); }

private:
    Position from_;
    Position to_;
    MoveKind kind_;
    std::optional<PieceTypeId> promotion_;
    std::optional<Position> rookFrom_;
    std::optional<Position> rookTo_;
};

}  // namespace chess::domain
