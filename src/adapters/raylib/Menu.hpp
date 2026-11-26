#pragma once

#include <optional>
#include <string>
#include <vector>

namespace chess::adapters::raylib {

enum class MenuAction { NewGame, Quit, BackToTitle };

struct MenuEntry {
    std::string label;
    MenuAction action;
};

struct ButtonColumn {
    int centerX;
    int top;
    int buttonWidth;
    int buttonHeight;
    int gap;
};

struct MenuButton {
    std::string label;
    MenuAction action;
    int x;
    int y;
    int width;
    int height;

    bool contains(int pixelX, int pixelY) const;
};

class Menu {
public:
    Menu(const std::vector<MenuEntry>& entries, ButtonColumn column);

    const std::vector<MenuButton>& buttons() const { return buttons_; }

    const MenuButton* buttonAt(int x, int y) const;
    std::optional<MenuAction> actionAt(int x, int y) const;

private:
    std::vector<MenuButton> buttons_;
};

}
