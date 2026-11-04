#pragma once

// Helper de tests: arma posiciones a mano en lugar de jugar partidas enteras
// (docs/arquitectura.md §8).
//
//   const Board board = BoardBuilder(factory)
//       .place("e1", kKing, Color::White)
//       .place("e8", kKing, Color::Black)
//       .build();
#include <string>

#include "domain/Board.hpp"
#include "domain/BoardDimensions.hpp"
#include "domain/Color.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceTypeId.hpp"
#include "support/Squares.hpp"

namespace chess::testing {

class BoardBuilder {
public:
    explicit BoardBuilder(const domain::PieceFactory& factory,
                          domain::BoardDimensions dimensions = domain::BoardDimensions())
        : factory_(factory), board_(dimensions) {}

    BoardBuilder& place(const std::string& square, const domain::PieceTypeId& type, domain::Color color) {
        board_.place(sq(square), factory_.create(type, color));
        return *this;
    }

    // Pieza que ya se movió (sin doble avance de peón ni derecho de enroque).
    BoardBuilder& placeMoved(const std::string& square, const domain::PieceTypeId& type,
                             domain::Color color) {
        board_.place(sq(square), factory_.create(type, color).withMoved());
        return *this;
    }

    domain::Board build() const { return board_; }

private:
    const domain::PieceFactory& factory_;
    domain::Board board_;
};

}  // namespace chess::testing
