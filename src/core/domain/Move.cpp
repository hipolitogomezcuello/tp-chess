#include "domain/Move.hpp"

#include <utility>

namespace chess::domain {

Move Move::castling(Position kingFrom, Position kingTo, Position rookFrom, Position rookTo) {
    Move move(kingFrom, kingTo, MoveKind::Castling);
    move.rookFrom_ = rookFrom;
    move.rookTo_ = rookTo;
    return move;
}

Move Move::enPassant(Position from, Position to, Position capturedSquare) {
    Move move(from, to, MoveKind::EnPassant);
    move.capturedSquare_ = capturedSquare;
    return move;
}

Move Move::withPromotion(PieceTypeId type) const {
    Move copy = *this;
    copy.promotion_ = std::move(type);
    return copy;
}

}  // namespace chess::domain
