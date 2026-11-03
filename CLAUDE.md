# CLAUDE.md — TPO Ajedrez (Ingeniería de Software 3.4.214)

Guía para agentes (y humanos) que trabajan en este repositorio. **Leer completo antes de tocar nada.**

## Qué es este proyecto

Un juego de ajedrez en C++ para un trabajo práctico universitario. La consigna completa está en
[`consigna.md`](consigna.md). El objetivo **no** es un motor robusto ni seguro: es un proyecto
**didáctico, claro, modular y con buenas prácticas**, donde cada patrón de diseño usado esté
justificado y documentado.

Lo que más se evalúa (ver consigna §9):
1. Separación núcleo/adaptadores (arquitectura hexagonal), SOLID, composición sobre herencia.
2. **Diagrama UML consistente con el código real** durante todo el desarrollo.
3. Tests unitarios del núcleo sin infraestructura.
4. Documento de decisiones en formato **qué / por qué / cuándo se rompería**.
5. **Escalabilidad real**: en la defensa individual pueden pedir *en el momento* agregar una pieza
   nueva o cambiar el tamaño del tablero. El diseño debe soportar eso sin modificar clases existentes.

## Documentos clave

| Archivo | Contenido |
|---|---|
| [`docs/arquitectura.md`](docs/arquitectura.md) | Arquitectura hexagonal, módulos, catálogo de patrones (dónde y por qué). **Es la especificación de diseño.** |
| [`docs/roadmap.md`](docs/roadmap.md) | Fases y misiones para agentes, con criterios de "terminado". |
| [`docs/decisiones.md`](docs/decisiones.md) | Registro de decisiones (qué / por qué / cuándo romperla). Entregable de la materia. |
| [`docs/diagramas/`](docs/diagramas/) | Diagramas PlantUML (`.puml`) — fuente de verdad del UML. |

## Stack técnico

- **Lenguaje:** C++17 (sin extensiones de compilador). Compatible MSVC, MinGW, gcc, clang.
- **Build:** CMake ≥ 3.20.
- **Tests:** GoogleTest, descargado con `FetchContent` (no se commitea).
- **UI:** dos adaptadores intercambiables: consola (ASCII) y raylib (gráfico, vía `FetchContent`).
- **Diagramas:** PlantUML (`.puml` versionados en `docs/diagramas/`).

## Convenciones de idioma

- **Código en inglés**: clases, métodos, variables, archivos (`Board`, `MovementRule`, `legalMovesFrom`).
- **Documentación en español**: comentarios de diseño, docs, diagramas (títulos/notas), mensajes de commit.

## Estructura del repositorio

```
src/core/domain/         Reglas del ajedrez: entidades y value objects. C++ puro.
src/core/application/    Casos de uso (GameService), historial, notificación de eventos.
src/core/ports/          Interfaces: puertos de entrada (driving) y salida (driven).
src/adapters/console/    Adaptador de UI por consola.
src/adapters/raylib/     Adaptador de UI gráfico.
app/                     Composition roots (main_console.cpp, main_raylib.cpp).
tests/core/              Tests unitarios del núcleo (GoogleTest).
docs/                    Documentación y diagramas.
```

## Reglas inquebrantables

1. **El núcleo (`src/core/`) no depende de nada externo.** Solo la biblioteca estándar de C++.
   Nada de raylib, iostream para UI, archivos, red, etc. El target CMake `chess_core` no enlaza
   contra ninguna librería; si un `#include` externo aparece en `core/`, no compila — y está bien que así sea.
2. **Las dependencias apuntan hacia adentro.** `adapters → ports ← application → domain`.
   El dominio no conoce a la aplicación; nada del núcleo conoce a los adaptadores.
3. **Composición sobre herencia.** Las piezas **no** son subclases (`Rook`, `Knight`…). Una pieza es
   tipo + color + una lista de `MovementRule` (Strategy). Ver `docs/arquitectura.md` §3.
4. **Abierto/cerrado.** Agregar una pieza o variante de tablero = agregar/registrar, no editar.
   Si para agregar algo hay que meter un `switch`/`if` sobre el tipo de pieza en código existente,
   el diseño está mal: parar y replantear.
5. **UML vivo.** Todo cambio en la interfaz pública de una clase (nueva clase, método público,
   relación) **actualiza el `.puml` correspondiente en el mismo commit**. Un commit que agrega una
   clase sin tocar el diagrama está incompleto.
6. **Cada patrón se documenta.** Al introducir un patrón: entrada en el catálogo de
   `docs/arquitectura.md` y entrada en `docs/decisiones.md` (qué / por qué / cuándo romperlo).
   En el código, un comentario breve en el header de la clase: `// Patrón: Strategy — ver docs/arquitectura.md §3.2`.
7. **Inyección de dependencias manual.** Sin singletons, sin estado global. Las dependencias entran
   por constructor; se arman en el composition root (`app/`).
8. **TDD en el núcleo.** Test primero (rojo), implementación mínima (verde), refactor.
9. **Simplicidad sobre robustez.** Es didáctico: preferir código legible a micro-optimizaciones
   (nada de bitboards). No agregar features fuera del roadmap (YAGNI).

## Estilo de código

- Clases/tipos `PascalCase`; métodos/variables `camelCase`; miembros privados con sufijo `_` (`board_`);
  constantes `kPascalCase`.
- Un header (`.hpp`) + un `.cpp` por clase relevante. Headers con `#pragma once`.
- Namespaces: `chess::domain`, `chess::application`, `chess::ports`, `chess::adapters::console`, `chess::adapters::raylib`.
- Ownership explícito: `std::unique_ptr` para propiedad, referencias para uso sin propiedad,
  `std::optional` para "puede no haber". Sin `new`/`delete` crudos.
- Interfaces = clases abstractas puras con destructor virtual.
- Preferir value objects inmutables (`Position`, `Move`).

## Comandos (una vez que exista el esqueleto, fase 2)

```bash
cmake -S . -B build                 # configurar
cmake --build build                 # compilar todo
ctest --test-dir build --output-on-failure   # correr tests
./build/app/chess_console           # jugar por consola (en Windows: build\app\Debug\chess_console.exe con MSVC)
```

Renderizar diagramas: `plantuml docs/diagramas/*.puml` (o la extensión PlantUML de VS Code).

## Flujo de trabajo de git

- Se trabaja **directo sobre `master`**. Commits chicos y atómicos, mensaje en español,
  en imperativo ("Agregar SlidingRule y sus tests").
- No commitear `build/` ni dependencias descargadas.
- Antes de commitear código: compila y `ctest` pasa.

## Cómo arrancar una sesión (para agentes)

1. Leer este archivo, `docs/arquitectura.md` y `docs/roadmap.md`.
2. Buscar en el roadmap la primera misión no completada (`[ ]`) cuyas dependencias estén hechas.
3. Respetar los límites de la misión (p. ej. la fase 1 **no escribe código**).
4. Al terminar: marcar la misión `[x]` en `docs/roadmap.md`, actualizar diagramas/decisiones
   si corresponde, y commitear.
