#!/usr/bin/env sh
# Exporta los diagramas PlantUML a docs/diagramas/export/ en SVG y PNG (misión 9.2).
#
# Uso:   sh docs/diagramas/exportar.sh [ruta/a/plantuml.jar]
# Solo necesita Java: no usa Graphviz. Los diagramas de clases se ordenan con ELK y el resto con
# Smetana (los dos motores vienen dentro del jar). Smetana falla con clases-dominio.puml y ELK
# superpone las etiquetas de los puertos en componentes.puml, por eso se eligen por archivo.
set -e

JAR="${1:-plantuml.jar}"
DIR="$(cd "$(dirname "$0")" && pwd)"
OUT="export"   # relativo a cada .puml

run() {
  layout="$1"; shift
  for fmt in svg png; do
    java -DPLANTUML_LIMIT_SIZE=16384 -jar "$JAR" -charset UTF-8 -Playout="$layout" \
      -t"$fmt" -o "$OUT" "$@"
  done
}

run elk     "$DIR"/clases-*.puml
run smetana "$DIR"/componentes.puml "$DIR"/estados-partida.puml \
            "$DIR"/secuencia-jugada.puml "$DIR"/actividad-validacion.puml

echo "Diagramas exportados en $DIR/$OUT"
