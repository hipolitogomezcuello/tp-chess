#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "domain/BoardDimensions.hpp"
#include "domain/Position.hpp"

namespace chess::adapters::console {

class AlgebraicNotation {
public:
    AlgebraicNotation() = delete;

    static std::optional<domain::Position> parse(std::string_view text, const domain::BoardDimensions& dims);

    static std::string format(domain::Position position);
};

}
