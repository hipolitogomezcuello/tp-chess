cmake_minimum_required(VERSION 3.20)

project(tp_chess
    VERSION 1.0.0
    DESCRIPTION "TPO Ajedrez — arquitectura hexagonal en C++17"
    LANGUAGES CXX)

# C++17 estándar, sin extensiones de compilador (ver CLAUDE.md, stack técnico).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Release sin DLLs del runtime de C++ (fase 13): los .exe corren en una máquina sin el
# Visual C++ Redistributable ni las DLLs de MinGW. Se aplica a todos los targets, incluidos
# raylib y GoogleTest, para que no se mezclen runtimes distintos al enlazar.
option(CHESS_STATIC_RUNTIME "Enlazar el runtime de C++ de forma estática" ON)
if(CHESS_STATIC_RUNTIME)
    if(MSVC)
        # raylib declara una versión mínima vieja de CMake; sin esta política ignoraría la variable.
        set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(MINGW)
        add_link_options(-static)
    endif()
endif()

option(CHESS_BUILD_TESTS "Compilar los tests unitarios del núcleo" ON)
# La UI gráfica descarga raylib en la primera configuración; se puede apagar si no hay internet.
option(CHESS_BUILD_RAYLIB "Compilar la UI gráfica con raylib" ON)

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
if(CHESS_BUILD_RAYLIB)
    add_subdirectory(src/adapters/raylib)
endif()
add_subdirectory(app)

if(CHESS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
