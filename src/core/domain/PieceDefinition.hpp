#pragma once

#include <memory>
#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/PieceTraits.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

// Patrón: composición sobre herencia — ver docs/arquitectura.md §3.3
// Describe UN TIPO de pieza: id + símbolo + reglas de movimiento + traits.
// No existen Rook, Knight… como subclases: la Torre es SlidingRule(ortogonales).
// Hay una sola instancia por tipo, compartida por todas las Piece de ese tipo.
class PieceDefinition {
public:
    PieceDefinition(PieceTypeId id, char symbol, std::vector<std::unique_ptr<MovementRule>> rules,
                    PieceTraits traits);

    // Es dueña de sus reglas: no se copia (se comparte por shared_ptr).
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

}  // namespace chess::domain
