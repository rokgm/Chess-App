
add_library(test test.cpp test1.cpp)

target_link_libraries(test spdlog::spdlog)

target_include_directories(test INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)