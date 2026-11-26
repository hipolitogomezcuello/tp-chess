#pragma once

#include "domain/StartingPositionProvider.hpp"

namespace chess::domain {

class StandardStartingPosition : public StartingPositionProvider {
public:
    Board createInitialBoard(const PieceFactory& factory) const override;
};

}
