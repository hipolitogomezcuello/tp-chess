# Decisiones de diseño: qué / por qué / cuándo se rompería

Versión breve con las seis decisiones centrales del proyecto. El registro completo, con todas las
decisiones y alternativas descartadas, está en [`decisiones.md`](decisiones.md).

## 1. Arquitectura hexagonal (D-01)
**Qué:** el núcleo (`src/core/`: dominio + casos de uso) solo se comunica con el exterior por los
puertos `GameService` (entrada) y `GameObserver` (salida). La consola y raylib son adaptadores.
**Por qué:** la consigna pide un núcleo sin dependencias externas y testeable sin infraestructura. Que
haya dos UIs sobre el mismo puerto muestra que el desacople funciona. CMake lo hace cumplir: `chess_core`
no enlaza ninguna librería.
**Cuándo se rompería:** en un programa chico con una sola UI, los puertos sobran. Tampoco sirve si el
rendimiento obligara a la UI a leer estructuras internas del núcleo.

## 2. Piezas por composición de reglas, no por herencia (D-02, D-03)
**Qué:** una pieza es un `PieceDefinition` (id textual + símbolo + lista de `MovementRule` + traits)
registrado en un `PieceRegistry`. No hay subclases `Rook` ni `Knight`, ni un `enum` cerrado de tipos.
**Por qué:** la Reina es Torre + Alfil. Una pieza nueva, como el Arzobispo (Alfil + Caballo), se arma
registrando reglas que ya existen, sin editar ninguna clase ni ningún `switch` (OCP). Es justo lo que
puede pedirse en la defensa.
**Cuándo se rompería:** si las piezas tuvieran comportamiento que no es movimiento (habilidades, estado
interno complejo), las reglas y los traits no alcanzarían. Si nunca fuera a haber piezas nuevas, la
herencia o un `enum` serían más directos de leer.

## 3. Validación en dos etapas: pseudo-legal → legal (D-04)
**Qué:** cada `MovementRule` genera jugadas solo según su geometría, sin tener en cuenta el jaque. Después,
`LegalMoveFilter` descarta las que dejan al propio rey en jaque, simulando cada jugada sobre una copia
del estado.
**Por qué:** cada regla tiene una sola responsabilidad (SRP). La lógica del jaque está en un solo lugar y
vale para cualquier pieza, también para las que se agreguen después.
**Cuándo se rompería:** en un motor competitivo, copiar el estado en cada jugada es caro. Ahí convendría
hacer y deshacer la jugada sobre el mismo estado, o usar bitboards.

## 4. Ninguna regla asume un tablero de 8×8 (D-09)
**Qué:** los límites salen siempre de `BoardDimensions`. El doble avance del peón depende de `hasMoved()` y
no de que esté en la segunda fila, y la promoción ocurre en la última fila del tablero real.
**Por qué:** si en la defensa piden un tablero de 10×10, alcanza con escribir otro
`StartingPositionProvider`. `Board`, las reglas y el motor quedan como están.
**Cuándo se rompería:** con tableros que no son rectangulares o que tienen casillas inutilizables.
`BoardDimensions::contains` ya no alcanzaría y haría falta una abstracción de forma (`BoardShape`).

## 5. Observer como puerto de salida (D-16)
**Qué:** el núcleo avisa lo que pasa (jugada hecha, jaque, fin de partida, deshacer) a través de
`GameObserver`, una interfaz definida en `ports/`. Los adaptadores la implementan y se suscriben con
`addObserver`.
**Por qué:** el núcleo no puede conocer a la UI, pero la UI necesita enterarse de los cambios. Con Observer
la dependencia se invierte: el núcleo define el contrato y la UI lo cumple. Se descartó que la UI
consultara el estado después de cada jugada (polling), porque cada adaptador tendría que deducir qué
cambió.
**Cuándo se rompería:** si los eventos tuvieran que pasar a otro hilo o a otro proceso, la notificación
síncrona por referencia no sirve y haría falta una cola de eventos.

## 6. Deshacer con Command + Memento del estado completo (D-20)
**Qué:** cada jugada válida se ejecuta como un `MoveCommand`, que antes de aplicarla guarda una copia
completa del `GameState`. `undo` restaura esa copia y `redo` vuelve a ejecutar el comando. `MoveHistory`
guarda los comandos en dos pilas.
**Por qué:** deshacer funciona igual para cualquier jugada (enroque, captura al paso, promoción, una pieza
inventada) sin programar la operación inversa de cada una. Se descartó guardar solo los cambios de cada
jugada porque obligaba a escribir una inversa por cada tipo de jugada.
**Cuándo se rompería:** con estados grandes o historiales de miles de jugadas, la memoria crece con copias
casi iguales. En ese caso conviene guardar solo los cambios. Tampoco sirve si aplicar una jugada dejara de
ser determinista (por azar o por reloj).
