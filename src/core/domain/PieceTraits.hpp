#pragma once

namespace chess::domain {

// Propiedades de un tipo de pieza que el motor consulta en lugar de preguntar "¿es un rey?".
// Reemplazan los if (tipo == Rey): el jaque busca isRoyal, la promoción isPromotable (D-08)
// y el enroque canCastleWith. Así una pieza nueva solo declara sus traits.
struct PieceTraits {
    bool isRoyal = false;        // su captura pierde la partida: no puede quedar en jaque
    bool canCastleWith = false;  // el rey puede enrocar con ella (la torre, en el ajedrez clásico)
    bool isPromotable = false;   // un peón puede promover a ella
};

}  // namespace chess::domain
