#include "domain/PieceRegistry.hpp"

#include <stdexcept>
#include <utility>

namespace chess::domain {

void PieceRegistry::add(std::shared_ptr<const PieceDefinition> definition) {
    if (!definition) {
        throw std::invalid_argument("PieceRegistry: la definición no puede ser nula");
    }
    if (contains(definition->id())) {
        throw std::invalid_argument("PieceRegistry: tipo ya registrado: " + definition->id().value());
    }
    const PieceTypeId id = definition->id();
    definitions_.emplace(id, std::move(definition));
}

std::shared_ptr<const PieceDefinition> PieceRegistry::find(const PieceTypeId& id) const {
    const auto it = definitions_.find(id);
    return it == definitions_.end() ? nullptr : it->second;
}

bool PieceRegistry::contains(const PieceTypeId& id) const {
    return definitions_.count(id) > 0;
}

std::vector<PieceTypeId> PieceRegistry::promotableTypes() const {
    std::vector<PieceTypeId> result;
    for (const auto& [id, definition] : definitions_) {
        if (definition->traits().isPromotable) {
            result.push_back(id);
        }
    }
    return result;
}

}  // namespace chess::domain
