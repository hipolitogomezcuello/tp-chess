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

<!-- Próximas decisiones (las completan los agentes cuando las implementen):
     D-06 Command + Memento para deshacer/rehacer (snapshot completo vs. delta)
     D-07 State para las fases de la partida
     D-08 Observer como puerto de salida
     D-09 Inyección de dependencias manual y composition root (sin contenedor de DI)
     D-10 raylib como UI gráfica (frente a SFML o Qt)
     D-11 PlantUML como UML vivo versionado en el repo -->
