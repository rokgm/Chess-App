add_library(logger Logger.cpp Logger.h)

target_link_libraries(logger
    PUBLIC
    spdlog
)

target_include_directories(logger
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_definitions(logger
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)