# Composition roots: el único lugar que conoce núcleo y adaptadores y arma el grafo de objetos.

# chess_console — UI por consola. Las fuentes de src/adapters/console se suman en la fase 6.
add_executable(chess_console main_console.cpp)
target_link_libraries(chess_console PRIVATE chess_core chess_warnings)

# chess_raylib se agrega en la fase 8.
