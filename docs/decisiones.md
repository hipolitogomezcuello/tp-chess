# Decisiones de diseño — qué / por qué / cuándo se rompería

> Entregable de la materia (consigna §6). Cada decisión **propia del grupo** se registra acá.
> Formato fijo de tres preguntas. Las decisiones se numeran y **no se borran**: si una se revierte,
> se marca como *Reemplazada por D-XX* y se agrega la nueva.
>
> Plantilla:
>
> ```markdown
> ## D-XX — Título corto
> **Estado:** vigente | reemplazada por D-YY
> **Qué:** la decisión, en una o dos oraciones.
> **Por qué:** el problema que resuelve y las alternativas descartadas.
> **Cuándo se rompería:** bajo qué condiciones dejaría de ser una buena decisión.
> **Dónde en el código:** clases o archivos afectados.
> ```

---

## D-01 — Arquitectura hexagonal (Ports & Adapters)
**Estado:** vigente
**Qué:** el núcleo (`src/core/`) tiene el dominio y los casos de uso y se comunica con el exterior solo
a través de los puertos `GameService` (entrada) y `GameObserver` (salida). Las UIs son adaptadores.
**Por qué:** la consigna exige un núcleo sin dependencias externas y testeable sin infraestructura.
Además, tener dos UIs (consola y raylib) sobre el mismo puerto demuestra que el desacople es real.
Se descartó una arquitectura en capas clásica porque permite que la UI y el dominio se acoplen sin que nadie lo note.
**Cuándo se rompería:** en un script de una sola UI y una sola vez, los puertos son ceremonia innecesaria.
También si el rendimiento exigiera que la UI accediera a estructuras internas, como un motor de búsqueda que lee el tablero en crudo.
**Dónde en el código:** `src/core/ports/`, `src/adapters/`, `app/`.

## D-02 — Piezas por composición de reglas, no por herencia
**Estado:** vigente
**Qué:** una pieza es un `PieceDefinition` (id + símbolo + lista de `MovementRule` + traits). No existen
subclases `Rook`, `Knight`, etc.
**Por qué:** la Reina es Torre + Alfil. Con herencia, se duplica código o se cae en herencia múltiple.
Con composición, una pieza nueva (por ejemplo, el Arzobispo = Alfil + Caballo) se arma registrando
reglas que ya existen, sin modificar ninguna clase (OCP). Es lo que puede pedirse en la defensa.
**Cuándo se rompería:** si las piezas tuvieran comportamiento muy distinto al movimiento (habilidades,
estados internos complejos), los traits y las reglas no alcanzarían y convendría otra abstracción.
Si nunca fuera a haber piezas nuevas, la herencia sería más directa de leer.
**Dónde en el código:** `PieceDefinition`, `Piece`, `MovementRule` y sus implementaciones.

## D-03 — `PieceTypeId` abierto (string) en lugar de `enum class`
**Estado:** vigente
**Qué:** el tipo de pieza se identifica con un id textual registrado en un `PieceRegistry`.
**Por qué:** un `enum` cerrado obliga a modificarlo, y a modificar cada `switch` que lo usa, para agregar
una pieza. Eso viola OCP.
**Cuándo se rompería:** si el rendimiento importara (comparar strings es más lento que comparar enteros) o
si se quisiera que el compilador verifique la exhaustividad de los casos.
**Dónde en el código:** `PieceTypeId`, `PieceRegistry`.

## D-04 — Validación en pipeline: pseudo-legal → legal
**Estado:** vigente
**Qué:** las reglas de movimiento generan movimientos sin considerar el jaque, y un `LegalMoveFilter`
aparte descarta los que dejan al propio rey en jaque simulando la jugada sobre una copia del estado.
**Por qué:** cada regla se ocupa solo de su geometría (SRP). La lógica de jaque vive en un único lugar
y aplica a cualquier pieza, incluso a las que se agreguen en el futuro.
**Cuándo se rompería:** en un motor competitivo, copiar el estado por cada movimiento es caro. Ahí
convendría hacer y deshacer la jugada sobre el mismo estado, o usar bitboards.
**Dónde en el código:** `MoveGenerator`, `LegalMoveFilter`, `AttackDetector`.

## D-05 — Jugadas inválidas como resultado, no como excepción
**Estado:** vigente
**Qué:** `makeMove` devuelve un `MoveResult` con el motivo del error y no lanza excepciones.
**Por qué:** una jugada ilegal es parte normal del flujo del juego, no una situación excepcional. Además,
el adaptador puede mostrar el motivo concreto.
**Cuándo se rompería:** si el núcleo se expusiera por una API donde la convención sea usar excepciones o
códigos HTTP, el adaptador traduciría. El núcleo no cambiaría.
**Dónde en el código:** `MoveResult`, `GameService::makeMove`.

## D-06 — Derechos de enroque derivados de `Piece::hasMoved`
**Estado:** vigente
**Qué:** `GameState` no guarda derechos de enroque. `CastlingRule` los deduce de que el rey y la pieza
`canCastleWith` no se hayan movido (`Piece::hasMoved()`).
**Por qué:** es un solo dato en vez de dos que hay que mantener sincronizados. Además funciona en
cualquier tablero: no hay que saber de antemano en qué casillas están las torres. Se descartó guardar
cuatro flags (`K`, `Q`, `k`, `q` como en FEN) porque asumen el tablero de 8×8 con dos torres por bando.
**Cuándo se rompería:** si hubiera que importar posiciones FEN donde los derechos no coinciden con
`hasMoved` (una torre que nunca se movió pero perdió el derecho), o en variantes como Chess960, donde
los derechos dependen de la posición inicial.
**Dónde en el código:** `Piece`, `CastlingRule`, `MoveApplier` (marca `hasMoved`).

## D-07 — La condición de jaque del enroque la aplica `LegalMoveFilter`
**Estado:** vigente
**Qué:** `CastlingRule` genera el enroque solo por geometría (piezas sin mover, casillas libres). Que el
rey no esté en jaque ni atraviese casillas atacadas lo verifica `LegalMoveFilter` para los `Move` de
tipo `Castling`.
**Por qué:** si `CastlingRule` consultara al `AttackDetector`, una regla de movimiento dependería del
motor que la usa (dependencia circular: el detector recorre las reglas). Así todo lo relativo al jaque
sigue en un único lugar (D-04).
**Cuándo se rompería:** si aparecieran muchas jugadas especiales con condiciones de jaque propias, el
filtro acumularía casos por `MoveKind` y convendría que cada regla aporte su propia validación de
legalidad (por ejemplo, una interfaz `LegalityCheck` inyectada).
**Dónde en el código:** `CastlingRule`, `LegalMoveFilter`.

## D-08 — Promoción: la regla la marca, el jugador elige y el registro valida
**Estado:** vigente
**Qué:** `PawnRule` genera un `Move` de tipo `Promotion` sin pieza elegida. `ChessGame` lo completa con
el `promotion` que pide el jugador y `RulesEngine::canPromoteTo` lo acepta si la definición tiene el
trait `isPromotable`.
**Por qué:** el peón no necesita conocer la lista de piezas. Si se agrega una pieza nueva promovible
(el Arzobispo), alcanza con su trait: no se toca `PawnRule`. Se descartó que el generador produzca un
`Move` por cada pieza posible, porque multiplica las jugadas legales y obliga a la regla a ver el registro.
**Cuándo se rompería:** si la promoción dependiera del contexto (por ejemplo, "solo a piezas que ya fueron
capturadas", como en algunas variantes), haría falta una política de promoción inyectable.
**Dónde en el código:** `PawnRule`, `Move::withPromotion`, `RulesEngine::canPromoteTo`, `ChessGame`.

## D-09 — Ninguna regla asume un tablero de 8×8
**Estado:** vigente
**Qué:** los límites salen siempre de `BoardDimensions`; el doble avance del peón usa `hasMoved()` en
lugar de "estar en la segunda fila"; la promoción ocurre en la última fila de las dimensiones reales.
**Por qué:** la defensa puede pedir un tablero de 10×10. Con esto basta escribir otro
`StartingPositionProvider`: ni `Board`, ni las reglas, ni el motor cambian.
**Cuándo se rompería:** si una variante necesitara tableros no rectangulares o con casillas
inutilizables. `BoardDimensions::contains` dejaría de alcanzar y habría que pasar a una "forma" de
tablero (interfaz `BoardShape`).
**Dónde en el código:** `BoardDimensions`, `PawnRule`, `SlidingRule`, `LeaperRule`, `StartingPositionProvider`.

## D-10 — La fase de la partida se deriva del `GameStatus`
**Estado:** vigente
**Qué:** `ChessGame` no guarda la fase (`GamePhase`) en el historial. Después de cada jugada, undo o redo
recalcula el `GameStatus` y elige `InProgressPhase` o `GameOverPhase`.
**Por qué:** el `GameState` ya alcanza para saber si la partida terminó. Derivar la fase evita
inconsistencias: deshacer un mate vuelve automáticamente a una partida en curso, sin transiciones
especiales.
**Cuándo se rompería:** si la partida pudiera terminar por razones que no están en el tablero
(abandono, tiempo agotado, tablas acordadas). Ahí la fase tendría estado propio y habría que guardarla.
**Dónde en el código:** `ChessGame::updatePhase`, `GamePhase`.

## D-11 — PlantUML como UML vivo versionado en el repo
**Estado:** vigente
**Qué:** los diagramas son archivos `.puml` en `docs/diagramas/`, editados en el mismo commit que el
código que describen. Lo que todavía no está implementado (fase 7 en adelante) se marca `<<opcional>>`.
**Por qué:** la consigna exige que el UML sea consistente con el código durante todo el desarrollo. Un
diagrama en texto se revisa en el diff igual que el código; uno en una herramienta gráfica se desactualiza
sin que nadie lo note. Se descartó generar el UML desde el código porque mezcla detalles de
implementación y pierde las notas de diseño.
**Cuándo se rompería:** si el grupo no tuviera disciplina para actualizarlos (el texto no se valida solo
contra el código) o si el proyecto creciera tanto que convenga generarlos automáticamente.
**Dónde en el código:** `docs/diagramas/*.puml`, `CLAUDE.md` (regla 5).

## D-12 — CMake con dependencias vía `FetchContent`
**Estado:** vigente
**Qué:** el build es CMake ≥ 3.20 y GoogleTest (y más adelante raylib) se descarga con `FetchContent`
en una versión fija. El núcleo es la librería `chess_core`, que no enlaza nada.
**Por qué:** funciona igual en MSVC, MinGW, gcc y clang sin instalar paquetes a mano, y la separación en
targets hace que el compilador haga cumplir la regla de dependencias: si `chess_core` incluyera algo de
raylib, no compilaría. Se descartaron vcpkg y Conan porque agregan una herramienta más que todo el grupo
tendría que instalar.
**Cuándo se rompería:** sin conexión a internet en la primera configuración, o si las dependencias fueran
muchas y pesadas (ahí conviene un gestor de paquetes con caché binaria).
**Dónde en el código:** `CMakeLists.txt`, `src/core/CMakeLists.txt`, `tests/CMakeLists.txt`.

## D-13 — Las dependencias se agregan cuando la fase las necesita
**Estado:** vigente
**Qué:** las clases del motor se implementan con las dependencias que usan hoy. `MoveApplier` no recibe la
`PieceFactory`, `RulesEngine` no recibe el `PieceRegistry` (ni tiene `canPromoteTo`) y
`GameStatusEvaluator` solo recibe el `AttackDetector`. Las tres crecen en la fase 7 (promoción, mate y
ahogado). El diagrama muestra lo implementado y anota lo que viene.
**Por qué:** un parámetro guardado y nunca usado es código muerto que confunde al lector y que los tests no
pueden cubrir (YAGNI, CLAUDE.md regla 9). Se descartó implementar ya la promoción "porque era fácil":
saltea el orden del roadmap y deja funcionalidad opcional sin su UI.
**Cuándo se rompería:** si cambiar esas firmas en la fase 7 obligara a tocar muchos clientes. Hoy el único
cliente de `RulesEngine` es el composition root (y `ChessGame`, que lo recibe armado), así que el costo es
una línea.
**Dónde en el código:** `MoveApplier`, `RulesEngine`, `GameStatusEvaluator`.

## D-14 — El turno lo valida la aplicación; el filtro usa el color de la pieza
**Estado:** vigente
**Qué:** `RulesEngine::legalMoves(state, from)` devuelve las jugadas legales de la pieza en `from` sea o no
su turno. `LegalMoveFilter` protege al rey del color **de la pieza que mueve**, no de `sideToMove`. Rechazar
una jugada fuera de turno es responsabilidad de `ChessGame` (`MoveError::NotYourTurn`).
**Por qué:** el turno es una regla de flujo de la partida (aplicación), no de geometría (dominio). Además,
así el motor responde bien consultas como "¿qué podría hacer esta pieza?" para resaltar en la UI, y los
tests de reglas no necesitan ajustar el turno.
**Cuándo se rompería:** si una variante tuviera turnos no alternados (p. ej. dos jugadas seguidas), el
turno pasaría a ser una política del dominio y `MoveApplier` dejaría de alternarlo a ciegas.
**Dónde en el código:** `RulesEngine::legalMoves`, `LegalMoveFilter::filter`, `ChessGame` (fase 5).

## D-15 — `attackedSquares` incluye la casilla de la pieza propia que corta la línea
**Estado:** vigente
**Qué:** una pieza "ataca" también las casillas ocupadas por piezas de su color hasta la primera que la
bloquea (las *defiende*). `candidateMoves` descarta esas casillas; `attackedSquares` no.
**Por qué:** con esto `LegalMoveFilter` impide que el rey capture una pieza defendida sin ningún caso
especial: después de la captura, la pieza defensora ataca esa casilla. Se descartó la alternativa de
excluirlas, que obligaba a un chequeo aparte de "pieza defendida".
**Cuándo se rompería:** si una regla atacara casillas distintas de las que alcanza y las piezas propias
cambiaran ese alcance de otra forma (p. ej. piezas que saltan sobre propias pero no sobre enemigas). Esa
regla tendría que implementar su propio `attackedSquares`.
**Dónde en el código:** `SlidingRule`, `LeaperRule`, `RuleHelpers`, `AttackDetector`.

## D-16 — Observer como puerto de salida
**Estado:** vigente
**Qué:** el núcleo avisa lo que pasa en la partida (jugada hecha, jaque) a través de la interfaz
`GameObserver`, definida en `ports/`. Los adaptadores la implementan y se suscriben con
`GameService::addObserver`. `ChessGame` guarda referencias no propietarias a los observers.
**Por qué:** el núcleo no puede conocer a la UI (regla de dependencias), pero la UI tiene que enterarse de
los cambios. Con Observer la dependencia se invierte: el núcleo define el contrato y la UI lo cumple.
Pueden escuchar varios a la vez (la UI y un logger). Se descartó que la UI consulte el estado después
de cada `makeMove` (polling): funciona con una sola UI, pero deja afuera a quien no hizo la jugada y
obliga a cada adaptador a deducir qué cambió (¿hubo jaque?).
**Cuándo se rompería:** si los eventos tuvieran que cruzar hilos o procesos (UI en otro hilo, red), la
notificación síncrona por referencia ya no sirve y haría falta una cola de eventos. También si un
observer se destruyera antes que el juego: hoy la regla es que el composition root los crea antes y los
destruye después.
**Dónde en el código:** `ports/GameObserver.hpp`, `ChessGame::addObserver`, `ConsoleRenderer`.

## D-17 — Inyección de dependencias manual y composition root
**Estado:** vigente
**Qué:** cada clase recibe sus dependencias por constructor (`ChessGame(rules, initialState)`,
`ConsoleUI(game, in, out)`, `ConsoleRenderer(out)`). El grafo completo se arma a mano en
`app/main_console.cpp`, que es el único archivo que conoce dominio, aplicación y adaptador.
**Por qué:** las dependencias quedan a la vista en las firmas y en los tests se reemplazan sin trucos
(`FakeObserver`, streams en memoria en lugar de `std::cin`/`std::cout`). Las extensiones de la defensa
(pieza nueva, otro tablero) se enchufan en un solo lugar. Se descartaron los singletons y el estado
global porque esconden dependencias y acoplan los tests entre sí, y un contenedor de DI porque para
media docena de objetos es más ceremonia que ayuda.
**Cuándo se rompería:** con decenas de servicios y varias configuraciones (por ejemplo, según un archivo
de configuración), escribir el armado a mano se vuelve repetitivo y propenso a errores; ahí conviene una
fábrica por configuración o un contenedor. También exige cuidar los tiempos de vida a mano: quien
recibe una referencia (`ChessGame` → `RulesEngine`, observers) asume que el composition root la creó
antes y la destruye después.
**Dónde en el código:** `app/main_console.cpp`, constructores de `ChessGame`, `ConsoleUI`, `ConsoleRenderer`.

<!-- Próximas decisiones (las completan los agentes cuando las implementen):
     Command + Memento para deshacer/rehacer (snapshot completo vs. delta)
     State para las fases de la partida (complementa D-10)
     raylib como UI gráfica (frente a SFML o Qt) -->
