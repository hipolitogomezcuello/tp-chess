#include "adapters/raylib/SpriteCatalog.hpp"

#include <stdexcept>
#include <utility>

namespace chess::adapters::raylib {

void SpriteCatalog::add(const domain::PieceTypeId& type, PixelSprite sprite) {
    if (!sprites_.emplace(type, std::move(sprite)).second) {
        throw std::invalid_argument("SpriteCatalog: el tipo ya tiene sprite: " + type.value());
    }
}

const PixelSprite* SpriteCatalog::find(const domain::PieceTypeId& type) const {
    const auto it = sprites_.find(type);
    return it == sprites_.end() ? nullptr : &it->second;
}

}
