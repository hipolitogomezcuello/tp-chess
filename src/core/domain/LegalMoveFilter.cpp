#include "domain/LegalMoveFilter.hpp"

#include "domain/Offset.hpp"

namespace chess::domain {

std::vector<Move> LegalMoveFilter::filter(const GameState& state, const std::vector<Move>& moves) const {
    std::vector<Move> legal;
    for (const Move& move : moves) {
        const bool castlingIsSafe = move.kind() != MoveKind::Castling || kingPathIsSafe(state, move);
        if (castlingIsSafe && leavesOwnKingSafe(state, move)) {
            legal.push_back(move);
        }
    }
    return legal;
}

bool LegalMoveFilter::leavesOwnKingSafe(const GameState& state, const Move& move) const {
    // El color sale de la pieza que mueve, no del turno: sirve también para consultar fuera de turno.
    const Color mover = state.board().pieceAt(move.from())->color();
    return !detector_.isInCheck(applier_.apply(state, move), mover);
}

bool LegalMoveFilter::kingPathIsSafe(const GameState& state, const Move& castling) const {
    const Color enemy = opposite(state.board().pieceAt(castling.from())->color());
    const Offset step(castling.to().file() > castling.from().file() ? 1 : -1, 0);
    // Desde la casilla de origen (no enrocar en jaque) hasta la anterior al destino; el destino lo
    // cubre leavesOwnKingSafe con el estado ya aplicado.
    for (Position square = castling.from(); square != castling.to(); square = square + step) {
        if (detector_.isSquareAttacked(state, square, enemy)) {
            return false;
        }
    }
    return true;
}

}  // namespace chess::domain
