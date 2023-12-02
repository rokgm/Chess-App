add_library(logic BoardPosition.cpp BoardPosition.h)

target_link_libraries(logic
    PRIVATE
    logger
)

target_include_directories(logic
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
    PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_precompile_headers(logic
    PRIVATE
    [["logger/Logger.h"]]
    <unordered_map>
    <vector>
    <array>
)

target_compile_definitions(logic
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)