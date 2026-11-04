# chess_tests — tests unitarios del núcleo. Solo enlaza chess_core y GoogleTest:
# no hay ninguna infraestructura (ni UI, ni archivos, ni red).

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# MSVC: usar el mismo runtime (/MD) que el resto del proyecto.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# No instalar GoogleTest junto con el proyecto.
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Un archivo de test por clase, espejando src/core (tests/core/domain/SlidingRuleTest.cpp).
add_executable(chess_tests
    core/domain/ColorTest.cpp
    core/domain/OffsetTest.cpp
    core/domain/PositionTest.cpp
    core/domain/BoardDimensionsTest.cpp
    core/domain/PieceTypeIdTest.cpp
    core/domain/PieceDefinitionTest.cpp
    core/domain/PieceTest.cpp
    core/domain/PieceRegistryTest.cpp
    core/domain/PieceFactoryTest.cpp
    core/domain/BoardTest.cpp
    core/domain/MoveTest.cpp
    core/domain/GameStateTest.cpp
    core/domain/DirectionsTest.cpp
    core/domain/MoveContextTest.cpp
    core/domain/SlidingRuleTest.cpp
    core/domain/LeaperRuleTest.cpp
    core/domain/PawnRuleTest.cpp
)
# Helpers compartidos por los tests: #include "support/BoardBuilder.hpp"
target_include_directories(chess_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_link_libraries(chess_tests PRIVATE chess_core chess_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chess_tests)
