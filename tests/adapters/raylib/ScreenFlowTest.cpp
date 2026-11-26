#include "adapters/raylib/ScreenFlow.hpp"

#include <gtest/gtest.h>

using chess::adapters::raylib::MenuAction;
using chess::adapters::raylib::Screen;
using chess::adapters::raylib::ScreenFlow;

TEST(ScreenFlowTest, StartsOnTheTitleScreen) {
    EXPECT_EQ(ScreenFlow().current(), Screen::Title);
}

TEST(ScreenFlowTest, NewGameFromTheTitleStartsPlaying) {
    ScreenFlow flow;

    flow.choose(MenuAction::NewGame);

    EXPECT_EQ(flow.current(), Screen::Playing);
}

TEST(ScreenFlowTest, QuitFromTheTitleClosesTheWindow) {
    ScreenFlow flow;

    flow.choose(MenuAction::Quit);

    EXPECT_EQ(flow.current(), Screen::Closed);
}

TEST(ScreenFlowTest, BackToTitleFromAGameReturnsToTheTitle) {
    ScreenFlow flow;
    flow.choose(MenuAction::NewGame);

    flow.choose(MenuAction::BackToTitle);

    EXPECT_EQ(flow.current(), Screen::Title);
}

TEST(ScreenFlowTest, AnActionThatDoesNotBelongToTheCurrentScreenIsIgnored) {
    ScreenFlow flow;
    flow.choose(MenuAction::BackToTitle);
    EXPECT_EQ(flow.current(), Screen::Title);

    flow.choose(MenuAction::NewGame);
    flow.choose(MenuAction::NewGame);
    flow.choose(MenuAction::Quit);
    EXPECT_EQ(flow.current(), Screen::Playing);
}

TEST(ScreenFlowTest, AClosedFlowStaysClosed) {
    ScreenFlow flow;
    flow.choose(MenuAction::Quit);

    flow.choose(MenuAction::NewGame);

    EXPECT_EQ(flow.current(), Screen::Closed);
}
