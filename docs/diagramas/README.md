# Diagramas (PlantUML)

Fuente de verdad del UML del proyecto. **Los `.puml` se versionan; las imágenes exportadas son opcionales.**

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
- CLI: `plantuml -tsvg docs/diagramas/*.puml` (requiere Java y Graphviz) o el servidor web de PlantUML.
