#include "domain/LegalMoveFilter.hpp"

namespace chess::domain {

std::vector<Move> LegalMoveFilter::filter(const GameState& state, const std::vector<Move>& moves) const {
    std::vector<Move> legal;
    for (const Move& move : moves) {
        // El color sale de la pieza que mueve, no del turno: sirve también para consultar fuera de turno.
        const Color mover = state.board().pieceAt(move.from())->color();
        const GameState after = applier_.apply(state, move);
        if (!detector_.isInCheck(after, mover)) {
            legal.push_back(move);
        }
    }
    return legal;
}

}  // namespace chess::domain
