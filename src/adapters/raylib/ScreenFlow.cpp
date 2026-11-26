#include "adapters/raylib/ScreenFlow.hpp"

namespace chess::adapters::raylib {

void ScreenFlow::choose(MenuAction action) {
    if (current_ == Screen::Title && action == MenuAction::NewGame) {
        current_ = Screen::Playing;
    } else if (current_ == Screen::Title && action == MenuAction::Quit) {
        current_ = Screen::Closed;
    } else if (current_ == Screen::Playing && action == MenuAction::BackToTitle) {
        current_ = Screen::Title;
    }
}

}
