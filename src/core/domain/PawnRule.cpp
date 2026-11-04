#include "domain/PawnRule.hpp"

#include "domain/Move.hpp"
#include "domain/MoveContext.hpp"
#include "domain/Offset.hpp"

namespace chess::domain {

namespace {

int forwardOf(Color color) {
    return color == Color::White ? 1 : -1;
}

// Un peón promueve al llegar a la fila desde la que ya no puede avanzar más.
MoveKind kindFor(const Board& board, Position target, Offset step, MoveKind usual) {
    return board.dimensions().contains(target + step) ? usual : MoveKind::Promotion;
}

}  // namespace

std::vector<Position> PawnRule::attackedSquares(const MoveContext& context) const {
    const int forward = forwardOf(context.piece().color());
    std::vector<Position> result;
    for (const Offset diagonal : {Offset(-1, forward), Offset(1, forward)}) {
        const Position target = context.from() + diagonal;
        if (context.board().dimensions().contains(target)) {
            result.push_back(target);
        }
    }
    return result;
}

std::vector<Move> PawnRule::candidateMoves(const MoveContext& context) const {
    const Board& board = context.board();
    const Piece& pawn = context.piece();
    const Offset step(0, forwardOf(pawn.color()));
    std::vector<Move> result;

    // Avance: solo a casillas vacías.
    const Position oneAhead = context.from() + step;
    if (board.dimensions().contains(oneAhead) && board.isEmpty(oneAhead)) {
        result.emplace_back(context.from(), oneAhead, kindFor(board, oneAhead, step, MoveKind::Normal));

        const Position twoAhead = oneAhead + step;
        if (!pawn.hasMoved() && board.dimensions().contains(twoAhead) && board.isEmpty(twoAhead)) {
            result.emplace_back(context.from(), twoAhead, MoveKind::DoublePawnPush);
        }
    }

    // Captura: solo en diagonal hacia adelante y si hay una pieza enemiga.
    for (const Position target : attackedSquares(context)) {
        const auto victim = board.pieceAt(target);
        if (victim && victim->color() != pawn.color()) {
            result.emplace_back(context.from(), target, kindFor(board, target, step, MoveKind::Capture));
        } else if (!victim && context.state().enPassantTarget() == target) {
            // El peón que avanzó dos está al lado, en la fila del que captura.
            const Position passed(target.file(), context.from().rank());
            const auto passedPawn = board.pieceAt(passed);
            if (passedPawn && passedPawn->color() != pawn.color()) {
                result.push_back(Move::enPassant(context.from(), target, passed));
            }
        }
    }
    return result;
}

}  // namespace chess::domain
