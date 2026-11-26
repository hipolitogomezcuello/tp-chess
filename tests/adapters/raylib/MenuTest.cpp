#include "adapters/raylib/Menu.hpp"

#include <gtest/gtest.h>

using chess::adapters::raylib::ButtonColumn;
using chess::adapters::raylib::Menu;
using chess::adapters::raylib::MenuAction;

namespace {

const Menu kMenu({{"Nueva partida", MenuAction::NewGame}, {"Salir", MenuAction::Quit}},
                 ButtonColumn{200, 100, 160, 40, 20});

}

TEST(MenuTest, ButtonsAreStackedInOrderAndCenteredOnTheColumn) {
    const auto& buttons = kMenu.buttons();

    ASSERT_EQ(buttons.size(), 2U);
    EXPECT_EQ(buttons[0].label, "Nueva partida");
    EXPECT_EQ(buttons[0].action, MenuAction::NewGame);
    EXPECT_EQ(buttons[0].x, 120);
    EXPECT_EQ(buttons[0].y, 100);
    EXPECT_EQ(buttons[0].width, 160);
    EXPECT_EQ(buttons[0].height, 40);
    EXPECT_EQ(buttons[1].label, "Salir");
    EXPECT_EQ(buttons[1].x, 120);
    EXPECT_EQ(buttons[1].y, 160);
}

TEST(MenuTest, APixelInsideAButtonTriggersItsAction) {
    EXPECT_EQ(kMenu.actionAt(120, 100), MenuAction::NewGame);
    EXPECT_EQ(kMenu.actionAt(279, 139), MenuAction::NewGame);
    EXPECT_EQ(kMenu.actionAt(200, 180), MenuAction::Quit);
}

TEST(MenuTest, PixelsOutsideEveryButtonTriggerNothing) {
    EXPECT_FALSE(kMenu.actionAt(119, 110).has_value());
    EXPECT_FALSE(kMenu.actionAt(280, 110).has_value());
    EXPECT_FALSE(kMenu.actionAt(200, 99).has_value());
    EXPECT_FALSE(kMenu.actionAt(200, 140).has_value());
    EXPECT_FALSE(kMenu.actionAt(200, 200).has_value());
}

TEST(MenuTest, TheHoveredButtonIsTheOneUnderTheMouse) {
    EXPECT_EQ(kMenu.buttonAt(200, 110), &kMenu.buttons()[0]);
    EXPECT_EQ(kMenu.buttonAt(200, 170), &kMenu.buttons()[1]);
    EXPECT_EQ(kMenu.buttonAt(0, 0), nullptr);
}

TEST(MenuTest, AMenuCanHaveASingleButton) {
    const Menu menu({{"Volver al titulo", MenuAction::BackToTitle}}, ButtonColumn{50, 0, 100, 30, 10});

    EXPECT_EQ(menu.actionAt(50, 15), MenuAction::BackToTitle);
}
