add_library(gui Board.cpp Board.h Game.cpp Game.h PieceTextures.cpp PieceTextures.h)

target_link_libraries(gui
    PRIVATE
    sfml-graphics
    spdlog
    logic
)

target_include_directories(gui
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

target_precompile_headers(gui
    PRIVATE
    <SFML/Graphics.hpp>
)

target_compile_definitions(gui
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>

    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=0>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=2>
)