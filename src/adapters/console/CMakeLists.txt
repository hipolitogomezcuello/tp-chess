# chess_console_adapter — el adaptador de consola como librería, para que lo usen el ejecutable
# chess_console (app/) y sus tests (tests/adapters/console). Depende del núcleo, nunca al revés.
add_library(chess_console_adapter STATIC
    AlgebraicNotation.cpp
    ConsoleRenderer.cpp
    ConsoleUI.cpp
)

# Los includes se escriben desde src: #include "adapters/console/ConsoleUI.hpp"
target_include_directories(chess_console_adapter PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(chess_console_adapter PUBLIC chess_core PRIVATE chess_warnings)
