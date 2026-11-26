#pragma once

#include <memory>
#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/PieceTraits.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

class PieceDefinition {
public:
    PieceDefinition(PieceTypeId id, char symbol, std::vector<std::unique_ptr<MovementRule>> rules,
                    PieceTraits traits);

    PieceDefinition(const PieceDefinition&) = delete;
    PieceDefinition& operator=(const PieceDefinition&) = delete;

    const PieceTypeId& id() const { return id_; }
    char symbol() const { return symbol_; }
    const std::vector<std::unique_ptr<MovementRule>>& rules() const { return rules_; }
    const PieceTraits& traits() const { return traits_; }

private:
    PieceTypeId id_;
    char symbol_;
    std::vector<std::unique_ptr<MovementRule>> rules_;
    PieceTraits traits_;
};

}
