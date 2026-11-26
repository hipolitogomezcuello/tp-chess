#pragma once

#include <optional>

#include "adapters/raylib/PixelSprite.hpp"
#include "domain/Color.hpp"

namespace chess::adapters::raylib {

struct Rgba {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

std::optional<Rgba> paletteColor(SpritePixel pixel, domain::Color side);

}
