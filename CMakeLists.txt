cmake_minimum_required(VERSION 3.20)

project(tp_chess
    VERSION 0.1.0
    DESCRIPTION "TPO Ajedrez — arquitectura hexagonal en C++17"
    LANGUAGES CXX)

# C++17 estándar, sin extensiones de compilador (ver CLAUDE.md, stack técnico).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CHESS_BUILD_TESTS "Compilar los tests unitarios del núcleo" ON)

# Warnings altos solo para nuestros targets (no para GoogleTest ni raylib).
# Se enlaza como dependencia PRIVATE de cada target propio.
add_library(chess_warnings INTERFACE)
if(MSVC)
    # /utf-8: los comentarios en español tienen tildes.
    target_compile_options(chess_warnings INTERFACE /W4 /permissive- /utf-8)
else()
    target_compile_options(chess_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(src/core)
add_subdirectory(src/adapters/console)
add_subdirectory(app)

if(CHESS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
