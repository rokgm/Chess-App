add_executable(unit_tests pawnMovesGeneration.cpp knightMovesGeneration.cpp movesGeneration.cpp fenParser.cpp evaluation.cpp)

target_link_libraries(unit_tests
    GTest::gtest_main
    core
)

add_test(NAME unit_tests COMMAND unit_tests)

add_compile_definitions(
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:RelWithDebInfo>:DEBUG>
)

# Add perft test positions to binary
file(COPY perft_positions DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")