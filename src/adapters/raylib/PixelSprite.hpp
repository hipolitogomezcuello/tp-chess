#pragma once

#include <string>
#include <vector>

namespace chess::adapters::raylib {

enum class SpritePixel { Transparent, Outline, Fill, Shade };

class PixelSprite {
public:
    struct Placement {
        int x;
        int y;
        int scale;
    };

    explicit PixelSprite(std::vector<std::string> rows);

    int width() const { return static_cast<int>(rows_.front().size()); }
    int height() const { return static_cast<int>(rows_.size()); }
    SpritePixel pixelAt(int x, int y) const;

    Placement placedIn(int boxX, int boxY, int boxPixels) const;

private:
    std::vector<std::string> rows_;
};

}
