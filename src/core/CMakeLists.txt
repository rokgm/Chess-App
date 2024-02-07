add_library(core BoardState.cpp BoardState.h
    PieceBitBoards.h PieceBitBoards.cpp
    PieceType.h PieceType.cpp
    Pawn.h
    Knight.h
    King.h King.cpp
    MoveGenerator.h
    Move.h Move.cpp
    magic-bits-master/include/magic_bits.hpp
    EndOfGameChecker.h EndOfGameChecker.cpp
    Engine.h Engine.cpp
    Evaluate.h Evaluate.cpp
    ZobristHash.h ZobristHash.cpp
    TranspositionTable.h TranspositionTable.cpp
    OpeningBook.h OpeningBook.cpp
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
    $<$<CONFIG:RelWithDebInfo>:DEBUG>
)

# Set warning level and treat warnings as errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(core PRIVATE -Werror -Wall -Wextra -Wpedantic -Wconversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(core PRIVATE /permissive /W4 /WX)
else()
    message(FATAL_ERROR "Compiler not supported for this project.")
endif()