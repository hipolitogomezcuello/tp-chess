#pragma once

// Fixture con las piezas clásicas registradas, para los tests del motor de reglas.
#include <gtest/gtest.h>

#include "domain/BoardDimensions.hpp"
#include "domain/GameState.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/PieceRegistry.hpp"
#include "domain/StandardPieces.hpp"
#include "domain/StandardStartingPosition.hpp"
#include "support/BoardBuilder.hpp"

namespace chess::testing {

class StandardPiecesFixture : public ::testing::Test {
protected:
    StandardPiecesFixture() : factory_(registry_) { domain::StandardPieces::registerAll(registry_); }

    BoardBuilder builder(domain::BoardDimensions dimensions = domain::BoardDimensions()) const {
        return BoardBuilder(factory_, dimensions);
    }

    domain::GameState initialState() const {
        return domain::GameState(domain::StandardStartingPosition().createInitialBoard(factory_));
    }

    const domain::PieceTypeId& kPawn = domain::StandardPieces::kPawn;
    const domain::PieceTypeId& kRook = domain::StandardPieces::kRook;
    const domain::PieceTypeId& kKnight = domain::StandardPieces::kKnight;
    const domain::PieceTypeId& kBishop = domain::StandardPieces::kBishop;
    const domain::PieceTypeId& kQueen = domain::StandardPieces::kQueen;
    const domain::PieceTypeId& kKing = domain::StandardPieces::kKing;

    domain::PieceRegistry registry_;
    domain::PieceFactory factory_;
};

}  // namespace chess::testing
