#pragma once

#include "domain/Board.hpp"
#include "domain/PieceFactory.hpp"

namespace chess::domain {

class StartingPositionProvider {
public:
    virtual ~StartingPositionProvider() = default;

    virtual Board createInitialBoard(const PieceFactory& factory) const = 0;
};

}
