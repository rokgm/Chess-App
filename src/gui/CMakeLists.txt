add_library(gui Board.cpp Board.h Game.cpp Game.h)

target_link_libraries(gui
    PRIVATE
    sfml-graphics
    logger
    logic
)

target_include_directories(gui
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

target_precompile_headers(gui
    PRIVATE
    <SFML/Graphics.hpp>
    [["logger/Logger.h"]]
)

target_compile_definitions(gui
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)