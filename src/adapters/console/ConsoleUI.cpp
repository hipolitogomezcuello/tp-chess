#include "adapters/console/ConsoleUI.hpp"

#include <algorithm>
#include <cctype>
#include <optional>
#include <sstream>

#include "adapters/console/AlgebraicNotation.hpp"
#include "domain/Color.hpp"
#include "domain/Position.hpp"
#include "ports/MoveError.hpp"

namespace chess::adapters::console {

using domain::Position;
using ports::MoveError;

namespace {

std::vector<std::string> splitWords(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> words;
    for (std::string word; stream >> word;) {
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        words.push_back(word);
    }
    return words;
}

const char* colorName(domain::Color color) {
    return color == domain::Color::White ? "blancas" : "negras";
}

}  // namespace

void ConsoleUI::run() {
    out_ << "Escribí una jugada como \"e2 e4\", o \"help\" para ver los comandos.\n";
    for (std::string line; out_ << "> " && std::getline(in_, line);) {
        const std::vector<std::string> words = splitWords(line);
        if (!words.empty() && !execute(words)) {
            return;
        }
    }
    out_ << '\n';
}

bool ConsoleUI::execute(const std::vector<std::string>& words) {
    if (words.size() == 1 && words[0] == "quit") {
        return false;
    }
    if (words.size() == 1 && words[0] == "help") {
        showHelp();
    } else if (words.size() == 2 && words[0] == "moves") {
        showMoves(words[1]);
    } else if (words.size() == 2) {
        makeMove(words[0], words[1]);
    } else {
        out_ << "Comando desconocido. Escribí \"help\" para ver los comandos.\n";
    }
    return true;
}

void ConsoleUI::makeMove(const std::string& fromText, const std::string& toText) {
    const auto& dims = game_.state().board().dimensions();
    const std::optional<Position> from = AlgebraicNotation::parse(fromText, dims);
    const std::optional<Position> to = AlgebraicNotation::parse(toText, dims);
    if (!from || !to) {
        out_ << "Casilla inválida: " << (from ? toText : fromText) << '\n';
        return;
    }

    // Si la jugada es válida, el ConsoleRenderer ya dibujó el tablero (onMoveMade).
    const ports::MoveResult result = game_.makeMove(*from, *to);
    if (!result.ok()) {
        reportError(result, fromText, toText);
    }
}

void ConsoleUI::showMoves(const std::string& squareText) {
    const std::optional<Position> from = AlgebraicNotation::parse(squareText, game_.state().board().dimensions());
    if (!from) {
        out_ << "Casilla inválida: " << squareText << '\n';
        return;
    }

    const auto moves = game_.legalMovesFrom(*from);
    if (moves.empty()) {
        out_ << "No hay jugadas desde " << squareText << ".\n";
        return;
    }
    // Ordenadas por columna y fila ("a3 c3"; "a2" antes que "a10"), no en el orden de las reglas.
    std::vector<Position> destinations;
    for (const auto& move : moves) {
        destinations.push_back(move.to());
    }
    std::sort(destinations.begin(), destinations.end());
    out_ << "Jugadas desde " << squareText << ":";
    for (const Position destination : destinations) {
        out_ << ' ' << AlgebraicNotation::format(destination);
    }
    out_ << '\n';
}

void ConsoleUI::showHelp() {
    out_ << "Comandos:\n"
         << "  e2 e4     mover la pieza de e2 a e4\n"
         << "  moves e2  ver a dónde puede ir la pieza de e2\n"
         << "  help      mostrar esta ayuda\n"
         << "  quit      salir\n";
}

void ConsoleUI::reportError(const ports::MoveResult& result, const std::string& fromText,
                            const std::string& toText) {
    // Traducir el motivo a texto es cosa de la presentación: el núcleo solo da el MoveError.
    switch (*result.error()) {
        case MoveError::NoPieceAtOrigin:
            out_ << "No hay ninguna pieza en " << fromText << ".\n";
            break;
        case MoveError::NotYourTurn:
            out_ << "Esa pieza no es tuya: juegan las " << colorName(game_.state().sideToMove()) << ".\n";
            break;
        case MoveError::IllegalMove:
            out_ << "Jugada ilegal: " << fromText << '-' << toText << ".\n";
            break;
    }
}

}  // namespace chess::adapters::console
