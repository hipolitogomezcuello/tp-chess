#include "domain/Color.hpp"

namespace chess::domain {

Color opposite(Color color) {
    return color == Color::White ? Color::Black : Color::White;
}

}  // namespace chess::domain
