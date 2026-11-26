#include "adapters/raylib/PixelSprite.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace chess::adapters::raylib {

namespace {

constexpr char kTransparent = '.';
constexpr char kOutline = 'o';
constexpr char kFill = 'f';
constexpr char kShade = 's';
constexpr int kMarginInSpritePixels = 1;

bool isPixelCode(char code) {
    return code == kTransparent || code == kOutline || code == kFill || code == kShade;
}

}

PixelSprite::PixelSprite(std::vector<std::string> rows) : rows_(std::move(rows)) {
    if (rows_.empty() || rows_.front().empty()) {
        throw std::invalid_argument("PixelSprite: el sprite no puede estar vacío");
    }
    for (const std::string& row : rows_) {
        if (row.size() != rows_.front().size()) {
            throw std::invalid_argument("PixelSprite: todas las filas tienen que tener el mismo ancho");
        }
        if (!std::all_of(row.begin(), row.end(), isPixelCode)) {
            throw std::invalid_argument("PixelSprite: código de píxel desconocido en \"" + row + "\"");
        }
    }
}

SpritePixel PixelSprite::pixelAt(int x, int y) const {
    switch (rows_.at(static_cast<std::size_t>(y)).at(static_cast<std::size_t>(x))) {
        case kOutline:
            return SpritePixel::Outline;
        case kFill:
            return SpritePixel::Fill;
        case kShade:
            return SpritePixel::Shade;
        default:
            return SpritePixel::Transparent;
    }
}

PixelSprite::Placement PixelSprite::placedIn(int boxX, int boxY, int boxPixels) const {
    const int largestSide = std::max(width(), height()) + 2 * kMarginInSpritePixels;
    const int scale = std::max(1, boxPixels / largestSide);
    return {boxX + (boxPixels - width() * scale) / 2, boxY + (boxPixels - height() * scale) / 2, scale};
}

}
