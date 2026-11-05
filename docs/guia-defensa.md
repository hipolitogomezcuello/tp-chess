# Guía de defensa — extensiones en vivo

> Para la defensa individual (consigna §8): cómo agregar **en el momento** una pieza nueva, cambiar el
> tamaño del tablero o sumar otra UI **sin modificar ninguna clase existente**. Cada receta dice qué
> archivos se crean, cuáles se tocan (solo el composition root y las listas de CMake) y qué decir.
>
> Los fragmentos de código de esta guía se compilaron y probaron sobre el repositorio (misión 9.4).
> Los tests de `tests/core/extensibility/` son el mismo ensayo, ya automatizado.

---

## 0. Antes de entrar

1. Compilar y correr todo una vez (la primera configuración descarga dependencias):
   ```bash
   cmake -S . -B build
   cmake --build build
   ctest --test-dir build -C Debug --output-on-failure     # 300 tests
   ```
2. Tener abiertos: `app/main_console.cpp` (composition root), `src/core/domain/StandardPieces.cpp`
   (cómo se arman las piezas clásicas) y `docs/diagramas/export/clases-dominio.svg`.
3. Repasar la idea central en una frase: **una pieza es datos (id + símbolo + reglas + traits), no una
   subclase; el tablero es un parámetro (`BoardDimensions`), no una constante; la UI es un adaptador
   del puerto `GameService`.**

### Mapa: qué se toca en cada caso

| Pedido | Archivos nuevos | Archivos que se editan | Clases existentes modificadas |
|---|---|---|---|
| Pieza que combina movimientos existentes | — | composition root | ninguna |
| Pieza con un movimiento nuevo | `XxxRule.hpp/.cpp` + test | composition root, listas de CMake, `.puml` | ninguna |
| Pieza en la posición inicial / otro tablero | `XxxStartingPosition.hpp/.cpp` | composition root, lista de CMake, `.puml` | ninguna |
| Otra UI | `src/adapters/xxx/` + `app/main_xxx.cpp` | `CMakeLists.txt` de `src/` y `app/`, `componentes.puml` | ninguna |

Editar el composition root o una lista de archivos de CMake **no es modificar una clase**: es
configurar el armado (D-17). Lo que no se toca nunca es el motor: `MoveGenerator`, `AttackDetector`,
`LegalMoveFilter`, `RulesEngine`, `ChessGame`, `Board`.

---

## 1. Pieza nueva que combina movimientos existentes (≈5 minutos)

**Ejemplo:** el Arzobispo, que se mueve como alfil **o** como caballo, y que un peón puede elegir al
promover. Otras del mismo tipo: Canciller (torre + caballo), Camello (salta 3+1: `LeaperRule` con esos
offsets), Rey sin enroque (solo `LeaperRule(Directions::all())`).

En `app/main_console.cpp` (y/o `app/main_raylib.cpp`), después de `StandardPieces::registerAll(registry);`:

```cpp
// Pieza nueva: Arzobispo = Alfil + Caballo. Solo se compone y se registra (D-02, D-03).
{
    std::vector<std::unique_ptr<domain::MovementRule>> rules;
    rules.push_back(std::make_unique<domain::SlidingRule>(domain::Directions::diagonals()));
    rules.push_back(std::make_unique<domain::LeaperRule>(domain::Directions::knightJumps()));
    domain::PieceTraits traits;
    traits.isPromotable = true;   // un peón puede promover a ella (D-08)
    registry.add(std::make_shared<const domain::PieceDefinition>(
        domain::PieceTypeId("archbishop"), 'A', std::move(rules), traits));
}
```

Includes que hacen falta: `<memory>`, `<vector>`, `"domain/Directions.hpp"`, `"domain/LeaperRule.hpp"`,
`"domain/PieceDefinition.hpp"`, `"domain/SlidingRule.hpp"`.

**Cómo mostrarlo funcionando:** llevar un peón a la última fila y escribir, por ejemplo, `b7 a8 a`
(la letra es el símbolo) o `b7 a8 archbishop`. Si se escribe `b7 a8` sin pieza, el juego pide elegirla. La consola lista las opciones de promoción y ahora aparece `A`; en
raylib aparece un botón más. Para verla desde el principio de la partida, ver la sección 3.

**Qué decir:**
- No existe la clase `Archbishop`: la pieza es **datos**. Las reglas son Strategies que ya estaban
  testeadas (`SlidingRule`, `LeaperRule`).
- El generador de jugadas, el detector de jaque y el filtro de legalidad recorren `definition().rules()`
  sin preguntar el tipo: el Arzobispo ya da jaque, puede quedar clavado y puede dar mate.
- La promoción no sabe qué piezas existen: pregunta el trait `isPromotable` (D-08, D-19). Por eso
  ninguna UI cambió.
- Si el `PieceTypeId` fuera un `enum`, habría que editarlo, y cada `switch` que lo usa (D-03).
- Si se registra dos veces el mismo id, `PieceRegistry::add` lanza una excepción: es un error de
  programación, no de juego (D-05).

---

## 2. Pieza con un movimiento que no existe (≈15–20 minutos)

**Ejemplo:** el Saltamontes: se mueve en las 8 direcciones de la reina, pero **tiene que saltar
exactamente una pieza** (de cualquier color) y cae en la casilla siguiente. Captura si esa casilla
tiene una pieza enemiga. Ninguna regla actual hace eso, así que se escribe **una clase nueva** que
implementa `MovementRule`.

### 2.1 Test primero (TDD)

`tests/core/domain/HopperRuleTest.cpp` y agregarlo a la lista de `chess_tests` en `tests/CMakeLists.txt`:

```cpp
#include "support/RuleTestFixture.hpp"
#include "domain/Directions.hpp"
#include "domain/HopperRule.hpp"
#include "support/MoveAssertions.hpp"

using chess::domain::Color;
using chess::domain::Directions;
using chess::domain::GameState;
using chess::domain::HopperRule;
using chess::domain::MoveKind;
using chess::testing::destinations;
using chess::testing::findMoveTo;
using chess::testing::RuleTest;
using chess::testing::sq;
using chess::testing::squares;

using HopperRuleTest = RuleTest;

TEST_F(HopperRuleTest, WithoutAHurdleItCannotMove) {
    const HopperRule hopper(Directions::all());
    const GameState state(builder().place("d4", kPiece, Color::White).build());

    EXPECT_TRUE(movesOf(hopper, state, "d4").empty());
}

TEST_F(HopperRuleTest, LandsRightBehindTheFirstPieceInEachLine) {
    const HopperRule hopper(Directions::all());
    const GameState state(builder()
                              .place("d4", kPiece, Color::White)
                              .place("d6", kPiece, Color::White)   // valla propia: salta a d7
                              .place("f4", kPiece, Color::Black)   // valla rival: salta a g4
                              .place("h4", kPiece, Color::Black)   // no es la primera: no importa
                              .build());

    EXPECT_EQ(destinations(movesOf(hopper, state, "d4")), squares({"d7", "g4"}));
}

TEST_F(HopperRuleTest, CapturesAnEnemyBehindTheHurdleButNotAnOwnPiece) {
    const HopperRule hopper(Directions::all());
    const GameState state(builder()
                              .place("d4", kPiece, Color::White)
                              .place("d5", kPiece, Color::White)
                              .place("d6", kPiece, Color::Black)   // detrás de la valla: captura
                              .place("e5", kPiece, Color::Black)
                              .place("f6", kPiece, Color::White)   // detrás de la valla: propia
                              .build());

    const auto moves = movesOf(hopper, state, "d4");
    EXPECT_EQ(destinations(moves), squares({"d6"}));
    EXPECT_EQ(findMoveTo(moves, sq("d6"))->kind(), MoveKind::Capture);
    // f6 no es una jugada, pero sí una casilla atacada (defendida, D-15).
    EXPECT_EQ(attacksOf(hopper, state, "d4"), squares({"d6", "f6"}));
}

TEST_F(HopperRuleTest, CannotJumpOffTheBoard) {
    const HopperRule hopper(Directions::all());
    const GameState state(builder()
                              .place("a1", kPiece, Color::White)
                              .place("a8", kPiece, Color::Black)
                              .build());

    EXPECT_TRUE(movesOf(hopper, state, "a1").empty());
}
```

### 2.2 La regla

`src/core/domain/HopperRule.hpp`:

```cpp
#pragma once

#include <vector>

#include "domain/Move.hpp"
#include "domain/MoveContext.hpp"
#include "domain/MovementRule.hpp"
#include "domain/Offset.hpp"
#include "domain/Position.hpp"

namespace chess::domain {

// Saltamontes: en cada dirección salta la primera pieza que encuentra y cae justo detrás.
// Patrón: Strategy — ver docs/arquitectura.md §3.2
class HopperRule : public MovementRule {
public:
    explicit HopperRule(std::vector<Offset> directions);

    std::vector<Move> candidateMoves(const MoveContext& context) const override;
    std::vector<Position> attackedSquares(const MoveContext& context) const override;

private:
    std::vector<Offset> directions_;
};

}  // namespace chess::domain
```

`src/core/domain/HopperRule.cpp` (y agregarlo a `target_sources` en `src/core/CMakeLists.txt`):

```cpp
#include "domain/HopperRule.hpp"

#include <utility>

#include "domain/Board.hpp"
#include "domain/RuleHelpers.hpp"

namespace chess::domain {

HopperRule::HopperRule(std::vector<Offset> directions) : directions_(std::move(directions)) {}

std::vector<Move> HopperRule::candidateMoves(const MoveContext& context) const {
    // Igual que SlidingRule y LeaperRule: vacía → Normal, enemiga → Capture, propia → no (D-15).
    return movesOntoReachableSquares(context, attackedSquares(context));
}

std::vector<Position> HopperRule::attackedSquares(const MoveContext& context) const {
    const Board& board = context.board();
    std::vector<Position> landings;
    for (const Offset& direction : directions_) {
        Position square = context.from() + direction;
        while (board.dimensions().contains(square) && board.isEmpty(square)) {
            square = square + direction;   // busca la valla
        }
        if (!board.dimensions().contains(square)) {
            continue;                      // no hay pieza que saltar en esta dirección
        }
        const Position landing = square + direction;
        if (board.dimensions().contains(landing)) {
            landings.push_back(landing);
        }
    }
    return landings;
}

}  // namespace chess::domain
```

### 2.3 Registrar la pieza

En el composition root, igual que en la sección 1 pero con la regla nueva:

```cpp
std::vector<std::unique_ptr<domain::MovementRule>> rules;
rules.push_back(std::make_unique<domain::HopperRule>(domain::Directions::all()));
domain::PieceTraits traits;
traits.isPromotable = true;
registry.add(std::make_shared<const domain::PieceDefinition>(
    domain::PieceTypeId("grasshopper"), 'G', std::move(rules), traits));
```

### 2.4 UML

En `docs/diagramas/clases-dominio.puml`, dentro de `package "Reglas de movimiento"`:

```plantuml
class HopperRule {
  +HopperRule(directions: vector<Offset>)
}
```
y en las relaciones: `MovementRule <|.. HopperRule` y `HopperRule ..> RuleHelpers : usa`. Regenerar
las imágenes con `sh docs/diagramas/exportar.sh ruta/a/plantuml.jar`.

**Qué decir:**
- Es una clase **nueva** que implementa una interfaz existente: abierto/cerrado en su forma literal.
- `attackedSquares` está separado de `candidateMoves` (§2.3 de la arquitectura): acá coinciden, y por
  eso se reutiliza `movesOntoReachableSquares`. En el peón no coinciden, y por eso existen los dos.
- La regla solo mira geometría. Si la jugada deja al rey en jaque, la descarta `LegalMoveFilter`,
  que ya existía y no se tocó (D-04).
- Los bordes salen de `board.dimensions()`: la regla funciona igual en un 10×10 (D-09).

---

## 3. Otro tamaño de tablero o la pieza en la posición inicial (≈10 minutos)

**Ejemplo:** tablero de 10×10 con el Arzobispo (`A`) y el Canciller (`C`, torre + caballo) en la fila
de atrás: `R N A B Q K B C N R`. Es una implementación más de `StartingPositionProvider` (Strategy, §3.5).

`src/core/domain/TenByTenStartingPosition.hpp`:

```cpp
#pragma once

#include "domain/Board.hpp"
#include "domain/PieceFactory.hpp"
#include "domain/StartingPositionProvider.hpp"

namespace chess::domain {

// Tablero de 10×10: R N A B Q K B C N R y una fila de peones por bando.
// Requiere que "archbishop" y "chancellor" estén registrados.
// Patrón: Strategy — ver docs/arquitectura.md §3.5
class TenByTenStartingPosition : public StartingPositionProvider {
public:
    Board createInitialBoard(const PieceFactory& factory) const override;
};

}  // namespace chess::domain
```

`src/core/domain/TenByTenStartingPosition.cpp` (y agregarlo a `src/core/CMakeLists.txt`):

```cpp
#include "domain/TenByTenStartingPosition.hpp"

#include <vector>

#include "domain/StandardPieces.hpp"

namespace chess::domain {

Board TenByTenStartingPosition::createInitialBoard(const PieceFactory& factory) const {
    const BoardDimensions dimensions(10, 10);
    const std::vector<PieceTypeId> backRank{
        StandardPieces::kRook,  StandardPieces::kKnight, PieceTypeId("archbishop"), StandardPieces::kBishop,
        StandardPieces::kQueen, StandardPieces::kKing,   StandardPieces::kBishop,   PieceTypeId("chancellor"),
        StandardPieces::kKnight, StandardPieces::kRook};
    const int blackBack = dimensions.ranks() - 1;

    Board board(dimensions);
    for (int file = 0; file < dimensions.files(); ++file) {
        const auto& type = backRank[static_cast<std::size_t>(file)];
        board.place(Position(file, 0), factory.create(type, Color::White));
        board.place(Position(file, 1), factory.create(StandardPieces::kPawn, Color::White));
        board.place(Position(file, blackBack - 1), factory.create(StandardPieces::kPawn, Color::Black));
        board.place(Position(file, blackBack), factory.create(type, Color::Black));
    }
    return board;
}

}  // namespace chess::domain
```

En el composition root, registrar las dos piezas (sección 1) y cambiar **una línea**:

```cpp
const domain::TenByTenStartingPosition startingPosition;   // antes: StandardStartingPosition
```

Para un tablero de otro tamaño **sin piezas nuevas** (por ejemplo, 8×10 con las piezas clásicas y
dos filas vacías más), alcanza con cambiar `BoardDimensions(8, 10)` y la fila de atrás de una copia de
`StandardStartingPosition`.

**Cómo mostrarlo funcionando:** `chess_console` dibuja 10 columnas (`a`–`j`) y 10 filas; `e2 e4` es un
doble avance, `j2 j4` funciona, y el enroque corto es `f1 h1` una vez despejadas `g1`, `h1` e `i1` (el
rey va dos casillas hacia la torre de `j1`).
`moves c1` lista las jugadas del Arzobispo. En raylib, la ventana dibuja el tablero de 10×10 sola.

**Qué decir:**
- `Board` guarda su `BoardDimensions`; ninguna regla usa el número 8 (D-09). El doble avance del peón
  mira `hasMoved()`, no "estar en la fila 2", y la promoción ocurre en la última fila real.
- El enroque deriva sus derechos de `hasMoved` y busca la torre por el trait `canCastleWith` en lugar
  de casillas fijas (D-06), así que funciona con el rey en `f1`.
- `AlgebraicNotation::parse` recibe las dimensiones, por eso acepta `j10` (D-23).
- Está probado sin UI en `tests/core/extensibility/LargeBoardTest.cpp`.

---

## 4. Otra UI (≈20–30 minutos para algo mínimo)

Una UI nueva es **un adaptador**: usa el puerto de entrada `GameService` y, si quiere enterarse de lo
que pasa, implementa el puerto de salida `GameObserver`. El núcleo no se entera.

Ejemplo mínimo: un observer que registra la partida en un archivo (o cualquier `std::ostream`), que
puede convivir con la consola o con raylib a la vez.

`src/adapters/log/GameLogger.hpp`:

```cpp
#pragma once

#include <ostream>

#include "ports/GameObserver.hpp"

namespace chess::adapters::log {

// Adaptador de salida: escribe cada evento de la partida en un stream.
class GameLogger : public ports::GameObserver {
public:
    explicit GameLogger(std::ostream& out) : out_(out) {}

    void onMoveMade(const domain::Move& move, const domain::GameState& state) override;
    void onCheck(domain::Color colorInCheck) override;
    void onGameOver(domain::GameStatus result, domain::Color sideToMove) override;
    void onUndoRedo(const domain::GameState& state) override;

private:
    std::ostream& out_;
};

}  // namespace chess::adapters::log
```

Pasos:
1. Crear `src/adapters/log/` con la clase y su `CMakeLists.txt` (`add_library(chess_log_adapter ...)`
   que enlaza `chess_core`), y sumar `add_subdirectory(src/adapters/log)` en el `CMakeLists.txt` raíz,
   junto a los otros adaptadores.
2. En el composition root: `std::ofstream file("partida.log"); adapters::log::GameLogger logger(file);
   game.addObserver(logger);` — antes de crear el juego, por el tiempo de vida (D-16).
3. Tests con un `std::ostringstream`, como los de `ConsoleRenderer`.
4. `componentes.puml` y `clases-aplicacion.puml`: el paquete nuevo con `GameObserver <|.. GameLogger`.

Para una UI interactiva (web, otra librería gráfica) el esquema es el de raylib: una clase que dibuja
(la única que conoce la librería), una clase sin dependencias que interpreta la entrada usando
`GameService` (como `BoardInteraction`, D-22) y un `app/main_xxx.cpp` que arma el mismo grafo.

**Qué decir:**
- `git diff --stat -- src/core` está vacío después de agregar la UI. Es exactamente lo que se verificó
  al agregar raylib (misión 8.3).
- El núcleo define los puertos y los adaptadores los implementan: la dependencia apunta hacia adentro
  (D-01). `chess_core` no enlaza ninguna librería, así que si el núcleo incluyera algo de la UI, no
  compilaría.
- Ninguna UI dibuja piezas por tipo: usan `Piece::symbol()`, por eso las piezas nuevas aparecen solas.

---

## 5. Otros pedidos posibles y dónde caen

| Pedido | Dónde se resuelve | Comentario |
|---|---|---|
| "El rey no puede enrocar" / variante sin enroque | Registrar otro rey sin `CastlingRule` en lugar de `registerAll` | `registerAll` es solo una conveniencia; se puede registrar pieza por pieza. |
| "La torre no puede enrocar, pero esta otra pieza sí" | Trait `canCastleWith` | Sin tocar `CastlingRule`. |
| "Un segundo rey" o pieza real | Trait `isRoyal` | `AttackDetector::isInCheck` busca piezas `isRoyal`; con dos reyes cuenta jaque a cualquiera de los dos. |
| "Que el peón no pueda promover a reina" | Trait `isPromotable` de la reina | Una definición distinta de la reina. |
| Tablas por 50 movimientos / repetición | `GameState` + `GameStatusEvaluator` | No implementado; el plan está en D-24. Es el único pedido de esta lista que **sí** modifica clases existentes (agrega un estado de partida). |
| IA | Adaptador nuevo que usa `GameService` | D-24. |
| Guardar/cargar partidas | Puerto de salida nuevo (`GameRepository`) + adaptador de archivo | La persistencia es infraestructura: nunca va en el núcleo. |

## 6. Preguntas de teoría frecuentes

- **¿Por qué no hay subclases de pieza?** D-02. Con herencia, la Reina duplica Torre + Alfil y el
  Arzobispo exige herencia múltiple. Con composición, todo se arma con reglas existentes.
- **¿Dónde está el jaque?** En un solo lugar: `AttackDetector` + `LegalMoveFilter` (D-04). Las reglas
  no saben nada del jaque.
- **¿Qué patrones usaron y dónde?** `arquitectura.md` §3 (catálogo con dónde y por qué). En el código,
  cada clase con patrón tiene el comentario `// Patrón: ... — ver docs/arquitectura.md §3.x`.
- **¿Cómo saben que el UML es real?** Se actualiza en el mismo commit que el código (D-11) y se
  verificó clase por clase en la misión 9.2 (`docs/diagramas/README.md`).
- **¿Cuándo dejaría de servir este diseño?** Cada decisión tiene su "cuándo se rompería" en
  `decisiones.md`. Los ejemplos más fuertes: un motor competitivo (D-04, copiar estados es lento),
  tableros no rectangulares (D-09) y partidas que terminan por algo que no está en el tablero (D-10).
- **¿Cómo se testea el núcleo sin infraestructura?** `chess_tests` solo enlaza `chess_core` y GoogleTest.
  Las posiciones se arman con `BoardBuilder` y los eventos se verifican con un `FakeObserver`.
