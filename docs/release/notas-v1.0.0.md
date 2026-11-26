# TPO Ajedrez v1.0.0

Primera versión del juego de ajedrez del TPO de Ingeniería de Software (3.4.214). El código está en
C++17 con arquitectura hexagonal: el núcleo con las reglas no depende de ninguna UI, y las dos
interfaces (consola y gráfica) son adaptadores intercambiables.

## Qué incluye

- Todas las reglas del ajedrez clásico: movimientos de las seis piezas, jaque, jaque mate y ahogado.
- Jugadas especiales: enroque corto y largo, captura al paso y promoción (eligiendo la pieza).
- Deshacer y rehacer jugadas.
- Dos interfaces que se juegan igual:
  - **Consola** (`chess_console.exe`): tablero en ASCII y jugadas en notación de casillas.
  - **Gráfica** (`chess_raylib.exe`): ventana con piezas pixel art, pantalla de título, destinos
    resaltados y aviso de jaque.
- Partida de dos jugadores en la misma computadora. No hay oponente con IA ni tablas por repetición o
  por la regla de los 50 movimientos (ver `docs/decisiones.md`, D-24).

## Cómo jugar

### Interfaz gráfica (`chess_raylib.exe`)

1. Abrir `chess_raylib.exe` y elegir **Nueva partida** (o **Salir** para cerrar).
2. Click en una pieza propia para elegirla: se marcan sus destinos posibles.
3. Click en un destino marcado para mover. En una promoción aparece un cuadro para elegir la pieza.
4. Teclas: `U` deshace, `R` rehace, `Esc` o click derecho cancelan la selección.
5. Al terminar la partida (mate o ahogado), **Volver al titulo** lleva a la pantalla de título.

### Consola (`chess_console.exe`)

Abrir `chess_console.exe` (o correrlo desde una terminal) y escribir comandos:

| Comando | Qué hace |
|---|---|
| `e2 e4` | Mueve la pieza de `e2` a `e4` |
| `e7 e8 q` | Promoción: la letra es la pieza elegida (`q`, `r`, `b`, `n`) |
| `moves e2` | Muestra los destinos posibles de la pieza en `e2` |
| `undo` / `redo` | Deshace o rehace la última jugada |
| `help` | Lista los comandos |
| `quit` | Sale |

## Requisitos

- Windows 10 u 11 de 64 bits.
- Nada más: los ejecutables vienen con raylib y el runtime de C++ enlazados de forma estática, así que
  no hace falta instalar el Visual C++ Redistributable ni copiar DLLs ni imágenes al lado.
- La interfaz gráfica necesita una placa de video con OpenGL 3.3 (cualquier equipo de los últimos
  diez años).

## Compilar desde el código

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Los ejecutables quedan en `build/app/Release/`. Ver `README` y `CLAUDE.md` para más detalle.
