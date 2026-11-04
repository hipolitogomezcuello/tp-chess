# Composition roots: el único lugar que conoce núcleo y adaptadores y arma el grafo de objetos.

# chess_console — UI por consola. El adaptador vive en src/adapters/console (chess_console_adapter).
add_executable(chess_console main_console.cpp)
target_link_libraries(chess_console PRIVATE chess_console_adapter chess_warnings)

# chess_raylib — UI gráfica. El adaptador vive en src/adapters/raylib (chess_raylib_adapter).
if(CHESS_BUILD_RAYLIB)
    add_executable(chess_raylib main_raylib.cpp)
    target_link_libraries(chess_raylib PRIVATE chess_raylib_adapter chess_warnings)
endif()
