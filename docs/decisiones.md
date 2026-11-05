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

## Índice

| # | Decisión | Tema |
|---|---|---|
| D-01 | Arquitectura hexagonal (Ports & Adapters) | Arquitectura |
| D-02 | Piezas por composición de reglas, no por herencia | Extensibilidad |
| D-03 | `PieceTypeId` abierto (string) en lugar de `enum class` | Extensibilidad |
| D-04 | Validación en pipeline: pseudo-legal → legal | Reglas |
| D-05 | Jugadas inválidas como resultado, no como excepción | Puertos |
| D-06 | Derechos de enroque derivados de `Piece::hasMoved` | Reglas |
| D-07 | La condición de jaque del enroque la aplica `LegalMoveFilter` | Reglas |
| D-08 | Promoción: la regla la marca, el jugador elige y el registro valida | Extensibilidad |
| D-09 | Ninguna regla asume un tablero de 8×8 | Extensibilidad |
| D-10 | La fase de la partida se deriva del `GameStatus` | Aplicación |
| D-11 | PlantUML como UML vivo versionado en el repo | Proceso |
| D-12 | CMake con dependencias vía `FetchContent` | Build |
| D-13 | Las dependencias se agregan cuando la fase las necesita | Proceso |
| D-14 | El turno lo valida la aplicación; el filtro usa el color de la pieza | Aplicación |
| D-15 | `attackedSquares` incluye la casilla de la pieza propia que corta la línea | Reglas |
| D-16 | Observer como puerto de salida | Puertos |
| D-17 | Inyección de dependencias manual y composition root | Arquitectura |
| D-18 | State para la fase de la partida | Aplicación |
| D-19 | El puerto ofrece las piezas de promoción; la UI no conoce los tipos | Puertos |
| D-20 | Deshacer con Command + Memento de estado completo | Aplicación |
| D-21 | raylib como UI gráfica | Adaptadores |
| D-22 | La lógica de la UI gráfica separada de raylib (Humble Object) | Adaptadores |
| D-23 | La notación algebraica vive en el adaptador, no en el dominio | Adaptadores |
| D-24 | Alcance opcional que se dejó afuera | Alcance |

**Cómo leerlo en la defensa:** D-02, D-03, D-08 y D-09 explican por qué agregar una pieza o cambiar
el tablero no toca código existente; D-01, D-16, D-17 y D-22, por qué la UI se cambia sin tocar el
núcleo. El paso a paso de esas extensiones está en [`guia-defensa.md`](guia-defensa.md).

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
código que describen. Mientras hubo partes sin implementar se marcaron `<<opcional>>`; al cerrar la
fase 8 ya no queda ninguna.
**Por qué:** la consigna exige que el UML sea consistente con el código durante todo el desarrollo. Un
diagrama en texto se revisa en el diff igual que el código; uno en una herramienta gráfica se desactualiza
sin que nadie lo note. Se descartó generar el UML desde el código porque mezcla detalles de
implementación y pierde las notas de diseño.
**Cuándo se rompería:** si el grupo no tuviera disciplina para actualizarlos (el texto no se valida solo
contra el código) o si el proyecto creciera tanto que convenga generarlos automáticamente.
**Cómo resultó (fase 9):** la verificación clase por clase de la misión 9.2 encontró solo tres omisiones
menores (una constante y dos detalles del adaptador raylib), ninguna en las relaciones. Las imágenes de
la entrega se generan desde los `.puml` con `docs/diagramas/exportar.sh`, que solo necesita Java: usa
los motores de layout que trae PlantUML (ELK y Smetana) en lugar de Graphviz, para que cualquiera del
grupo pueda regenerarlas.
**Dónde en el código:** `docs/diagramas/*.puml`, `docs/diagramas/exportar.sh`, `CLAUDE.md` (regla 5).

## D-12 — CMake con dependencias vía `FetchContent`
**Estado:** vigente
**Qué:** el build es CMake ≥ 3.20 y GoogleTest y raylib se descargan con `FetchContent`
en una versión fija. El núcleo es la librería `chess_core`, que no enlaza nada.
**Por qué:** funciona igual en MSVC, MinGW, gcc y clang sin instalar paquetes a mano, y la separación en
targets hace que el compilador haga cumplir la regla de dependencias: si `chess_core` incluyera algo de
raylib, no compilaría. Se descartaron vcpkg y Conan porque agregan una herramienta más que todo el grupo
tendría que instalar.
**Cuándo se rompería:** sin conexión a internet en la primera configuración, o si las dependencias fueran
muchas y pesadas (ahí conviene un gestor de paquetes con caché binaria).
**Dónde en el código:** `CMakeLists.txt`, `src/core/CMakeLists.txt`, `tests/CMakeLists.txt`,
`src/adapters/raylib/CMakeLists.txt`.

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
**Cómo resultó (fase 7):** las tres firmas crecieron como estaba previsto (`MoveApplier(factory)`,
`RulesEngine(registry)`, `GameStatusEvaluator(detector, generator, filter)`) y lo mismo los puertos
(`promotion`, `undo`/`redo`, `onGameOver`, `onUndoRedo`). Los clientes de producción a tocar fueron el
composition root y `ChessGame`; el resto fueron fixtures de tests, que cambiaron una línea cada uno.
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
**Dónde en el código:** `RulesEngine::legalMoves`, `LegalMoveFilter::filter`, `ChessGame::performMove`.

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
Pueden escuchar varios a la vez (la UI y un logger). `RaylibUI` es a la vez adaptador de entrada y
observer, sin que el núcleo haya cambiado. Se descartó que la UI consulte el estado después
de cada `makeMove` (polling): funciona con una sola UI, pero deja afuera a quien no hizo la jugada y
obliga a cada adaptador a deducir qué cambió (¿hubo jaque?).
**Cuándo se rompería:** si los eventos tuvieran que cruzar hilos o procesos (UI en otro hilo, red), la
notificación síncrona por referencia ya no sirve y haría falta una cola de eventos. También si un
observer se destruyera antes que el juego: hoy la regla es que el composition root los crea antes y los
destruye después.
**Dónde en el código:** `ports/GameObserver.hpp`, `ChessGame::addObserver`, `ConsoleRenderer`, `RaylibUI`.

## D-17 — Inyección de dependencias manual y composition root
**Estado:** vigente
**Qué:** cada clase recibe sus dependencias por constructor (`ChessGame(rules, initialState)`,
`ConsoleUI(game, in, out)`, `ConsoleRenderer(out)`). El grafo completo se arma a mano en
`app/main_console.cpp` y `app/main_raylib.cpp`, los únicos archivos que conocen dominio, aplicación y
adaptador. Los dos arman el mismo grafo y solo cambia el adaptador.
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
**Dónde en el código:** `app/main_console.cpp`, `app/main_raylib.cpp`, constructores de `ChessGame`,
`ConsoleUI`, `ConsoleRenderer`, `BoardInteraction`.

## D-18 — State para la fase de la partida
**Estado:** vigente
**Qué:** `ChessGame::makeMove` delega en su fase actual (`GamePhase`): `InProgressPhase` valida y hace la
jugada (`ChessGame::performMove`) y `GameOverPhase` la rechaza con `MoveError::GameOver`. Las fases no
tienen datos; `ChessGame::updatePhase` crea la que corresponde según el `GameStatus` (D-10).
**Por qué:** el comportamiento de `makeMove` cambia según si la partida terminó, y con State ese cambio
es un objeto en lugar de un `if (terminada)` al principio de cada operación. La transición queda en un
solo lugar (`updatePhase`) y coincide con el diagrama de estados. Se descartó guardar un `enum` de fase
con un `switch` porque cada operación nueva que dependa de la fase repetiría el `switch`. `performMove`
es público en `ChessGame` pero no en el puerto `GameService`, así que los adaptadores no lo ven; se
descartó declarar a las fases `friend` porque acopla más de lo que esconde.
**Cuándo se rompería:** con dos fases y una sola operación que cambia, State es casi tanta ceremonia
como un `if`; se justifica si aparecen más fases (esperando la elección de promoción, tablas ofrecidas,
reloj en pausa) u otras operaciones que dependan de la fase. Si las fases tuvieran datos propios
(quién abandonó), habría que guardarlas y D-10 dejaría de valer.
**Dónde en el código:** `application/GamePhase.hpp`, `InProgressPhase`, `GameOverPhase`, `ChessGame`.

## D-19 — El puerto ofrece las piezas de promoción; la UI no conoce los tipos
**Estado:** vigente
**Qué:** `GameService::promotionChoices()` devuelve las `Piece` (símbolo + tipo + color) a las que puede
promover el bando que mueve, sacadas del registro por el trait `isPromotable`. La consola busca entre
ellas la que tiene el símbolo (`q`) o el id (`queen`) que escribió el jugador y manda su `PieceTypeId`.
Si la jugada promueve y no se eligió pieza, `makeMove` devuelve `PromotionRequired`.
**Por qué:** la alternativa directa era un `switch ('q' → reina, 'r' → torre…)` en el adaptador, que
viola OCP: una pieza nueva promovible (el Arzobispo) obligaría a editar cada UI. Así la UI de raylib
también puede dibujar las opciones con `symbol()` sin saber cuáles son. Se devuelve `Piece` y no solo
`PieceTypeId` porque la UI necesita el símbolo y el puerto no expone el registro. Pedir la pieza como
parámetro de `makeMove` (en lugar de una segunda llamada "elegir pieza") mantiene la jugada atómica y
sin un estado intermedio "esperando promoción".
**Cuándo se rompería:** si dos piezas promovibles compartieran símbolo, la letra sería ambigua (la
consola toma la primera; el id completo siempre funciona). Si la UI quisiera preguntar la pieza
*después* de soltar el peón con el mouse, alcanza con que el adaptador espere antes de llamar a
`makeMove` (como hace `RaylibUI`); si la elección tuviera que vivir en el núcleo, habría que agregar
una fase "esperando promoción" (D-18).
**Dónde en el código:** `GameService::promotionChoices`, `RulesEngine::promotionChoices`/`canPromoteTo`,
`ChessGame::withPromotionChoice`, `ConsoleUI::findPromotionChoice`.

## D-20 — Deshacer con Command + Memento de estado completo
**Estado:** vigente
**Qué:** cada jugada válida se ejecuta como un `MoveCommand`, que antes de aplicarla guarda una copia
del `GameState` entero (el Memento: tablero, turno y casilla de al paso). `undo` restaura esa copia;
rehacer vuelve a ejecutar el comando. `MoveHistory` guarda los comandos en dos pilas y una jugada nueva
vacía la de rehacer.
**Por qué:** guardar el estado completo hace que deshacer sea trivialmente correcto para cualquier
jugada (enroque, al paso, promoción, una pieza inventada) sin escribir la operación inversa de cada
una: no hay que "desenrocar" ni "despromover". Además `GameState` ya era un value object copiable
(lo usa `LegalMoveFilter`). Se descartó guardar solo un delta (qué pieza se movió y qué se capturó)
porque obliga a `MoveApplier` a tener una inversa por cada `MoveKind` y cada caso nuevo es un bug
posible. Rehacer re-ejecuta en lugar de guardar también el estado siguiente porque el motor es
determinista y así el comando tiene un solo Memento.
**Cuándo se rompería:** si el estado fuera grande (tableros enormes, historial de miles de jugadas
en un análisis) la memoria crecería linealmente con copias casi iguales; ahí conviene el delta o
estructuras persistentes. También si `apply` dejara de ser determinista (azar, reloj): rehacer
tendría que guardar el estado resultante.
**Dónde en el código:** `application/MoveCommand`, `application/MoveHistory`, `ChessGame::undo/redo`.

## D-21 — raylib como UI gráfica
**Estado:** vigente
**Qué:** la segunda UI usa raylib 5.5, descargada con `FetchContent` y compilada como dependencia
PRIVATE de la librería `chess_raylib_adapter`. Las piezas se dibujan con su `symbol()` dentro de un
círculo: no hay imágenes ni otros archivos.
**Por qué:** raylib es una sola librería en C, sin dependencias, que compila con CMake igual en MSVC,
MinGW, gcc y clang (D-12), y su API inmediata (dibujar cada cuadro) alcanza para un tablero. Se
descartaron SFML (más pesada de compilar y con dependencias de sistema en Linux) y Qt (un framework
entero, con su propio build y su modelo de señales). Dibujar el símbolo hace que una pieza nueva
aparezca sin cambios en la UI, igual que en la consola. Que raylib sea PRIVATE hace que ni el núcleo
ni los headers del adaptador la vean: el compilador hace cumplir la regla de dependencias.
**Cuándo se rompería:** si la UI necesitara widgets de verdad (menús, diálogos, texto con tildes en
otra fuente), raylib obliga a dibujarlos a mano y conviene un toolkit de interfaz. También sin
internet en la primera configuración (se apaga con `CHESS_BUILD_RAYLIB=OFF`).
**Dónde en el código:** `src/adapters/raylib/CMakeLists.txt`, `CMakeLists.txt`, `app/CMakeLists.txt`.

## D-22 — La lógica de la UI gráfica separada de raylib (Humble Object)
**Estado:** vigente
**Qué:** el adaptador gráfico tiene tres clases. `RaylibUI` es la única que incluye `raylib.h` y solo
dibuja y lee el mouse y el teclado; `BoardInteraction` decide qué significa cada click (seleccionar,
mover, esperar la pieza de la promoción, cancelar) y `BoardLayout` convierte píxeles en casillas. Las
dos últimas no conocen raylib y se testean en `chess_raylib_tests` sin abrir ninguna ventana.
**Por qué:** la parte difícil de testear (una ventana, eventos del sistema operativo) queda reducida a
un objeto "humilde" sin decisiones, y lo que puede fallar (el orden de los clicks, la promoción, un
tablero de 10×10 dibujado al revés) tiene tests. Se descartó poner todo en `RaylibUI` porque la única
forma de probarlo hubiera sido jugar a mano. `BoardInteraction` solo conoce `GameService`, igual que
`ConsoleUI`: muestra que el puerto alcanza también para una UI con estado propio.
**Cuándo se rompería:** si la UI casi no tuviera lógica (un tablero que solo se muestra), la separación
sería ceremonia. Si creciera mucho (menús, animaciones, varias pantallas), `BoardInteraction`
acumularía modos y convendría un State propio del adaptador o un modelo de vista (MVVM).
**Dónde en el código:** `src/adapters/raylib/` (`RaylibUI`, `BoardInteraction`, `BoardLayout`),
`tests/adapters/raylib/`.

## D-23 — La notación algebraica vive en el adaptador, no en el dominio
**Estado:** vigente
**Qué:** convertir `"e2"` ↔ `Position` es tarea de `AlgebraicNotation`, en el adaptador de consola. El
dominio solo trabaja con `Position(file, rank)` en base 0.
**Por qué:** la notación es una forma de *mostrar* y *leer* casillas, no una regla del ajedrez: la UI
de raylib no la necesita (trabaja con clicks) y otra UI podría usar otra. Tenerla en el dominio
obligaría al núcleo a interpretar texto que escribe el usuario. `parse` recibe las `BoardDimensions`,
así que `j10` funciona en un tablero de 10×10 sin cambios.
**Cuándo se rompería:** si el núcleo tuviera que producir notación por sí mismo (exportar PGN, guardar
el historial como `1. e4 e5`), la notación pasaría a ser parte del modelo y convendría moverla a la
aplicación o detrás de un puerto propio.
**Dónde en el código:** `src/adapters/console/AlgebraicNotation`, `ConsoleUI`, `ConsoleRenderer`.

## D-24 — Alcance opcional que se dejó afuera
**Estado:** vigente
**Qué:** de la lista opcional de la consigna (§4) se implementaron jaque mate, ahogado, enroque, captura
al paso, promoción, deshacer/rehacer e interfaz gráfica. **No** se implementaron las tablas por
repetición ni por la regla de los 50 movimientos, ni un oponente con IA.
**Por qué:** las funcionalidades elegidas son las que muestran patrones (State, Command + Memento,
Observer con una segunda UI) y ponen a prueba la extensibilidad (jugadas especiales sobre reglas
compuestas). Las que quedaron afuera no agregan ningún patrón nuevo y sí bastante código. Queda
registrado cómo entrarían sin romper el diseño:
- **Repetición y 50 movimientos:** un contador de medias jugadas en `GameState` (que actualiza
  `MoveApplier`) y una comparación contra los estados anteriores, que ya guardan los `MoveCommand` del
  historial. `GameStatusEvaluator` sumaría un estado de tablas, y `ChessGame::updatePhase` pasaría a
  `GameOverPhase` con él igual que con el mate.
- **IA:** un adaptador más (un "jugador" que elige entre `legalMovesFrom` y llama a `makeMove`),
  armado en el composition root. El núcleo no cambia.
**Cuándo se rompería:** si la defensa pidiera alguna de estas reglas, el plan de arriba es el punto de
partida. Una IA competitiva sí pondría en cuestión D-04: copiar el estado por cada jugada es lento
para buscar en profundidad.
**Dónde en el código:** puntos de extensión: `GameState`, `MoveApplier`, `GameStatusEvaluator`,
`ChessGame::updatePhase`.
