#include "domain/StandardPieces.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "domain/Directions.hpp"
#include "domain/LeaperRule.hpp"
#include "domain/PawnRule.hpp"
#include "domain/PieceDefinition.hpp"
#include "domain/SlidingRule.hpp"

namespace chess::domain {

namespace {

using Rules = std::vector<std::unique_ptr<MovementRule>>;

Rules single(std::unique_ptr<MovementRule> rule) {
    Rules rules;
    rules.push_back(std::move(rule));
    return rules;
}

void add(PieceRegistry& registry, const PieceTypeId& id, char symbol, Rules rules, PieceTraits traits) {
    registry.add(std::make_shared<const PieceDefinition>(id, symbol, std::move(rules), traits));
}

}  // namespace

void StandardPieces::registerAll(PieceRegistry& registry) {
    PieceTraits promotable;
    promotable.isPromotable = true;

    PieceTraits rookTraits = promotable;
    rookTraits.canCastleWith = true;

    PieceTraits royal;
    royal.isRoyal = true;

    add(registry, kPawn, 'P', single(std::make_unique<PawnRule>()), PieceTraits{});
    add(registry, kRook, 'R', single(std::make_unique<SlidingRule>(Directions::orthogonals())), rookTraits);
    add(registry, kKnight, 'N', single(std::make_unique<LeaperRule>(Directions::knightJumps())), promotable);
    add(registry, kBishop, 'B', single(std::make_unique<SlidingRule>(Directions::diagonals())), promotable);
    add(registry, kQueen, 'Q', single(std::make_unique<SlidingRule>(Directions::all())), promotable);
    // El enroque (CastlingRule) se suma a las reglas del rey en la fase 7.
    add(registry, kKing, 'K', single(std::make_unique<LeaperRule>(Directions::all())), royal);
}

}  // namespace chess::domain
