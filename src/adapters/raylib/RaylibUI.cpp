#include "adapters/raylib/RaylibUI.hpp"

#include <raylib.h>

namespace chess::adapters::raylib {

void RaylibUI::run() {
    InitWindow(640, 480, "TPO Ajedrez");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("TPO Ajedrez", 20, 20, 30, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
}

}  // namespace chess::adapters::raylib
