# chess_raylib_adapter — el adaptador gráfico como librería, para que lo usen el ejecutable chess_raylib
# (app/) y sus tests (tests/adapters/raylib). Depende del núcleo y de raylib; el núcleo no sabe que existe.
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Solo la librería: sin ejemplos ni juegos de raylib.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

add_library(chess_raylib_adapter STATIC
    RaylibUI.cpp
)

# Los includes se escriben desde src: #include "adapters/raylib/RaylibUI.hpp"
target_include_directories(chess_raylib_adapter PUBLIC ${PROJECT_SOURCE_DIR}/src)
# raylib es PRIVATE: los headers del adaptador no lo incluyen, así quien los usa no ve raylib.
target_link_libraries(chess_raylib_adapter PUBLIC chess_core PRIVATE raylib chess_warnings)
