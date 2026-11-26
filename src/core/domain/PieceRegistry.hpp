#pragma once

#include <map>
#include <memory>
#include <vector>

#include "domain/PieceDefinition.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

class PieceRegistry {
public:
    void add(std::shared_ptr<const PieceDefinition> definition);

    std::shared_ptr<const PieceDefinition> find(const PieceTypeId& id) const;
    bool contains(const PieceTypeId& id) const;

    std::vector<PieceTypeId> promotableTypes() const;

private:
    std::map<PieceTypeId, std::shared_ptr<const PieceDefinition>> definitions_;
};

}
