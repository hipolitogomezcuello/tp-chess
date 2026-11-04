#pragma once

#include <map>
#include <memory>
#include <vector>

#include "domain/PieceDefinition.hpp"
#include "domain/PieceTypeId.hpp"

namespace chess::domain {

// Patrón: Registry (+ Factory en PieceFactory) — ver docs/arquitectura.md §3.4
// Catálogo de tipos de pieza disponibles en la partida. Agregar un tipo = add(), sin switch.
// Se arma en el composition root (StandardPieces::registerAll y, si hace falta, piezas nuevas).
class PieceRegistry {
public:
    // Lanza std::invalid_argument si la definición es nula o su id ya está registrado.
    void add(std::shared_ptr<const PieceDefinition> definition);

    // nullptr si el tipo no está registrado.
    std::shared_ptr<const PieceDefinition> find(const PieceTypeId& id) const;
    bool contains(const PieceTypeId& id) const;

    // Tipos con el trait isPromotable (D-08).
    std::vector<PieceTypeId> promotableTypes() const;

private:
    std::map<PieceTypeId, std::shared_ptr<const PieceDefinition>> definitions_;
};

}  // namespace chess::domain
