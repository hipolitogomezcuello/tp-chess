#pragma once

#include "adapters/raylib/SpriteCatalog.hpp"

namespace chess::adapters::raylib {

class StandardSprites {
public:
    StandardSprites() = delete;

    static void registerAll(SpriteCatalog& catalog);
};

}
