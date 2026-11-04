#include "adapters/console/ConsoleRenderer.hpp"

#include <cctype>
#include <iomanip>
#include <string>

#include "adapters/console/AlgebraicNotation.hpp"

namespace chess::adapters::console {

using domain::Color;
using domain::Position;

namespace {

const char* colorName(Color color) {
    return color == Color::White ? "blancas" : "negras";
}

char squareSymbol(const domain::Board& board, Position position) {
    const auto piece = board.pieceAt(position);
    if (!piece) {
        return '.';
    }
    const auto symbol = static_cast<unsigned char>(piece->symbol());
    return static_cast<char>(piece->color() == Color::White ? std::toupper(symbol) : std::tolower(symbol));
}

}  // namespace

void ConsoleRenderer::render(const domain::GameState& state) {
    const domain::Board& board = state.board();
    const int files = board.dimensions().files();
    const int ranks = board.dimensions().ranks();
    // Ancho de la columna de números de fila: 1 para 8×8, 2 para 10×10.
    const int labelWidth = static_cast<int>(std::to_string(ranks).size());

    out_ << '\n';
    for (int rank = ranks - 1; rank >= 0; --rank) {  // las negras arriba
        out_ << std::setw(labelWidth) << rank + 1;
        for (int file = 0; file < files; ++file) {
            out_ << ' ' << squareSymbol(board, Position(file, rank));
        }
        out_ << '\n';
    }
    out_ << std::string(static_cast<std::size_t>(labelWidth), ' ');
    for (int file = 0; file < files; ++file) {
        out_ << ' ' << static_cast<char>('a' + file);
    }
    out_ << "\n\nJuegan las " << colorName(state.sideToMove()) << ".\n";
}

void ConsoleRenderer::onMoveMade(const domain::Move& move, const domain::GameState& state) {
    const bool capture = move.kind() == domain::MoveKind::Capture || move.kind() == domain::MoveKind::EnPassant;
    const char separator = capture ? 'x' : '-';
    out_ << "\nJugada: " << AlgebraicNotation::format(move.from()) << separator
         << AlgebraicNotation::format(move.to());
    if (move.promotion()) {
        // La pieza nueva ya está en el destino: se muestra su símbolo, sin mirar el tipo.
        out_ << '=' << static_cast<char>(std::toupper(static_cast<unsigned char>(
                           state.board().pieceAt(move.to())->symbol())));
    }
    if (move.kind() == domain::MoveKind::Castling) {
        out_ << " (enroque)";
    }
    if (move.kind() == domain::MoveKind::EnPassant) {
        out_ << " (al paso)";
    }
    out_ << '\n';
    render(state);
}

void ConsoleRenderer::onCheck(Color colorInCheck) {
    out_ << "¡Jaque a las " << colorName(colorInCheck) << "!\n";
}

void ConsoleRenderer::onGameOver(domain::GameStatus result, Color sideToMove) {
    if (result == domain::GameStatus::Checkmate) {
        out_ << "¡Jaque mate! Ganan las " << colorName(opposite(sideToMove)) << ".\n";
    } else {
        out_ << "Ahogado: la partida termina en tablas.\n";
    }
}

void ConsoleRenderer::onUndoRedo(const domain::GameState& state) {
    render(state);
}

}  // namespace chess::adapters::console
