#include "domain/StandardStartingPosition.hpp"

#include <vector>

#include "domain/StandardPieces.hpp"

namespace chess::domain {

Board StandardStartingPosition::createInitialBoard(const PieceFactory& factory) const {
    const BoardDimensions dimensions;  // 8×8
    const std::vector<PieceTypeId> backRank{StandardPieces::kRook,  StandardPieces::kKnight, StandardPieces::kBishop,
                                            StandardPieces::kQueen, StandardPieces::kKing,   StandardPieces::kBishop,
                                            StandardPieces::kKnight, StandardPieces::kRook};
    const int whiteBack = 0;
    const int blackBack = dimensions.ranks() - 1;

    Board board(dimensions);
    for (int file = 0; file < dimensions.files(); ++file) {
        const auto& type = backRank[static_cast<std::size_t>(file)];
        board.place(Position(file, whiteBack), factory.create(type, Color::White));
        board.place(Position(file, whiteBack + 1), factory.create(StandardPieces::kPawn, Color::White));
        board.place(Position(file, blackBack - 1), factory.create(StandardPieces::kPawn, Color::Black));
        board.place(Position(file, blackBack), factory.create(type, Color::Black));
    }
    return board;
}

}  // namespace chess::domain
