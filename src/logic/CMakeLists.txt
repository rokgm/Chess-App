add_library(logic BoardState.cpp BoardState.h)

target_include_directories(logic
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_definitions(logic
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>

    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=0>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=2>
)