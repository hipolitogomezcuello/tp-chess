#include "domain/Move.hpp"

#include <utility>

namespace chess::domain {

Move Move::withPromotion(PieceTypeId type) const {
    Move copy = *this;
    copy.promotion_ = std::move(type);
    return copy;
}

}  // namespace chess::domain
