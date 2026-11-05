# Diagramas (PlantUML)

Fuente de verdad del UML del proyecto. **Los `.puml` son la fuente; las imágenes de `export/` se generan desde ellos** (ver "Renderizar").

## Diagramas esperados (ver `docs/roadmap.md`, fase 1)

| Archivo | Tipo | Muestra |
|---|---|---|
| `componentes.puml` | Componentes/paquetes | Hexagonal: núcleo, puertos, adaptadores y dependencias |
| `clases-dominio.puml` | Clases | Dominio: piezas, reglas, tablero y motor |
| `clases-aplicacion.puml` | Clases | Puertos, `ChessGame`, Command, State y Observer |
| `secuencia-jugada.puml` | Secuencia | Una jugada desde la UI hasta los observers |
| `actividad-validacion.puml` | Actividad | Pipeline de validación de movimientos |
| `estados-partida.puml` | Estados | En curso / jaque / mate / ahogado |

## Convenciones

- Títulos y notas en español; nombres de clases y métodos **idénticos al código** (en inglés).
- Marcar los patrones con estereotipos: `<<Strategy>>`, `<<Factory>>`, `<<Facade>>`, `<<Command>>`, `<<Observer>>`, `<<State>>`, `<<Port>>`, `<<Adapter>>`.
- Agrupar por paquete (`package chess::domain { ... }`).
- Incluir solo los miembros públicos relevantes (sin getters triviales).
- **Regla de oro:** si cambia la interfaz pública de una clase, el `.puml` se actualiza en el mismo commit.

## Renderizar

- VS Code: extensión "PlantUML" (jebbs.plantuml) → `Alt+D` para previsualizar.
- CLI, solo con Java (sin Graphviz): `sh docs/diagramas/exportar.sh ruta/a/plantuml.jar`. Genera SVG y PNG
  en [`export/`](export/). Usa el motor ELK para los diagramas de clases y Smetana para el resto (los dos
  vienen dentro del jar; Smetana no puede con `clases-dominio.puml`).
- Las imágenes de `export/` se versionan para la entrega, pero **la fuente de verdad son los `.puml`**:
  después de cambiar un diagrama hay que volver a correr el script.

## Imágenes exportadas

| Diagrama | SVG | PNG |
|---|---|---|
| Componentes | [svg](export/componentes.svg) | [png](export/componentes.png) |
| Clases del dominio | [svg](export/clases-dominio.svg) | [png](export/clases-dominio.png) |
| Clases de aplicación, puertos y adaptadores | [svg](export/clases-aplicacion.svg) | [png](export/clases-aplicacion.png) |
| Secuencia de una jugada | [svg](export/secuencia-jugada.svg) | [png](export/secuencia-jugada.png) |
| Actividad: validación | [svg](export/actividad-validacion.svg) | [png](export/actividad-validacion.png) |
| Estados de la partida | [svg](export/estados-partida.svg) | [png](export/estados-partida.png) |

## Verificación UML ↔ código (misión 9.2)

Se recorrió cada header de `src/` contra los diagramas de clases, comparando clases, métodos públicos
(nombre, parámetros y retorno), atributos que guardan dependencias y multiplicidades.

| Paquete | Clases revisadas | Resultado |
|---|---|---|
| `chess::domain` | `Color`, `Position`, `Offset`, `Directions`, `BoardDimensions`, `PieceTypeId`, `MoveKind`, `Move`, `GameStatus`, `PieceTraits`, `PieceDefinition`, `Piece`, `PieceRegistry`, `PieceFactory`, `StandardPieces`, `MovementRule`, `MoveContext`, `SlidingRule`, `LeaperRule`, `PawnRule`, `CastlingRule`, `RuleHelpers`, `Board`, `StartingPositionProvider`, `StandardStartingPosition`, `GameState`, `RulesEngine`, `MoveGenerator`, `AttackDetector`, `MoveApplier`, `LegalMoveFilter`, `GameStatusEvaluator` | Coinciden. Faltaba la constante `BoardDimensions::kStandardSize` (agregada). |
| `chess::application` | `ChessGame`, `GamePhase`, `InProgressPhase`, `GameOverPhase`, `MoveCommand`, `MoveHistory` | Coinciden (incluidos los privados de `ChessGame` que usa el diagrama de secuencia). |
| `chess::ports` | `GameService`, `GameObserver`, `MoveResult`, `MoveError` | Coinciden. |
| `chess::adapters::console` | `ConsoleUI`, `ConsoleRenderer`, `AlgebraicNotation` | Coinciden. |
| `chess::adapters::raylib` | `RaylibUI`, `BoardInteraction`, `BoardLayout` | Faltaban el struct `BoardLayout::Point` (retorno de `topLeftOf`) y el estado privado de `BoardInteraction` (agregados). |

Las relaciones también se contrastaron con los miembros: referencias `const X&` → asociación (`-->`),
miembros por valor → composición (`*--`), `shared_ptr` compartido → asociación `0..*` → `1`. El diagrama
de componentes coincide con los targets de CMake (`chess_core` no enlaza nada; raylib y GoogleTest solo
aparecen en adaptadores y tests). Por convención se omiten `operator!=` (simétrico de `==`) y los
helpers privados de una línea (`Board::indexOf`, `BoardInteraction::select`).
