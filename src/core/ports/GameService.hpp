#pragma once

#include <optional>
#include <vector>

#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "domain/Piece.hpp"
#include "domain/PieceTypeId.hpp"
#include "domain/Position.hpp"
#include "ports/GameObserver.hpp"
#include "ports/MoveResult.hpp"

namespace chess::ports {

class GameService {
public:
    virtual ~GameService() = default;

    virtual MoveResult makeMove(domain::Position from, domain::Position to,
                                std::optional<domain::PieceTypeId> promotion = std::nullopt) = 0;

    virtual std::vector<domain::Move> legalMovesFrom(domain::Position from) const = 0;

    virtual const domain::GameState& state() const = 0;
    virtual domain::GameStatus status() const = 0;

    virtual std::vector<domain::Piece> promotionChoices() const = 0;

    virtual bool undo() = 0;
    virtual bool redo() = 0;

    virtual void addObserver(GameObserver& observer) = 0;
};

}
