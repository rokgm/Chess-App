add_library(logic BoardPosition.cpp BoardPosition.h)

target_link_libraries(logic
    PRIVATE
    spdlog
)

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