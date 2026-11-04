#include "adapters/raylib/RaylibUI.hpp"

#include <raylib.h>

#include <cctype>
#include <string>
#include <vector>

#include "adapters/raylib/BoardInteraction.hpp"
#include "adapters/raylib/BoardLayout.hpp"
#include "domain/Board.hpp"
#include "domain/Piece.hpp"
#include "domain/Position.hpp"

// raylib define un ::Color (RGBA) global; el color de las piezas se escribe siempre domain::Color.
// La fuente por defecto de raylib solo trae ASCII: los textos de la ventana van sin tildes.
namespace chess::adapters::raylib {

namespace {

using domain::Position;

constexpr int kMargin = 40;        // espacio para las letras y números de las coordenadas
constexpr int kBoardPixels = 640;  // lado del tablero, sea de 8×8 o de 10×10
constexpr int kStatusHeight = 110;
constexpr int kWindowWidth = kBoardPixels + 2 * kMargin;
constexpr int kWindowHeight = kMargin + kBoardPixels + kStatusHeight;

const ::Color kLightSquare{240, 217, 181, 255};
const ::Color kDarkSquare{181, 136, 99, 255};
const ::Color kLastMove{205, 210, 106, 150};
const ::Color kSelected{106, 162, 205, 170};
const ::Color kHint{40, 40, 40, 110};
const ::Color kCheck{220, 60, 60, 190};
const ::Color kWhitePiece{250, 250, 245, 255};
const ::Color kBlackPiece{45, 45, 45, 255};

const char* colorName(domain::Color color) {
    return color == domain::Color::White ? "blancas" : "negras";
}

::Rectangle squareRect(const BoardLayout& layout, Position position) {
    const auto corner = layout.topLeftOf(position);
    const auto size = static_cast<float>(layout.squareSize());
    return {static_cast<float>(corner.x), static_cast<float>(corner.y), size, size};
}

::Vector2 centerOf(const ::Rectangle& rect) {
    return {rect.x + rect.width / 2.0F, rect.y + rect.height / 2.0F};
}

void drawCenteredText(const std::string& text, ::Vector2 center, int fontSize, ::Color color) {
    const int width = MeasureText(text.c_str(), fontSize);
    DrawText(text.c_str(), static_cast<int>(center.x) - width / 2, static_cast<int>(center.y) - fontSize / 2,
             fontSize, color);
}

// Un círculo con la letra de la pieza: blanco con letra oscura, o al revés. Sin mirar el tipo.
void drawPiece(const domain::Piece& piece, const ::Rectangle& rect) {
    const bool white = piece.color() == domain::Color::White;
    const ::Vector2 center = centerOf(rect);
    const float radius = rect.width * 0.38F;
    DrawCircleV(center, radius, white ? kWhitePiece : kBlackPiece);
    DrawCircleLinesV(center, radius, white ? kBlackPiece : kWhitePiece);
    const std::string letter(1, static_cast<char>(std::toupper(static_cast<unsigned char>(piece.symbol()))));
    drawCenteredText(letter, center, static_cast<int>(rect.width * 0.45F), white ? kBlackPiece : kWhitePiece);
}

// Las casillas donde se dibujan las opciones de promoción, en fila en el centro del tablero.
std::vector<::Rectangle> promotionBoxes(std::size_t count) {
    constexpr float kBox = 90.0F;
    constexpr float kGap = 16.0F;
    const float total = static_cast<float>(count) * kBox + static_cast<float>(count - 1) * kGap;
    const float left = kMargin + (kBoardPixels - total) / 2.0F;
    const float top = kMargin + (kBoardPixels - kBox) / 2.0F;
    std::vector<::Rectangle> boxes;
    for (std::size_t i = 0; i < count; ++i) {
        boxes.push_back({left + static_cast<float>(i) * (kBox + kGap), top, kBox, kBox});
    }
    return boxes;
}

bool isGameOver(domain::GameStatus status) {
    return status == domain::GameStatus::Checkmate || status == domain::GameStatus::Stalemate;
}

}  // namespace

void RaylibUI::run(ports::GameService& game) {
    BoardInteraction interaction(game);

    InitWindow(kWindowWidth, kWindowHeight, "TPO Ajedrez");
    SetExitKey(KEY_NULL);  // Esc cancela la selección; la ventana se cierra con la X
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        const BoardLayout layout(game.state().board().dimensions(), kMargin, kMargin, kBoardPixels);

        // ---- Entrada: mouse y teclado → BoardInteraction → GameService
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const ::Vector2 mouse = GetMousePosition();
            if (interaction.awaitingPromotion()) {
                const auto choices = interaction.promotionChoices();
                const auto boxes = promotionBoxes(choices.size());
                for (std::size_t i = 0; i < boxes.size(); ++i) {
                    if (CheckCollisionPointRec(mouse, boxes[i])) {
                        interaction.choosePromotion(choices[i].type());
                        break;
                    }
                }
            } else if (const auto square = layout.squareAt(static_cast<int>(mouse.x), static_cast<int>(mouse.y))) {
                interaction.click(*square);
            } else {
                interaction.cancel();
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_ESCAPE)) {
            interaction.cancel();
        }
        if (IsKeyPressed(KEY_U)) {
            // Si se pudo, onUndoRedo ya actualizó el estado de la UI; el mensaje se pone después.
            message_ = interaction.undo() ? "Jugada deshecha." : "No hay jugadas para deshacer.";
        }
        if (IsKeyPressed(KEY_R)) {
            message_ = interaction.redo() ? "Jugada rehecha." : "No hay jugadas para rehacer.";
        }

        // ---- Dibujo (se lee el estado otra vez: la entrada puede haberlo cambiado)
        const domain::GameState& current = game.state();
        const domain::Board& currentBoard = current.board();
        const domain::GameStatus status = game.status();
        const auto& dims = currentBoard.dimensions();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int file = 0; file < dims.files(); ++file) {
            for (int rank = 0; rank < dims.ranks(); ++rank) {
                const Position position(file, rank);
                const ::Rectangle rect = squareRect(layout, position);
                DrawRectangleRec(rect, (file + rank) % 2 == 0 ? kDarkSquare : kLightSquare);
                if (lastMove_ && (lastMove_->from() == position || lastMove_->to() == position)) {
                    DrawRectangleRec(rect, kLastMove);
                }
                if (interaction.selected() == position) {
                    DrawRectangleRec(rect, kSelected);
                }
                const auto piece = currentBoard.pieceAt(position);
                // El rey en jaque se marca por el trait isRoyal, no por ser "el rey".
                const bool inCheck = (status == domain::GameStatus::Check || status == domain::GameStatus::Checkmate)
                                  && piece && piece->traits().isRoyal && piece->color() == current.sideToMove();
                if (inCheck) {
                    DrawRectangleRec(rect, kCheck);
                }
                if (piece) {
                    drawPiece(*piece, rect);
                }
            }
        }

        // Jugadas posibles de la pieza elegida: un punto si la casilla está vacía, un anillo si captura.
        for (const domain::Move& move : interaction.highlightedMoves()) {
            const ::Rectangle rect = squareRect(layout, move.to());
            if (currentBoard.isEmpty(move.to())) {
                DrawCircleV(centerOf(rect), rect.width * 0.15F, kHint);
            } else {
                DrawRing(centerOf(rect), rect.width * 0.40F, rect.width * 0.48F, 0.0F, 360.0F, 32, kHint);
            }
        }

        // Coordenadas: letras abajo, números a la izquierda.
        for (int file = 0; file < dims.files(); ++file) {
            const ::Rectangle rect = squareRect(layout, Position(file, 0));
            drawCenteredText(std::string(1, static_cast<char>('a' + file)),
                             {rect.x + rect.width / 2.0F, rect.y + rect.height + kMargin / 2.0F}, 20, DARKGRAY);
        }
        for (int rank = 0; rank < dims.ranks(); ++rank) {
            const ::Rectangle rect = squareRect(layout, Position(0, rank));
            drawCenteredText(std::to_string(rank + 1), {kMargin / 2.0F, rect.y + rect.height / 2.0F}, 20, DARKGRAY);
        }

        // Elección de la pieza de promoción: las opciones las da el juego (D-19), se dibujan con su símbolo.
        if (interaction.awaitingPromotion()) {
            DrawRectangle(kMargin, kMargin, kBoardPixels, kBoardPixels, ::Color{0, 0, 0, 140});
            const auto choices = interaction.promotionChoices();
            const auto boxes = promotionBoxes(choices.size());
            for (std::size_t i = 0; i < boxes.size(); ++i) {
                DrawRectangleRec(boxes[i], kLightSquare);
                drawPiece(choices[i], boxes[i]);
            }
            drawCenteredText("Elegi la pieza de la promocion (Esc cancela)",
                             {kMargin + kBoardPixels / 2.0F, boxes.front().y - 30.0F}, 22, RAYWHITE);
        }

        // Barra de estado.
        const int statusTop = kMargin + kBoardPixels + kMargin;
        if (!isGameOver(status)) {
            DrawText((std::string("Juegan las ") + colorName(current.sideToMove()) + ".").c_str(), kMargin,
                     statusTop, 24, BLACK);
        }
        const int messageLeft = isGameOver(status) ? kMargin : kMargin + 260;
        DrawText(message_.c_str(), messageLeft, statusTop, 24, MAROON);
        DrawText("Click: elegir y mover   U: deshacer   R: rehacer   Esc / click derecho: cancelar", kMargin,
                 statusTop + 40, 16, GRAY);

        EndDrawing();
    }
    CloseWindow();
}

void RaylibUI::onMoveMade(const domain::Move& move, const domain::GameState& /*state*/) {
    lastMove_ = move;
    message_.clear();
}

void RaylibUI::onCheck(domain::Color colorInCheck) {
    message_ = std::string("Jaque a las ") + colorName(colorInCheck) + "!";
}

void RaylibUI::onGameOver(domain::GameStatus result, domain::Color sideToMove) {
    if (result == domain::GameStatus::Checkmate) {
        message_ = std::string("Jaque mate! Ganan las ") + colorName(opposite(sideToMove)) + ".";
    } else {
        message_ = "Ahogado: tablas.";
    }
}

void RaylibUI::onUndoRedo(const domain::GameState& /*state*/) {
    lastMove_.reset();
    message_.clear();
}

}  // namespace chess::adapters::raylib
