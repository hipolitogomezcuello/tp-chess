#pragma once

// Helpers para los tests de piezas: definiciones armadas a mano.
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/PieceDefinition.hpp"
#include "domain/PieceTraits.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::testing {

// rulesOf(std::make_unique<SlidingRule>(...), std::make_unique<LeaperRule>(...))
// (un vector de unique_ptr no se puede armar con una initializer_list porque no se copian).
template <typename... Rules>
std::vector<std::unique_ptr<domain::MovementRule>> rulesOf(std::unique_ptr<Rules>... rules) {
    std::vector<std::unique_ptr<domain::MovementRule>> result;
    (result.push_back(std::move(rules)), ...);
    return result;
}

inline std::shared_ptr<const domain::PieceDefinition> makeDefinition(
    const std::string& id, char symbol, std::vector<std::unique_ptr<domain::MovementRule>> rules,
    domain::PieceTraits traits = {}) {
    return std::make_shared<const domain::PieceDefinition>(domain::PieceTypeId(id), symbol, std::move(rules),
                                                           traits);
}

// Definición sin reglas de movimiento: alcanza para probar tablero, registro y fábrica.
inline std::shared_ptr<const domain::PieceDefinition> makeBareDefinition(
    const std::string& id, char symbol, domain::PieceTraits traits = {}) {
    return makeDefinition(id, symbol, {}, traits);
}

}  // namespace chess::testing
