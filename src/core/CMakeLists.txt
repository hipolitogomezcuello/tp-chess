# chess_core — el núcleo (dominio + aplicación + puertos).
# Regla inquebrantable: este target NO enlaza contra ninguna biblioteca externa.
# Si alguien agrega un #include de raylib o similar en src/core/, no compila.
add_library(chess_core STATIC)

target_sources(chess_core PRIVATE
    domain/Color.cpp
    domain/BoardDimensions.cpp
    domain/PieceDefinition.cpp
    domain/Piece.cpp
    domain/PieceRegistry.cpp
    domain/PieceFactory.cpp
    domain/Board.cpp
    domain/Move.cpp
    domain/MoveContext.cpp
    domain/Directions.cpp
    domain/SlidingRule.cpp
    domain/LeaperRule.cpp
    domain/RuleHelpers.cpp
    domain/PawnRule.cpp
    domain/StandardPieces.cpp
    domain/StandardStartingPosition.cpp
    domain/MoveGenerator.cpp
    domain/MoveApplier.cpp
    domain/AttackDetector.cpp
    domain/LegalMoveFilter.cpp
    domain/GameStatusEvaluator.cpp
    domain/RulesEngine.cpp
    application/ChessGame.cpp
    application/InProgressPhase.cpp
    application/GameOverPhase.cpp
)

# Los includes se escriben desde src/core: #include "domain/Position.hpp"
target_include_directories(chess_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(chess_core PUBLIC cxx_std_17)
target_link_libraries(chess_core PRIVATE chess_warnings)
