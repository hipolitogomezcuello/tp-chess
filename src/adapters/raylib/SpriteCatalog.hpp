#pragma once

#include <map>

#include "adapters/raylib/PixelSprite.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::adapters::raylib {

class SpriteCatalog {
public:
    void add(const domain::PieceTypeId& type, PixelSprite sprite);

    const PixelSprite* find(const domain::PieceTypeId& type) const;

private:
    std::map<domain::PieceTypeId, PixelSprite> sprites_;
};

}
