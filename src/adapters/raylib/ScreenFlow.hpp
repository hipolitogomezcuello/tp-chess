#pragma once

#include "adapters/raylib/Menu.hpp"

namespace chess::adapters::raylib {

enum class Screen { Title, Playing, Closed };

class ScreenFlow {
public:
    Screen current() const { return current_; }

    void choose(MenuAction action);

private:
    Screen current_ = Screen::Title;
};

}
