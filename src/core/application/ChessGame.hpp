#pragma once

#include <functional>
#include <vector>

#include "domain/GameState.hpp"
#include "domain/GameStatus.hpp"
#include "domain/Move.hpp"
#include "domain/Position.hpp"
#include "domain/RulesEngine.hpp"
#include "ports/GameObserver.hpp"
#include "ports/GameService.hpp"
#include "ports/MoveResult.hpp"

namespace chess::application {

// Caso de uso "jugar una partida": implementa el puerto de entrada GameService.
// Guarda el GameState actual, valida el turno, le pide al RulesEngine (Facade) las jugadas legales
// y avisa a los observers (Observer, §3.9). Las reglas de ajedrez no viven acá: viven en el dominio.
// Dependencias por constructor (§3.10): el RulesEngine lo crea y posee el composition root.
// Fase 7: MoveHistory (deshacer/rehacer), GamePhase (State) y la promoción.
class ChessGame : public ports::GameService {
public:
    // Guarda una referencia al motor: debe vivir más que el juego.
    ChessGame(const domain::RulesEngine& rules, domain::GameState initialState);

    ports::MoveResult makeMove(domain::Position from, domain::Position to) override;
    std::vector<domain::Move> legalMovesFrom(domain::Position from) const override;
    const domain::GameState& state() const override { return state_; }
    domain::GameStatus status() const override;
    void addObserver(ports::GameObserver& observer) override;

private:
    // ¿Hay en `from` una pieza del bando que mueve? (fuera del tablero cuenta como vacía)
    bool hasPieceOfSideToMove(domain::Position from) const;
    void notifyAfterMove(const domain::Move& move);

    const domain::RulesEngine& rules_;
    domain::GameState state_;
    std::vector<std::reference_wrapper<ports::GameObserver>> observers_;
};

}  // namespace chess::application
