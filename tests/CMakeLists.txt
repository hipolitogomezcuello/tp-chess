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
    core/domain/CastlingRuleTest.cpp
    core/domain/StandardPiecesTest.cpp
    core/domain/StandardStartingPositionTest.cpp
    core/domain/MoveGeneratorTest.cpp
    core/domain/MoveApplierTest.cpp
    core/domain/AttackDetectorTest.cpp
    core/domain/LegalMoveFilterTest.cpp
    core/domain/GameStatusEvaluatorTest.cpp
    core/domain/RulesEngineTest.cpp
    core/ports/MoveResultTest.cpp
    core/application/ChessGameTest.cpp
    core/application/MoveCommandTest.cpp
    core/application/MoveHistoryTest.cpp
)
# Helpers compartidos por los tests: #include "support/BoardBuilder.hpp"
target_include_directories(chess_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_link_libraries(chess_tests PRIVATE chess_core chess_warnings GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(chess_tests)

# chess_console_tests — tests del adaptador de consola (notación, dibujo y loop de comandos con
# streams en memoria). Van aparte de chess_tests para que los tests del núcleo sigan sin
# depender de ningún adaptador.
add_executable(chess_console_tests
    adapters/console/AlgebraicNotationTest.cpp
    adapters/console/ConsoleRendererTest.cpp
    adapters/console/ConsoleUITest.cpp
)
target_include_directories(chess_console_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/core)
target_link_libraries(chess_console_tests PRIVATE chess_console_adapter chess_warnings GTest::gtest_main)
gtest_discover_tests(chess_console_tests)
