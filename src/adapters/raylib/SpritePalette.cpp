#include "adapters/raylib/SpritePalette.hpp"

namespace chess::adapters::raylib {

namespace {

struct SidePalette {
    Rgba outline;
    Rgba fill;
    Rgba shade;
};

constexpr SidePalette kWhiteSide{{28, 22, 18, 255}, {246, 238, 220, 255}, {186, 170, 150, 255}};
constexpr SidePalette kBlackSide{{12, 10, 12, 255}, {78, 70, 84, 255}, {44, 40, 50, 255}};

}

std::optional<Rgba> paletteColor(SpritePixel pixel, domain::Color side) {
    const SidePalette& palette = side == domain::Color::White ? kWhiteSide : kBlackSide;
    switch (pixel) {
        case SpritePixel::Outline:
            return palette.outline;
        case SpritePixel::Fill:
            return palette.fill;
        case SpritePixel::Shade:
            return palette.shade;
        case SpritePixel::Transparent:
            break;
    }
    return std::nullopt;
}

}
