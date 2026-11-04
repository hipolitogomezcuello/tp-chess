#include "domain/CastlingRule.hpp"

#include <cstdlib>

#include "domain/Move.hpp"
#include "domain/MoveContext.hpp"
#include "domain/Offset.hpp"

namespace chess::domain {

namespace {

// Para que el rey se mueva dos casillas sin pisar a su compañera, tiene que haber al menos dos entre ellos.
constexpr int kMinimumDistance = 3;

}  // namespace

std::vector<Move> CastlingRule::candidateMoves(const MoveContext& context) const {
    const Board& board = context.board();
    const Piece& king = context.piece();
    std::vector<Move> result;
    if (king.hasMoved()) {
        return result;
    }

    for (const Offset side : {Offset(1, 0), Offset(-1, 0)}) {
        // Avanza por las casillas vacías hasta la primera pieza (o el borde).
        Position square = context.from() + side;
        while (board.dimensions().contains(square) && board.isEmpty(square)) {
            square = square + side;
        }
        if (!board.dimensions().contains(square)) {
            continue;
        }

        const Piece partner = *board.pieceAt(square);
        const bool canCastle = partner.color() == king.color() && partner.traits().canCastleWith
                            && !partner.hasMoved()
                            && std::abs(square.file() - context.from().file()) >= kMinimumDistance;
        if (canCastle) {
            const Position kingTo = context.from() + side + side;
            const Position partnerTo = context.from() + side;
            result.push_back(Move::castling(context.from(), kingTo, square, partnerTo));
        }
    }
    return result;
}

std::vector<Position> CastlingRule::attackedSquares(const MoveContext& /*context*/) const {
    return {};
}

}  // namespace chess::domain
