#pragma once

#include <istream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "domain/PieceTypeId.hpp"
#include "ports/GameService.hpp"
#include "ports/MoveResult.hpp"

namespace chess::adapters::console {

// Adaptador de entrada (driving): lee comandos de texto y los traduce a llamadas al puerto
// GameService. No conoce ChessGame ni el RulesEngine, y no dibuja el tablero: eso lo hace el
// ConsoleRenderer cuando el juego le avisa (Observer).
// Comandos: "e2 e4" (jugada), "e7 e8 q" (promoción), "moves e2", "help", "quit". Fase 7.5: "undo", "redo".
class ConsoleUI {
public:
    // Streams inyectados: std::cin/std::cout en la app, streams en memoria en los tests.
    ConsoleUI(ports::GameService& game, std::istream& in, std::ostream& out)
        : game_(game), in_(in), out_(out) {}

    // Lee comandos hasta "quit" o el fin de la entrada.
    void run();

private:
    // Devuelve false si el comando pide terminar.
    bool execute(const std::vector<std::string>& words);
    void makeMove(const std::string& fromText, const std::string& toText, const std::string& promotionText = "");
    // La pieza de promoción cuyo símbolo ("q") o id ("queen") es `text`, entre las que ofrece el juego.
    std::optional<domain::PieceTypeId> findPromotionChoice(const std::string& text) const;
    std::string promotionChoicesText() const;
    void showMoves(const std::string& squareText);
    void showHelp();
    void reportError(const ports::MoveResult& result, const std::string& fromText, const std::string& toText);

    ports::GameService& game_;
    std::istream& in_;
    std::ostream& out_;
};

}  // namespace chess::adapters::console
