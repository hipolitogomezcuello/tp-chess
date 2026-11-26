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

class ConsoleUI {
public:
    ConsoleUI(ports::GameService& game, std::istream& in, std::ostream& out)
        : game_(game), in_(in), out_(out) {}

    void run();

private:
    bool execute(const std::vector<std::string>& words);
    void makeMove(const std::string& fromText, const std::string& toText, const std::string& promotionText = "");
    std::optional<domain::PieceTypeId> findPromotionChoice(const std::string& text) const;
    std::string promotionChoicesText() const;
    void showMoves(const std::string& squareText);
    void showHelp();
    void reportError(const ports::MoveResult& result, const std::string& fromText, const std::string& toText);

    ports::GameService& game_;
    std::istream& in_;
    std::ostream& out_;
};

}
