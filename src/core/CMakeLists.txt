add_library(core BoardState.cpp BoardState.h
    PieceBitBoards.h PieceBitBoards.cpp
    PieceType.h PieceType.cpp
    Pawn.h
    Knight.h
    King.h King.cpp
    MoveGenerator.h MoveGenerator.cpp
    Move.h Move.cpp
    magic-bits-master/include/magic_bits.hpp
    EndOfGameChecker.h EndOfGameChecker.cpp
    Engine.h Engine.cpp
    Evaluate.h Evaluate.cpp
    ZobristHash.h ZobristHash.cpp
    TranspositionTable.h TranspositionTable.cpp
)

target_link_libraries(core
    PUBLIC
    logger
)

target_include_directories(core
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
    PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_precompile_headers(core
    PRIVATE
    [["logger/Logger.h"]]
)

target_compile_definitions(core
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)