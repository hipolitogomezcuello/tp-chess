#pragma once

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

}
