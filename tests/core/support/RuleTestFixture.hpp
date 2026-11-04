#pragma once

// Fixture común a los tests de reglas: un registro con piezas "de prueba" sin reglas propias.
// La regla bajo test se instancia aparte y se evalúa sobre un MoveContext; así se prueba la
// regla aislada, no la definición de la pieza.
#include <gtest/gtest.h>

#include <vector>

#include "domain/GameState.hpp"
#include "domain/Move.hpp"
#include "domain/MoveContext.hpp"
#include "domain/MovementRule.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "support/BoardBuilder.hpp"
#include "support/PieceTestSupport.hpp"
#include "support/Squares.hpp"

namespace chess::testing {

class RuleTest : public ::testing::Test {
protected:
    RuleTest() : factory_(registry_) {
        registry_.add(makeBareDefinition("piece", 'P'));
    }

    BoardBuilder builder(domain::BoardDimensions dimensions = domain::BoardDimensions()) const {
        return BoardBuilder(factory_, dimensions);
    }

    static std::vector<domain::Move> movesOf(const domain::MovementRule& rule, const domain::GameState& state,
                                             const std::string& from) {
        return rule.candidateMoves(domain::MoveContext(state, sq(from)));
    }

    static std::vector<domain::Position> attacksOf(const domain::MovementRule& rule,
                                                   const domain::GameState& state, const std::string& from) {
        return rule.attackedSquares(domain::MoveContext(state, sq(from)));
    }

    const domain::PieceTypeId kPiece{"piece"};
    domain::PieceRegistry registry_;
    domain::PieceFactory factory_;
};

}  // namespace chess::testing
