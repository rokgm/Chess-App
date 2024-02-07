add_executable(performance_tests enginePerformance.cpp)

target_link_libraries(performance_tests
    GTest::gtest_main
    core
)

add_test(NAME performance_tests COMMAND performance_tests)

add_compile_definitions(
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:RelWithDebInfo>:DEBUG>
)

# Add perft test positions to binary
file(COPY positions DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")