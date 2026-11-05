# tp-chess

Ajedrez en C++17 con arquitectura hexagonal. TPO de Ingeniería de Software (3.4.214).

- Consigna: [`consigna.md`](consigna.md)
- Guía para el desarrollo (humanos y agentes): [`CLAUDE.md`](CLAUDE.md)
- Arquitectura y patrones: [`docs/arquitectura.md`](docs/arquitectura.md)
- Roadmap: [`docs/roadmap.md`](docs/roadmap.md)
- Decisiones de diseño: [`docs/decisiones.md`](docs/decisiones.md)
- Guía de defensa (extensiones en vivo): [`docs/guia-defensa.md`](docs/guia-defensa.md)
- Diagramas exportados (SVG/PNG): [`docs/diagramas/export/`](docs/diagramas/export/)
- Presentación para la exposición: [`docs/presentacion/tpo-ajedrez.pptx`](docs/presentacion/tpo-ajedrez.pptx)

## Requisitos

- Un compilador con soporte de C++17: MSVC (Visual Studio 2019 o posterior), MinGW, gcc o clang.
- [CMake](https://cmake.org/download/) 3.20 o posterior.
- Conexión a internet la **primera vez** que se configura: CMake descarga GoogleTest y raylib con
  `FetchContent`. No hay que instalar nada más a mano. Sin internet (o sin necesitar la UI gráfica),
  se puede configurar con `cmake -S . -B build -DCHESS_BUILD_RAYLIB=OFF`.

> **Windows sin `cmake` en el PATH:** Visual Studio trae uno en
> `<carpeta de Visual Studio>\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\`.
> Otra opción es abrir la *Developer PowerShell for VS*, que ya lo tiene en el PATH.

## Compilar

Desde la raíz del repositorio:

```bash
cmake -S . -B build        # configurar (la primera vez descarga GoogleTest y raylib)
cmake --build build        # compilar el juego y los tests
```

Todo lo generado queda en `build/`, que no se versiona. Para empezar de cero, alcanza con borrar esa carpeta.

Con Visual Studio (un generador *multi-config*), la configuración se elige al compilar:
`cmake --build build --config Debug` (o `Release`). Si no se indica nada, se usa `Debug`.

## Jugar (consola)

| Generador | Ejecutable |
|---|---|
| Visual Studio (Windows) | `build\app\Debug\chess_console.exe` |
| Make / Ninja (Linux, macOS, MinGW) | `./build/app/chess_console` |

Las blancas juegan primero. Comandos:

| Comando | Qué hace |
|---|---|
| `e2 e4` | mueve la pieza de `e2` a `e4` (el enroque es mover el rey dos casillas: `e1 g1`) |
| `e7 e8 q` | mueve y promueve el peón; la letra es el símbolo de la pieza (`q r b n`) o su nombre (`queen`) |
| `moves e2` | lista a dónde puede ir la pieza de `e2` |
| `undo` / `redo` | deshace o rehace la última jugada |
| `help` | muestra la ayuda |
| `quit` | sale (también termina al cerrar la entrada: `Ctrl+Z` + Enter en Windows, `Ctrl+D` en Linux/macOS) |

En el tablero, las blancas van en mayúscula (`P N B R Q K`), las negras en minúscula y las casillas
vacías se muestran con `.`. Las jugadas inválidas no se aplican: el juego explica por qué (casilla
vacía, pieza del rival, jugada ilegal o que deja al rey en jaque, promoción sin pieza). Se juega con
todas las reglas: enroque, captura al paso, promoción, jaque mate y ahogado.

```
8 r n b q k b n r
7 p p p p p p p p
6 . . . . . . . .
5 . . . . . . . .
4 . . . . P . . .
3 . . . . . . . .
2 P P P P . P P P
1 R N B Q K B N R
  a b c d e f g h
```

## Jugar (ventana, raylib)

| Generador | Ejecutable |
|---|---|
| Visual Studio (Windows) | `build\app\Debug\chess_raylib.exe` |
| Make / Ninja (Linux, macOS, MinGW) | `./build/app/chess_raylib` |

- **Click izquierdo** en una pieza propia: la elige y marca sus destinos (punto = casilla libre,
  anillo = captura). Click en un destino: mueve. Click en otro lado: cancela.
- En una **promoción** aparecen las piezas posibles: click en la elegida.
- **U** deshace, **R** rehace, **Esc** o **click derecho** cancelan la selección.

Las dos UIs usan exactamente el mismo núcleo: solo cambia el composition root (`app/main_*.cpp`).

## Tests

Hay tres ejecutables de tests (GoogleTest):

| Ejecutable | Qué prueba |
|---|---|
| `chess_tests` | el núcleo (`tests/core/`): dominio, aplicación y puertos, sin ninguna infraestructura |
| `chess_console_tests` | el adaptador de consola (`tests/adapters/console/`), con streams en memoria |
| `chess_raylib_tests` | la parte del adaptador gráfico que no necesita ventana (`tests/adapters/raylib/`) |

Correr todos con CTest, después de compilar:

```bash
ctest --test-dir build --output-on-failure            # Make / Ninja
ctest --test-dir build -C Debug --output-on-failure   # Visual Studio (hay que indicar la configuración)
```

Para correr solo una parte, `-R` filtra por nombre (expresión regular):

```bash
ctest --test-dir build -C Debug -R ChessGameTest
```

También se pueden correr los ejecutables directamente, con los filtros de GoogleTest:

```bash
build\tests\Debug\chess_tests.exe --gtest_filter=ChessGameTest.*   # Visual Studio
./build/tests/chess_tests --gtest_filter=ChessGameTest.*           # Make / Ninja
```

## Diagramas

Los diagramas UML son archivos PlantUML en [`docs/diagramas/`](docs/diagramas/). Se renderizan con
`plantuml docs/diagramas/*.puml` o con la extensión PlantUML de VS Code.
