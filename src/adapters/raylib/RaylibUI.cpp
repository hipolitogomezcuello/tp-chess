#include "adapters/raylib/RaylibUI.hpp"

#include <raylib.h>

#include <cctype>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "adapters/raylib/BoardInteraction.hpp"
#include "adapters/raylib/BoardLayout.hpp"
#include "adapters/raylib/Menu.hpp"
#include "adapters/raylib/PixelSprite.hpp"
#include "adapters/raylib/ScreenFlow.hpp"
#include "adapters/raylib/SpritePalette.hpp"
#include "domain/Board.hpp"
#include "domain/Piece.hpp"
#include "domain/Position.hpp"

namespace chess::adapters::raylib {

namespace {

using domain::Position;

constexpr int kLabelMargin = 40;
constexpr int kBoardPixels = 640;
constexpr int kStatusHeight = 110;
constexpr int kWindowWidth = kBoardPixels + 2 * kLabelMargin;
constexpr int kWindowHeight = kLabelMargin + kBoardPixels + kStatusHeight;
constexpr int kStatusTop = kLabelMargin + kBoardPixels + kLabelMargin;

const ::Color kLightSquare{240, 217, 181, 255};
const ::Color kDarkSquare{181, 136, 99, 255};
const ::Color kLastMove{205, 210, 106, 150};
const ::Color kSelected{106, 162, 205, 170};
const ::Color kHint{40, 40, 40, 110};
const ::Color kCheck{220, 60, 60, 190};
const ::Color kWhitePiece{250, 250, 245, 255};
const ::Color kBlackPiece{45, 45, 45, 255};

const ::Color kTitleBackground{48, 36, 30, 255};
const ::Color kButtonFill{240, 217, 181, 255};
const ::Color kButtonHover{255, 240, 205, 255};
const ::Color kButtonShadow{20, 14, 10, 255};
constexpr float kButtonOutline = 4.0F;
constexpr float kButtonShadowOffset = 6.0F;

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

class SpriteTextures {
public:
    SpriteTextures() = default;
    SpriteTextures(const SpriteTextures&) = delete;
    SpriteTextures& operator=(const SpriteTextures&) = delete;

    ~SpriteTextures() {
        for (const auto& [key, texture] : textures_) {
            UnloadTexture(texture);
        }
    }

    const ::Texture2D& of(const PixelSprite& sprite, domain::Color side) {
        const auto key = std::make_pair(&sprite, side);
        auto it = textures_.find(key);
        if (it == textures_.end()) {
            it = textures_.emplace(key, bake(sprite, side)).first;
        }
        return it->second;
    }

private:
    static ::Texture2D bake(const PixelSprite& sprite, domain::Color side) {
        ::Image image = GenImageColor(sprite.width(), sprite.height(), BLANK);
        for (int y = 0; y < sprite.height(); ++y) {
            for (int x = 0; x < sprite.width(); ++x) {
                if (const auto rgba = paletteColor(sprite.pixelAt(x, y), side)) {
                    ImageDrawPixel(&image, x, y, ::Color{rgba->r, rgba->g, rgba->b, rgba->a});
                }
            }
        }
        const ::Texture2D texture = LoadTextureFromImage(image);
        UnloadImage(image);
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        return texture;
    }

    std::map<std::pair<const PixelSprite*, domain::Color>, ::Texture2D> textures_;
};

void drawPieceLetter(const domain::Piece& piece, const ::Rectangle& rect) {
    const bool white = piece.color() == domain::Color::White;
    const ::Vector2 center = centerOf(rect);
    const float radius = rect.width * 0.38F;
    DrawCircleV(center, radius, white ? kWhitePiece : kBlackPiece);
    DrawCircleLinesV(center, radius, white ? kBlackPiece : kWhitePiece);
    const std::string letter(1, static_cast<char>(std::toupper(static_cast<unsigned char>(piece.symbol()))));
    drawCenteredText(letter, center, static_cast<int>(rect.width * 0.45F), white ? kBlackPiece : kWhitePiece);
}

void drawSprite(const PixelSprite& sprite, const ::Texture2D& texture, const ::Rectangle& rect) {
    const auto placement = sprite.placedIn(static_cast<int>(rect.x), static_cast<int>(rect.y),
                                           static_cast<int>(rect.width));
    DrawTextureEx(texture, {static_cast<float>(placement.x), static_cast<float>(placement.y)}, 0.0F,
                  static_cast<float>(placement.scale), WHITE);
}

void drawPiece(const domain::Piece& piece, const ::Rectangle& rect, const SpriteCatalog& sprites,
               SpriteTextures& textures) {
    if (const PixelSprite* sprite = sprites.find(piece.type())) {
        drawSprite(*sprite, textures.of(*sprite, piece.color()), rect);
    } else {
        drawPieceLetter(piece, rect);
    }
}

std::vector<::Rectangle> promotionBoxes(std::size_t count) {
    constexpr float kBox = 90.0F;
    constexpr float kGap = 16.0F;
    const float total = static_cast<float>(count) * kBox + static_cast<float>(count - 1) * kGap;
    const float left = kLabelMargin + (kBoardPixels - total) / 2.0F;
    const float top = kLabelMargin + (kBoardPixels - kBox) / 2.0F;
    std::vector<::Rectangle> boxes;
    for (std::size_t i = 0; i < count; ++i) {
        boxes.push_back({left + static_cast<float>(i) * (kBox + kGap), top, kBox, kBox});
    }
    return boxes;
}

bool isRoyalInCheck(const domain::Piece& piece, domain::GameStatus status, domain::Color sideToMove) {
    const bool sideToMoveIsInCheck = status == domain::GameStatus::Check || status == domain::GameStatus::Checkmate;
    return sideToMoveIsInCheck && piece.traits().isRoyal && piece.color() == sideToMove;
}

bool isGameOver(domain::GameStatus status) {
    return status == domain::GameStatus::Checkmate || status == domain::GameStatus::Stalemate;
}

Menu titleMenu() {
    return Menu({{"Nueva partida", MenuAction::NewGame}, {"Salir", MenuAction::Quit}},
                ButtonColumn{kWindowWidth / 2, 420, 320, 64, 28});
}

Menu backToTitleMenu() {
    constexpr int kWidth = 220;
    return Menu({{"Volver al titulo", MenuAction::BackToTitle}},
                ButtonColumn{kWindowWidth - kLabelMargin - kWidth / 2, kStatusTop - 8, kWidth, 40, 0});
}

::Rectangle rectOf(const MenuButton& button) {
    return {static_cast<float>(button.x), static_cast<float>(button.y), static_cast<float>(button.width),
            static_cast<float>(button.height)};
}

const MenuButton* hoveredButton(const Menu& menu) {
    const ::Vector2 mouse = GetMousePosition();
    return menu.buttonAt(static_cast<int>(mouse.x), static_cast<int>(mouse.y));
}

std::optional<MenuAction> clickedAction(const Menu& menu) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return std::nullopt;
    }
    const ::Vector2 mouse = GetMousePosition();
    return menu.actionAt(static_cast<int>(mouse.x), static_cast<int>(mouse.y));
}

void drawButton(const MenuButton& button, bool hovered, int fontSize) {
    ::Rectangle face = rectOf(button);
    DrawRectangleRec({face.x + kButtonShadowOffset, face.y + kButtonShadowOffset, face.width, face.height},
                     kButtonShadow);
    if (hovered) {
        face.x += kButtonShadowOffset / 2.0F;
        face.y += kButtonShadowOffset / 2.0F;
    }
    DrawRectangleRec(face, hovered ? kButtonHover : kButtonFill);
    DrawRectangleLinesEx(face, kButtonOutline, kBlackPiece);
    drawCenteredText(button.label, centerOf(face), fontSize, kBlackPiece);
}

void drawMenu(const Menu& menu, int fontSize) {
    const MenuButton* hovered = hoveredButton(menu);
    for (const MenuButton& button : menu.buttons()) {
        drawButton(button, &button == hovered, fontSize);
    }
}

void drawCheckerStrip(int top, int squarePixels) {
    for (int column = 0; column * squarePixels < kWindowWidth; ++column) {
        DrawRectangle(column * squarePixels, top, squarePixels, squarePixels,
                      column % 2 == 0 ? kDarkSquare : kLightSquare);
    }
}

void drawTitleScreen(const Menu& menu) {
    constexpr int kStripPixels = 40;
    constexpr int kTitleFont = 96;
    ClearBackground(kTitleBackground);
    drawCheckerStrip(0, kStripPixels);
    drawCheckerStrip(kWindowHeight - kStripPixels, kStripPixels);

    const ::Vector2 titleCenter{kWindowWidth / 2.0F, 230.0F};
    drawCenteredText("AJEDREZ", {titleCenter.x + kButtonShadowOffset, titleCenter.y + kButtonShadowOffset},
                     kTitleFont, kButtonShadow);
    drawCenteredText("AJEDREZ", titleCenter, kTitleFont, kLightSquare);
    drawCenteredText("TPO Ingenieria de Software", {titleCenter.x, titleCenter.y + 80.0F}, 20, kDarkSquare);

    drawMenu(menu, 28);
}

class GameScreen {
public:
    GameScreen(std::unique_ptr<ports::GameService> game, const SpriteCatalog& sprites, SpriteTextures& textures)
        : game_(std::move(game)), interaction_(*game_), sprites_(sprites), textures_(textures) {}

    std::optional<MenuAction> update(std::string& message);
    void draw(const std::optional<domain::Move>& lastMove, const std::string& message);

private:
    BoardLayout layout() const {
        return BoardLayout(game_->state().board().dimensions(), kLabelMargin, kLabelMargin, kBoardPixels);
    }
    void handleBoardInput(std::string& message);
    void drawBoard(const std::optional<domain::Move>& lastMove);
    void drawPromotionChoices();
    void drawStatus(const std::string& message);

    std::unique_ptr<ports::GameService> game_;
    BoardInteraction interaction_;
    const SpriteCatalog& sprites_;
    SpriteTextures& textures_;
    Menu backToTitle_ = backToTitleMenu();
};

std::optional<MenuAction> GameScreen::update(std::string& message) {
    if (isGameOver(game_->status())) {
        if (const auto action = clickedAction(backToTitle_)) {
            return action;
        }
    }
    handleBoardInput(message);
    return std::nullopt;
}

void GameScreen::handleBoardInput(std::string& message) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        const ::Vector2 mouse = GetMousePosition();
        if (interaction_.awaitingPromotion()) {
            const auto choices = interaction_.promotionChoices();
            const auto boxes = promotionBoxes(choices.size());
            for (std::size_t i = 0; i < boxes.size(); ++i) {
                if (CheckCollisionPointRec(mouse, boxes[i])) {
                    interaction_.choosePromotion(choices[i].type());
                    break;
                }
            }
        } else if (const auto square = layout().squareAt(static_cast<int>(mouse.x), static_cast<int>(mouse.y))) {
            interaction_.click(*square);
        } else {
            interaction_.cancel();
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_ESCAPE)) {
        interaction_.cancel();
    }
    if (IsKeyPressed(KEY_U)) {
        message = interaction_.undo() ? "Jugada deshecha." : "No hay jugadas para deshacer.";
    }
    if (IsKeyPressed(KEY_R)) {
        message = interaction_.redo() ? "Jugada rehecha." : "No hay jugadas para rehacer.";
    }
}

void GameScreen::draw(const std::optional<domain::Move>& lastMove, const std::string& message) {
    ClearBackground(RAYWHITE);
    drawBoard(lastMove);
    if (interaction_.awaitingPromotion()) {
        drawPromotionChoices();
    }
    drawStatus(message);
}

void GameScreen::drawBoard(const std::optional<domain::Move>& lastMove) {
    const BoardLayout boardLayout = layout();
    const domain::GameState& current = game_->state();
    const domain::Board& currentBoard = current.board();
    const domain::GameStatus status = game_->status();
    const auto& dims = currentBoard.dimensions();

    for (int file = 0; file < dims.files(); ++file) {
        for (int rank = 0; rank < dims.ranks(); ++rank) {
            const Position position(file, rank);
            const ::Rectangle rect = squareRect(boardLayout, position);
            DrawRectangleRec(rect, (file + rank) % 2 == 0 ? kDarkSquare : kLightSquare);
            if (lastMove && (lastMove->from() == position || lastMove->to() == position)) {
                DrawRectangleRec(rect, kLastMove);
            }
            if (interaction_.selected() == position) {
                DrawRectangleRec(rect, kSelected);
            }
            const auto piece = currentBoard.pieceAt(position);
            if (piece && isRoyalInCheck(*piece, status, current.sideToMove())) {
                DrawRectangleRec(rect, kCheck);
            }
            if (piece) {
                drawPiece(*piece, rect, sprites_, textures_);
            }
        }
    }

    for (const domain::Move& move : interaction_.highlightedMoves()) {
        const ::Rectangle rect = squareRect(boardLayout, move.to());
        if (currentBoard.isEmpty(move.to())) {
            DrawCircleV(centerOf(rect), rect.width * 0.15F, kHint);
        } else {
            DrawRing(centerOf(rect), rect.width * 0.40F, rect.width * 0.48F, 0.0F, 360.0F, 32, kHint);
        }
    }

    for (int file = 0; file < dims.files(); ++file) {
        const ::Rectangle rect = squareRect(boardLayout, Position(file, 0));
        drawCenteredText(std::string(1, static_cast<char>('a' + file)),
                         {rect.x + rect.width / 2.0F, rect.y + rect.height + kLabelMargin / 2.0F}, 20, DARKGRAY);
    }
    for (int rank = 0; rank < dims.ranks(); ++rank) {
        const ::Rectangle rect = squareRect(boardLayout, Position(0, rank));
        drawCenteredText(std::to_string(rank + 1), {kLabelMargin / 2.0F, rect.y + rect.height / 2.0F}, 20, DARKGRAY);
    }
}

void GameScreen::drawPromotionChoices() {
    DrawRectangle(kLabelMargin, kLabelMargin, kBoardPixels, kBoardPixels, ::Color{0, 0, 0, 140});
    const auto choices = interaction_.promotionChoices();
    const auto boxes = promotionBoxes(choices.size());
    for (std::size_t i = 0; i < boxes.size(); ++i) {
        DrawRectangleRec(boxes[i], kLightSquare);
        drawPiece(choices[i], boxes[i], sprites_, textures_);
    }
    drawCenteredText("Elegi la pieza de la promocion (Esc cancela)",
                     {kLabelMargin + kBoardPixels / 2.0F, boxes.front().y - 30.0F}, 22, RAYWHITE);
}

void GameScreen::drawStatus(const std::string& message) {
    const bool over = isGameOver(game_->status());
    if (!over) {
        DrawText((std::string("Juegan las ") + colorName(game_->state().sideToMove()) + ".").c_str(), kLabelMargin,
                 kStatusTop, 24, BLACK);
    }
    const int messageLeft = over ? kLabelMargin : kLabelMargin + 260;
    DrawText(message.c_str(), messageLeft, kStatusTop, 24, MAROON);
    DrawText("Click: elegir y mover   U: deshacer   R: rehacer   Esc / click derecho: cancelar", kLabelMargin,
             kStatusTop + 40, 16, GRAY);
    if (over) {
        drawMenu(backToTitle_, 20);
    }
}

}

RaylibUI::RaylibUI(const SpriteCatalog& sprites, GameFactory newGame)
    : sprites_(sprites), newGame_(std::move(newGame)) {}

std::unique_ptr<ports::GameService> RaylibUI::startGame() {
    auto game = newGame_();
    game->addObserver(*this);
    lastMove_.reset();
    message_.clear();
    return game;
}

void RaylibUI::run() {
    InitWindow(kWindowWidth, kWindowHeight, "TPO Ajedrez");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    {
        SpriteTextures textures;
        ScreenFlow flow;
        const Menu title = titleMenu();
        std::unique_ptr<GameScreen> game;
        while (!WindowShouldClose() && flow.current() != Screen::Closed) {
            if (flow.current() == Screen::Title) {
                if (const auto action = clickedAction(title)) {
                    flow.choose(*action);
                    if (flow.current() == Screen::Playing) {
                        game = std::make_unique<GameScreen>(startGame(), sprites_, textures);
                    }
                }
            } else if (const auto action = game->update(message_)) {
                flow.choose(*action);
                game.reset();
            }

            BeginDrawing();
            if (flow.current() == Screen::Playing) {
                game->draw(lastMove_, message_);
            } else {
                drawTitleScreen(title);
            }
            EndDrawing();
        }
    }
    CloseWindow();
}

void RaylibUI::onMoveMade(const domain::Move& move, const domain::GameState&) {
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

void RaylibUI::onUndoRedo(const domain::GameState&) {
    lastMove_.reset();
    message_.clear();
}

}
