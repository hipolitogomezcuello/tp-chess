#pragma once

namespace chess::domain {

struct PieceTraits {
    bool isRoyal = false;
    bool canCastleWith = false;
    bool isPromotable = false;
};

}
