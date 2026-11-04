# chess_core — el núcleo (dominio + aplicación + puertos).
# Regla inquebrantable: este target NO enlaza contra ninguna biblioteca externa.
# Si alguien agrega un #include de raylib o similar en src/core/, no compila.
#
# Por ahora es INTERFACE porque todavía no hay ningún .cpp. En la misión 3.1, al
# agregar el primer archivo, pasa a:
#   add_library(chess_core STATIC)
#   target_sources(chess_core PRIVATE domain/Color.cpp ...)
# y los include/compile options pasan de INTERFACE a PUBLIC/PRIVATE.
add_library(chess_core INTERFACE)

# Los includes se escriben desde src/core: #include "domain/Position.hpp"
target_include_directories(chess_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(chess_core INTERFACE cxx_std_17)
