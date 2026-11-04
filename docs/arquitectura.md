# Arquitectura — TPO Ajedrez

> Especificación de diseño del proyecto. Describe **qué** módulos existen, **cómo** se relacionan
> y **qué patrones** se usan y por qué. Los diagramas PlantUML de `docs/diagramas/` deben reflejar
> este documento *y* el código; si alguno de los tres diverge, se corrige en el mismo commit.
>
> Los nombres de clases son la propuesta inicial. Los agentes de diseño (fase 1) pueden refinarlos,
> pero cualquier cambio debe actualizar este documento.

---

## 1. Visión general: arquitectura hexagonal (Ports & Adapters)

```
            ┌──────────────────────────── ADAPTADORES (driving) ───────────────────────────┐
            │   ConsoleUI (texto, "e2 e4")            RaylibUI (ventana, mouse)            │
            └──────────────┬───────────────────────────────────────┬───────────────────────┘
                           │ llama                                  │ llama
                           ▼                                        ▼
            ┌─────────────────────────────── PUERTOS ───────────────────────────────────────┐
            │  GameService   (entrada: lo que el núcleo ofrece)                              │
            │  GameObserver  (salida: lo que el núcleo necesita avisar)                      │
            └──────────────┬────────────────────────────────────────────────────────────────┘
                           │ implementa / usa
            ┌──────────────▼──────────────── NÚCLEO ────────────────────────────────────────┐
            │  application/  ChessGame, MoveHistory, MoveCommand                            │
            │        │ usa                                                                   │
            │  domain/       Board, Piece, PieceDefinition, MovementRule…, RulesEngine       │
            └───────────────────────────────────────────────────────────────────────────────┘
```

**Por qué hexagonal:** la consigna exige que el núcleo no dependa de frameworks ni infraestructura y
que sea testeable sin levantar nada. Con puertos explícitos, la UI es un detalle reemplazable: hay
**dos adaptadores del mismo puerto** (consola y raylib), lo que demuestra la arquitectura en la práctica.

### Regla de dependencias

- `domain` no depende de nadie (solo STL).
- `application` depende de `domain` y de `ports`.
- `ports` depende solo de tipos de `domain` (value objects que cruzan la frontera).
- `adapters` dependen de `ports` (y de los value objects de `domain` que los puertos exponen).
- `app/` (composition root) es el único lugar que conoce todo y arma el grafo de objetos.

### Targets CMake

| Target | Tipo | Contenido | Enlaza con |
|---|---|---|---|
| `chess_core` | librería estática | `src/core/**` | nada |
| `chess_console_adapter` | librería estática | `src/adapters/console/**` | `chess_core` |
| `chess_console` | ejecutable | `app/main_console.cpp` | `chess_console_adapter` |
| `chess_raylib` | ejecutable | `src/adapters/raylib/**` + `app/main_raylib.cpp` | `chess_core`, raylib |
| `chess_tests` | ejecutable de tests | `tests/core/**` | `chess_core`, GoogleTest |
| `chess_console_tests` | ejecutable de tests | `tests/adapters/console/**` | `chess_console_adapter`, GoogleTest |

El adaptador de consola es una librería aparte para poder testearlo (notación, dibujo y loop de
comandos con streams en memoria) sin mezclar esos tests con los del núcleo: `chess_tests` sigue sin
conocer ningún adaptador.

---

## 2. Dominio (`src/core/domain`, namespace `chess::domain`)

### 2.1 Value objects

| Clase | Responsabilidad |
|---|---|
| `Color` | `enum class { White, Black }` + `opposite()`. |
| `Position` | Coordenada `(file, rank)` con índices base 0. Inmutable, comparable. Sabe sumarse a un `Offset`. |
| `Offset` | Desplazamiento `(dFile, dRank)`. Usado para direcciones y saltos. |
| `BoardDimensions` | `files × ranks` (8×8 por defecto). `contains(Position)`. **Permite tableros de otro tamaño.** |
| `PieceTypeId` | Identificador de tipo de pieza (string corto: `"pawn"`, `"rook"`…). **No es un enum cerrado**: así se agregan piezas sin modificar código existente. |
| `Move` | `from`, `to`, `MoveKind` (`Normal`, `Capture`, `DoublePawnPush`, `EnPassant`, `Castling`, `Promotion`) y datos extra opcionales (pieza de promoción, movimiento de la torre en el enroque, casilla capturada al paso). |

### 2.2 Piezas: composición, no herencia

```
PieceDefinition                       Piece (valor, liviano, copiable)
 ├─ id: PieceTypeId                    ├─ definition: shared_ptr<const PieceDefinition>
 ├─ symbol: char                       ├─ color: Color
 ├─ rules: vector<unique_ptr<MovementRule>> └─ hasMoved: bool
 └─ traits: { isRoyal, canCastleWith, isPromotable }
```

- `PieceDefinition` describe **un tipo** de pieza (una sola instancia compartida por tipo).
- `Piece` es una pieza concreta sobre el tablero (tipo + color + estado mínimo).
- Los **traits** reemplazan los `if (tipo == Rey)`. Por ejemplo, la detección de jaque busca la pieza
  con `isRoyal` en lugar de preguntar "¿es un rey?".

### 2.3 Reglas de movimiento (Strategy)

```cpp
class MovementRule {                      // interfaz
public:
    virtual ~MovementRule() = default;
    virtual std::vector<Move> candidateMoves(const MoveContext&) const = 0;     // pseudo-legales
    virtual std::vector<Position> attackedSquares(const MoveContext&) const = 0; // para detectar jaque
};
```

`MoveContext` agrupa lo que una regla necesita leer: el `GameState` (tablero, turno, casilla de al
paso), la posición de origen y la pieza que está ahí. Es de solo lectura.

Reglas de diseño que surgieron de la revisión (misión 1.7, ver `decisiones.md` D-06 a D-09):
- Ninguna regla asume un tablero de 8×8: los bordes salen de `board.dimensions()`, el doble avance
  del peón usa `Piece::hasMoved()` (no "estar en la fila 2") y la promoción ocurre en la última fila
  de las dimensiones reales.
- `PawnRule` marca el `Move` como `Promotion` **sin elegir la pieza**. La pieza la elige el jugador
  (`makeMove(..., promotion)`) y la valida `RulesEngine::canPromoteTo`, que consulta el trait
  `isPromotable` del registro. Así, una pieza nueva promovible no exige tocar `PawnRule`.
- `CastlingRule` solo mira la geometría (rey y torre sin mover, casillas intermedias vacías). La
  condición "no enrocar en jaque ni atravesando casillas atacadas" la aplica `LegalMoveFilter`,
  porque es una cuestión de jaque y el jaque vive en un solo lugar.

| Regla | Comportamiento | Usada por |
|---|---|---|
| `SlidingRule(directions)` | Avanza en cada dirección hasta chocar. | Torre (ortogonales), Alfil (diagonales), Reina (ambas) |
| `LeaperRule(offsets)` | Salta a cada destino fijo. | Caballo (L), Rey (8 vecinas) |
| `PawnRule` | Avance simple/doble, captura diagonal, al paso; marca la promoción al llegar a la última fila. | Peón |
| `CastlingRule` | Genera el enroque si el rey y una pieza `canCastleWith` no se movieron y las casillas intermedias están libres. | Rey |

`SlidingRule` y `LeaperRule` atacan las mismas casillas a las que se mueven, así que generan sus
jugadas con la función libre `movesOntoReachableSquares` (`RuleHelpers.hpp`): casilla vacía → `Normal`,
enemiga → `Capture`, propia → se descarta. `attackedSquares` incluye la casilla de la primera pieza
propia que corta la línea (está *defendida*), así el rey rival no puede capturarla.

**Por qué `attackedSquares` está separado de `candidateMoves`:** el peón ataca en diagonal pero avanza
recto, y el enroque mueve al rey pero no "ataca". Separarlos evita casos especiales en la detección de jaque.

### 2.4 Registro y fábrica de piezas (Factory + Registry)

- `PieceRegistry`: mapa `PieceTypeId → shared_ptr<const PieceDefinition>`. Se le **registran** definiciones.
- `StandardPieces::registerAll(PieceRegistry&)`: registra las 6 piezas clásicas y compone sus reglas.
- `PieceFactory`: crea `Piece` a partir de `(PieceTypeId, Color)` consultando el registro.

**Extensión en la defensa: "agregar el Arzobispo (alfil + caballo)"**
```cpp
// Pseudocódigo ilustrativo (C++17: sin designated initializers)
PieceTraits traits;
traits.isPromotable = true;
registry.add(makeDefinition("archbishop", 'A',
    { std::make_unique<SlidingRule>(Directions::diagonals()),
      std::make_unique<LeaperRule>(Offsets::knightJumps()) },
    traits));
```
Más una línea en la disposición inicial. **Ninguna clase existente se modifica.**

Helpers sin estado: `Directions::orthogonals()`, `diagonals()`, `all()` y `knightJumps()` devuelven
`vector<Offset>` para componer reglas. Los ids clásicos son constantes `StandardPieces::kPawn`, `kRook`…

### 2.5 Tablero y estado

| Clase | Responsabilidad |
|---|---|
| `Board` | Grilla `dimensions` de `std::optional<Piece>`. `pieceAt`, `place`, `remove`, `move`. Copiable (por valor) para simular jugadas. Sin reglas de juego. |
| `StartingPositionProvider` (interfaz, Strategy) | `Board createInitialBoard(const PieceFactory&)`. Implementación: `StandardStartingPosition`. Otra variante (10×10, Chess960) = otra clase. |
| `GameState` | `Board` + turno (`Color`) + casilla de al paso (`optional<Position>`). Snapshot copiable (sirve de Memento). Los derechos de enroque **no** se guardan aparte: se derivan de `Piece::hasMoved()` (D-06). |
| `GameStatus` | `enum class { InProgress, Check, Checkmate, Stalemate }`. Lo calcula `GameStatusEvaluator`. |

### 2.6 Motor de reglas

| Clase | Responsabilidad |
|---|---|
| `MoveGenerator` | Recorre las reglas de la pieza y junta los movimientos **pseudo-legales**. |
| `AttackDetector` | `isSquareAttacked(state, position, byColor)` y `isInCheck(state, color)` (busca la pieza `isRoyal`). |
| `MoveApplier` | Aplica un `Move` a un `GameState` y devuelve el nuevo estado: mueve las piezas, resuelve la captura al paso, el enroque y la promoción, y actualiza derechos y turno. Es puro: no muta la entrada. |
| `LegalMoveFilter` | Descarta los movimientos pseudo-legales que dejan al propio rey en jaque (los aplica sobre una copia y consulta `AttackDetector`). |
| `GameStatusEvaluator` | Calcula el estado de la partida para el bando que mueve. Hoy: `InProgress` o `Check` (solo necesita el `AttackDetector`). En la fase 7 recibe `MoveGenerator` y `LegalMoveFilter` para `Checkmate` y `Stalemate`. |
| `RulesEngine` | Fachada del dominio: `legalMoves(state, from)`, `apply(state, move)`, `status(state)`. Crea y posee las clases anteriores; no se copia. `legalMoves` no mira el turno (lo valida la aplicación, D-14). En la fase 7 se construye con un `PieceRegistry&`, crea la `PieceFactory` para la promoción y suma `canPromoteTo(id)` (D-13). |

Pipeline de validación de una jugada:
```
candidatos (MovementRules) → pseudo-legales (MoveGenerator) → legales (LegalMoveFilter) → ¿contiene la jugada pedida?
```

---

## 3. Catálogo de patrones (dónde y por qué)

> Cada entrada tiene su contraparte en `docs/decisiones.md` con el formato qué / por qué / cuándo romperlo.
> En el código, el header de la clase lleva el comentario `// Patrón: <nombre> — ver docs/arquitectura.md §3.x`.

### 3.1 Arquitectura hexagonal (Ports & Adapters)
- **Dónde:** `src/core/ports/` (`GameService`, `GameObserver`) y `src/adapters/*`.
- **Por qué:** el núcleo queda testeable y aislado; la UI se cambia sin tocar las reglas.

### 3.2 Strategy — reglas de movimiento
- **Dónde:** `MovementRule` y sus implementaciones (`SlidingRule`, `LeaperRule`, `PawnRule`, `CastlingRule`).
- **Por qué:** cada forma de moverse es un algoritmo intercambiable. La Reina no duplica la lógica de
  Torre y Alfil: compone dos `SlidingRule` (o una con 8 direcciones).

### 3.3 Composición sobre herencia — piezas
- **Dónde:** `PieceDefinition` (tipo + reglas + traits) y `Piece` (valor).
- **Por qué:** no hay jerarquía `Piece → Rook/Knight/...`. Una pieza nueva es una combinación de reglas
  ya existentes y no hace falta ninguna subclase. Es lo que la consigna evalúa explícitamente.

### 3.4 Factory + Registry — creación de piezas
- **Dónde:** `PieceRegistry`, `PieceFactory`, `StandardPieces`.
- **Por qué:** centraliza la creación y cumple OCP: agregar un tipo = registrar, no editar un `switch`.

### 3.5 Strategy — disposición inicial
- **Dónde:** `StartingPositionProvider`, `StandardStartingPosition`.
- **Por qué:** cambiar el tamaño o la disposición del tablero sin modificar `Board` ni el motor.

### 3.6 Facade — motor de reglas
- **Dónde:** `RulesEngine`.
- **Por qué:** la capa de aplicación usa una interfaz simple y no conoce el pipeline interno
  (generador, filtro, detector).

### 3.7 State — estado de la partida
- **Dónde:** `GamePhase` (interfaz, `tryMove(game, from, to, promotion)`) con `InProgressPhase`
  (delega en `ChessGame::performMove`) y `GameOverPhase` (rechaza con `MoveError::GameOver`) dentro
  de `ChessGame`. La fase no se guarda en el historial: se deriva del `GameStatus` después de cada
  jugada, undo o redo, así deshacer un mate vuelve a `InProgressPhase` sin casos especiales.
- **Por qué:** el comportamiento cambia según el estado: en `GameOverPhase`, `makeMove` se rechaza
  sin llenar `ChessGame` de `if (terminado)`. Las transiciones quedan explícitas (diagrama de estados).

### 3.8 Command + Memento — deshacer/rehacer
- **Dónde:** `MoveCommand` (`execute`/`undo`) y `MoveHistory` (pilas undo/redo) en `application/`.
  El comando guarda el `GameState` previo como Memento.
- **Por qué:** cada jugada es un objeto reversible. Guardar el snapshot completo es simple y correcto,
  a costa de memoria, que acá es despreciable.

### 3.9 Observer — notificación a la UI
- **Dónde:** puerto de salida `GameObserver` (`onMoveMade`, `onCheck`, `onGameOver`, `onUndo`).
- **Por qué:** el núcleo avisa sin saber quién escucha. Pueden suscribirse varios adaptadores
  (por ejemplo, un logger y la UI).

### 3.10 Inyección de dependencias manual + Composition Root
- **Dónde:** constructores de `ChessGame`, `RulesEngine`, etc. El armado se hace en `app/main_*.cpp`.
- **Por qué:** las dependencias quedan visibles y en los tests se inyectan fakes. No hay singletons
  ni estado global.

---

## 4. Capa de aplicación (`src/core/application`, namespace `chess::application`)

| Clase | Responsabilidad |
|---|---|
| `ChessGame` | Implementa el puerto `GameService`. Tiene el `GameState` actual, el `RulesEngine`, el `MoveHistory`, la fase actual (`GamePhase`) y los observers. |
| `MoveCommand` | Una jugada ejecutada; guarda el estado anterior para deshacerla. |
| `MoveHistory` | Pilas de deshacer y rehacer. Una jugada nueva vacía la pila de rehacer. |
| `GamePhase` y sus implementaciones | State (§3.7). |

## 5. Puertos (`src/core/ports`, namespace `chess::ports`)

```cpp
// Puerto de entrada (driving): lo que los adaptadores pueden pedirle al juego.
class GameService {
public:
    virtual ~GameService() = default;
    virtual MoveResult makeMove(Position from, Position to,
                                std::optional<PieceTypeId> promotion = std::nullopt) = 0;
    virtual std::vector<Move> legalMovesFrom(Position from) const = 0;
    virtual const GameState& state() const = 0;
    virtual GameStatus status() const = 0;
    virtual bool undo() = 0;
    virtual bool redo() = 0;
    virtual void addObserver(GameObserver&) = 0;
};

// Puerto de salida (driven): eventos que el juego emite.
class GameObserver {
public:
    virtual ~GameObserver() = default;
    virtual void onMoveMade(const Move&, const GameState&) = 0;
    virtual void onCheck(Color colorInCheck) = 0;
    virtual void onGameOver(GameStatus result) = 0;
    virtual void onUndoRedo(const GameState&) = 0;
};
```

Lo implementado hasta la fase 6 es el subconjunto que el MVP usa: `makeMove(from, to)` sin
`promotion`, sin `undo`/`redo` y un `GameObserver` con solo `onMoveMade` y `onCheck`. El resto se suma
en la misión de la fase 7 que lo necesita (D-13): `promotion` en la 7.2, `onGameOver` en la 7.1,
`undo`/`redo`/`onUndoRedo` en la 7.5. `GameService::legalMovesFrom` devuelve vacío si la pieza no es
del bando que mueve: son las jugadas que `makeMove` aceptaría (la UI las usa para resaltar).

`MoveResult` es un value object creado con `MoveResult::success(move)` o `MoveResult::failure(error)`
(`ok()`, `error()`, `move()`), donde `MoveError` puede ser
`NoPieceAtOrigin`, `NotYourTurn`, `IllegalMove` (y en la fase 7, `PromotionRequired` y `GameOver`).

**Manejo de errores:** en el flujo de juego **no se usan excepciones**. Una jugada inválida es un
resultado esperado y se devuelve en `MoveResult`. Las excepciones quedan para errores de programación
(por ejemplo, una `Position` fuera del tablero en el constructor del `Board`).

## 6. Adaptadores

### 6.1 Consola (`src/adapters/console`)
- `ConsoleUI`: loop de lectura sobre streams inyectados. Comandos: `e2 e4`, `moves e2`, `help`, `quit`
  (y en la fase 7: `e7 e8 q` para la promoción, `undo`, `redo`). Traduce cada `MoveError` a un mensaje.
  No dibuja: el tablero lo dibuja el `ConsoleRenderer` cuando el juego le avisa.
- `ConsoleRenderer`: implementa `GameObserver` y dibuja el tablero en ASCII.
- `AlgebraicNotation`: convierte `"e2"` ↔ `Position`. Es una cuestión de presentación, por eso **no** está en el dominio.

### 6.2 raylib (`src/adapters/raylib`)
- `RaylibUI`: ventana, click para elegir origen y destino, resalta los movimientos legales (`legalMovesFrom`).
- Piezas dibujadas con letras o formas simples (minimalista, sin assets obligatorios).
- Teclas: `U` deshace y `R` rehace.

## 7. Flujo de una jugada (resumen del diagrama de secuencia)

```
ConsoleUI → GameService.makeMove(from, to)
  ChessGame → fase actual: ¿acepta jugadas?            (State)
  ChessGame → RulesEngine.legalMoves(state, from)      (Facade → Strategy)
  ¿la jugada está entre las legales? no → MoveResult{IllegalMove}
  sí → MoveCommand(execute) → MoveHistory.push         (Command + Memento)
  ChessGame → RulesEngine.status(newState) → ¿cambia la fase?
  ChessGame → observers.onMoveMade / onCheck / onGameOver   (Observer)
  ← MoveResult{ok}
```

## 8. Estrategia de tests

- Todo lo de `src/core/` tiene tests en `tests/core/`, espejando la estructura (`tests/core/domain/SlidingRuleTest.cpp`).
- Los tests construyen posiciones a mano con un helper (`BoardBuilder`) en lugar de jugar partidas enteras.
- Casos mínimos por regla: tablero vacío, bloqueo propio, captura enemiga, borde del tablero.
- Casos de jaque: jaque simple, pieza clavada que no puede moverse, el rey no puede entrar en jaque,
  mate del pastor, ahogado clásico.
- `ChessGame` se testea con un `FakeObserver` que registra los eventos recibidos.
- Tests de extensibilidad: registrar una pieza inventada y verificar sus movimientos; tablero de 10×10.
  Funcionan como ensayo de la defensa individual.

---

## 9. Revisión de extensibilidad (misión 1.7)

Recorrido de los diagramas con las dos preguntas de la defensa individual.

**¿Se puede agregar una pieza nueva sin modificar ninguna clase?** Sí.
1. Crear un `PieceDefinition` con un `PieceTypeId` nuevo, un símbolo, reglas existentes (o una
   `MovementRule` nueva, que es una clase **nueva**) y sus `PieceTraits`.
2. Registrarlo en el `PieceRegistry` desde el composition root.
3. Ubicarlo en un `StartingPositionProvider` (uno nuevo, o el estándar si se acepta tocar la disposición).
- `MoveGenerator` y `AttackDetector` recorren las reglas de la pieza sin preguntar el tipo.
- El jaque usa `isRoyal`; la promoción, `isPromotable` (D-08); el enroque, `canCastleWith`.
- Las UIs dibujan `Piece::symbol()`, así que la pieza nueva aparece sin cambios.

**¿Y un tablero de 10×10?** Sí.
1. Nuevo `StartingPositionProvider` que crea `Board(BoardDimensions{10, 10})`.
2. Inyectarlo en el composition root.
- Las reglas usan `BoardDimensions` para los bordes y la última fila; el peón usa `hasMoved` (D-09).
- `AlgebraicNotation::parse` recibe las dimensiones, así que acepta `j10`. Los adaptadores dibujan
  según `state().board().dimensions()`.

**Puntos que se corrigieron durante la revisión** (todos registrados en `decisiones.md`): derechos de
enroque derivados de `hasMoved` (D-06), condición de jaque del enroque en el filtro (D-07), promoción
validada por trait (D-08), reglas independientes del tamaño (D-09) y fase derivada del estado (D-10).
