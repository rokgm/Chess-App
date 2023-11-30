add_library(gui Board.cpp Board.h Game.cpp Game.h PiecesSprites.cpp PiecesSprites.h)

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