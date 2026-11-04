#include "domain/PieceDefinition.hpp"

#include <utility>

namespace chess::domain {

PieceDefinition::PieceDefinition(PieceTypeId id, char symbol,
                                 std::vector<std::unique_ptr<MovementRule>> rules, PieceTraits traits)
    : id_(std::move(id)), symbol_(symbol), rules_(std::move(rules)), traits_(traits) {}

}  // namespace chess::domain
