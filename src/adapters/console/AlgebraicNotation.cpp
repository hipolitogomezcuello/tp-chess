#include "adapters/console/AlgebraicNotation.hpp"

#include <cctype>

namespace chess::adapters::console {

using domain::Position;

std::optional<Position> AlgebraicNotation::parse(std::string_view text, const domain::BoardDimensions& dims) {
    if (text.size() < 2 || !std::isalpha(static_cast<unsigned char>(text[0]))) {
        return std::nullopt;
    }
    const int file = std::tolower(static_cast<unsigned char>(text[0])) - 'a';

    // El resto tienen que ser solo dígitos: "e4", "j10".
    int rankNumber = 0;
    for (const char c : text.substr(1)) {
        if (!std::isdigit(static_cast<unsigned char>(c)) || rankNumber > dims.ranks()) {
            return std::nullopt;
        }
        rankNumber = rankNumber * 10 + (c - '0');
    }

    const Position position(file, rankNumber - 1);
    if (!dims.contains(position)) {
        return std::nullopt;
    }
    return position;
}

std::string AlgebraicNotation::format(Position position) {
    return std::string(1, static_cast<char>('a' + position.file())) + std::to_string(position.rank() + 1);
}

}  // namespace chess::adapters::console
