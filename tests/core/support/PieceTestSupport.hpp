#pragma once

// Helpers para los tests de piezas: definiciones mínimas sin reglas de movimiento.
#include <memory>
#include <string>
#include <vector>

#include "domain/MovementRule.hpp"
#include "domain/PieceDefinition.hpp"
#include "domain/PieceTraits.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::testing {

inline std::shared_ptr<const domain::PieceDefinition> makeBareDefinition(
    const std::string& id, char symbol, domain::PieceTraits traits = {}) {
    return std::make_shared<const domain::PieceDefinition>(
        domain::PieceTypeId(id), symbol, std::vector<std::unique_ptr<domain::MovementRule>>{}, traits);
}

}  // namespace chess::testing
