#include "adapters/raylib/Menu.hpp"

namespace chess::adapters::raylib {

bool MenuButton::contains(int pixelX, int pixelY) const {
    return pixelX >= x && pixelX < x + width && pixelY >= y && pixelY < y + height;
}

Menu::Menu(const std::vector<MenuEntry>& entries, ButtonColumn column) {
    const int left = column.centerX - column.buttonWidth / 2;
    int top = column.top;
    for (const MenuEntry& entry : entries) {
        buttons_.push_back({entry.label, entry.action, left, top, column.buttonWidth, column.buttonHeight});
        top += column.buttonHeight + column.gap;
    }
}

const MenuButton* Menu::buttonAt(int x, int y) const {
    for (const MenuButton& button : buttons_) {
        if (button.contains(x, y)) {
            return &button;
        }
    }
    return nullptr;
}

std::optional<MenuAction> Menu::actionAt(int x, int y) const {
    if (const MenuButton* button = buttonAt(x, y)) {
        return button->action;
    }
    return std::nullopt;
}

}
