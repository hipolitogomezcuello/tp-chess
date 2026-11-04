# Roadmap — misiones para agentes

> Cada misión es una unidad de trabajo para un agente (o una persona). Antes de empezar, leer
> `CLAUDE.md` y `docs/arquitectura.md`. Al terminar, marcar `[x]` y commitear.
> **Respetar el orden de las dependencias.** Una misión no empieza hasta que terminen las anteriores
> de las que depende.

Leyenda: `[ ]` pendiente · `[~]` en curso · `[x]` hecha

---

## Fase 0 — Fundación ✅

- [x] **0.1** CLAUDE.md, arquitectura, roadmap, plantilla de decisiones y estructura de carpetas.

## Fase 1 — Diseño (⚠️ SIN CÓDIGO) ✅

> Los agentes de esta fase **no escriben C++**. Producen diagramas PlantUML en `docs/diagramas/`
> y, si el diseño cambia, actualizan `docs/arquitectura.md`. El objetivo es pensar el diseño antes
> de implementarlo y tener el UML desde el día uno.

- [x] **1.1 Diagrama de componentes / hexagonal** → `docs/diagramas/componentes.puml`
  - Paquetes `domain`, `application`, `ports`, `adapters.console`, `adapters.raylib` y `app`, con las flechas de dependencia.
  - Terminado si: se ve claramente que ninguna flecha sale del núcleo hacia los adaptadores.
- [x] **1.2 Diagrama de clases del dominio** → `docs/diagramas/clases-dominio.puml`
  - Value objects, `PieceDefinition`/`Piece`, jerarquía de `MovementRule`, registro/fábrica, `Board`, `GameState` y el motor de reglas.
  - Notas UML que indiquen el patrón aplicado (`<<Strategy>>`, `<<Factory>>`, `<<Facade>>`…).
  - Terminado si: cada clase de `arquitectura.md` §2 aparece con sus métodos públicos y la multiplicidad de sus relaciones.
- [x] **1.3 Diagrama de clases de aplicación y puertos** → `docs/diagramas/clases-aplicacion.puml`
  - `GameService`, `GameObserver`, `ChessGame`, `MoveCommand`, `MoveHistory`, `GamePhase`…, más los adaptadores como clientes.
- [x] **1.4 Diagrama de secuencia: hacer una jugada** → `docs/diagramas/secuencia-jugada.puml`
  - Desde `ConsoleUI` hasta la notificación a los observers, con los dos caminos: jugada válida y jugada inválida.
- [x] **1.5 Diagrama de actividad: validación de movimiento** → `docs/diagramas/actividad-validacion.puml`
  - Pipeline: candidatos → pseudo-legales → filtro de jaque → evaluación de estado (jaque, mate, ahogado).
- [x] **1.6 Diagrama de estados de la partida** → `docs/diagramas/estados-partida.puml`
  - `InProgress` ⇄ `Check` → `Checkmate` / `Stalemate`, y cómo afecta deshacer.
- [x] **1.7 Revisión de diseño**
  - Recorrer los diagramas preguntando: ¿se puede agregar una pieza nueva sin modificar ninguna clase?
    ¿Y un tablero de 10×10? Si no, corregir el diseño. Registrar las decisiones en `docs/decisiones.md`.

## Fase 2 — Esqueleto de build ✅

- [x] **2.1** `CMakeLists.txt` raíz más los de los subdirectorios: targets `chess_core`, `chess_console` y `chess_tests` (raylib en la fase 8).
  - GoogleTest vía `FetchContent`, `enable_testing()`, warnings altos (`/W4` o `-Wall -Wextra -Wpedantic`).
  - `.gitignore` con `build/`.
  - Terminado si: `cmake -S . -B build && cmake --build build && ctest --test-dir build` pasa con un test trivial.

## Fase 3 — Dominio base (TDD)

- [x] **3.1** `Color`, `Position`, `Offset`, `BoardDimensions`, más sus tests.
  - Al agregar el primer `.cpp`, pasar `chess_core` de `INTERFACE` a `STATIC` (ver comentario en `src/core/CMakeLists.txt`) y borrar `tests/core/SmokeTest.cpp`.
- [x] **3.2** `PieceTypeId`, `PieceTraits`, `PieceDefinition`, `Piece`, `PieceRegistry`, `PieceFactory`.
- [x] **3.3** `Board` y el helper de tests `BoardBuilder`.
- [x] **3.4** `MovementRule`, `MoveContext`, `Move`, más `SlidingRule` y `LeaperRule` con tests (vacío, bloqueo, captura, borde).
- [x] **3.5** `PawnRule` sin al paso ni promoción: avance simple y doble, y captura diagonal.
- [x] **3.6** `StandardPieces` y `StandardStartingPosition`. Test: la posición inicial tiene 32 piezas bien ubicadas.
- [x] **3.7** Actualizar `clases-dominio.puml` según lo implementado.

## Fase 4 — Reglas: legalidad y jaque

- [x] **4.1** `GameState`, `MoveApplier`, `MoveGenerator`. (`GameState` se adelantó en la 3.4 porque `MoveContext` lo necesita.)
- [x] **4.2** `AttackDetector` (`isSquareAttacked`, `isInCheck`).
- [x] **4.3** `LegalMoveFilter`: piezas clavadas y el rey que no puede entrar en jaque.
- [x] **4.4** `RulesEngine` (Facade) y `GameStatusEvaluator` (por ahora: `InProgress` y `Check`).
- [x] **4.5** Actualizar diagramas.

## Fase 5 — Aplicación y puertos

- [x] **5.1** Puertos `GameService` y `GameObserver`, y el value object `MoveResult`.
- [x] **5.2** `ChessGame`: turnos, validación y captura. Tests con `FakeObserver`.
- [x] **5.3** Actualizar `clases-aplicacion.puml` y `secuencia-jugada.puml`.

## Fase 6 — Adaptador de consola → 🎯 MVP OBLIGATORIO

- [x] **6.1** `AlgebraicNotation`, `ConsoleRenderer` (observer) y `ConsoleUI`.
- [ ] **6.2** `app/main_console.cpp` (composition root).
- [ ] **6.3** Verificación manual: jugar una partida corta, intentar jugadas ilegales y provocar un jaque.
  - Al completar esta fase **se cumple todo el alcance obligatorio** de la consigna.

## Fase 7 — Opcionales

- [ ] **7.1** Jaque mate y ahogado en `GameStatusEvaluator`, más el patrón State (`GamePhase`) en `ChessGame`.
- [ ] **7.2** Promoción: extender `PawnRule` y `MoveApplier`, y el parámetro `promotion` en `makeMove`.
- [ ] **7.3** Enroque: `CastlingRule` y los derechos de enroque en `GameState`.
- [ ] **7.4** Captura al paso: `PawnRule` y la casilla de al paso en `GameState`.
- [ ] **7.5** Deshacer y rehacer: `MoveCommand` y `MoveHistory` (Command + Memento), con comandos en la consola.
- [ ] **7.6** Tests de extensibilidad: una pieza inventada y un tablero de 10×10.
- [ ] **7.7** Actualizar todos los diagramas y `decisiones.md`.

## Fase 8 — Adaptador raylib

- [ ] **8.1** raylib vía `FetchContent` y el target `chess_raylib`.
- [ ] **8.2** `RaylibUI`: dibujo del tablero, selección con el mouse, resaltado de jugadas legales, promoción y deshacer/rehacer.
- [ ] **8.3** `app/main_raylib.cpp`. Verificar que **no se tocó nada de `src/core/`** para agregar esta UI.

## Fase 9 — Entregables

- [ ] **9.1** Completar y pulir `docs/decisiones.md`.
- [ ] **9.2** Exportar los diagramas a PNG/SVG y verificar la consistencia UML ↔ código clase por clase.
- [ ] **9.3** Presentación PPTX: arquitectura, patrones, demo en dos UIs y demo de extensión en vivo.
- [ ] **9.4** Guía de defensa (`docs/guia-defensa.md`): paso a paso para agregar una pieza, cambiar el tamaño del tablero o agregar otra UI.
