#pragma once

#include <vector>

#include "domain/Board.hpp"
#include "domain/BoardDimensions.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/StandardPieces.hpp"
#include "domain/StartingPositionProvider.hpp"
#include "support/InventedPieces.hpp"

namespace chess::testing {

class TenByTenStartingPosition : public domain::StartingPositionProvider {
public:
    domain::Board createInitialBoard(const domain::PieceFactory& factory) const override {
        using domain::Color;
        using domain::Position;
        using domain::StandardPieces;

        const domain::BoardDimensions dimensions(10, 10);
        const std::vector<domain::PieceTypeId> backRank{
            StandardPieces::kRook,  StandardPieces::kKnight, kArchbishop,           StandardPieces::kBishop,
            StandardPieces::kQueen, StandardPieces::kKing,   StandardPieces::kBishop, kChancellor,
            StandardPieces::kKnight, StandardPieces::kRook};
        const int blackBack = dimensions.ranks() - 1;

        domain::Board board(dimensions);
        for (int file = 0; file < dimensions.files(); ++file) {
            const auto& type = backRank[static_cast<std::size_t>(file)];
            board.place(Position(file, 0), factory.create(type, Color::White));
            board.place(Position(file, 1), factory.create(StandardPieces::kPawn, Color::White));
            board.place(Position(file, blackBack - 1), factory.create(StandardPieces::kPawn, Color::Black));
            board.place(Position(file, blackBack), factory.create(type, Color::Black));
        }
        return board;
    }
};

}
