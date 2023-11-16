add_library(gui Board.cpp)

target_link_libraries(gui
    PRIVATE
    sfml-graphics
)

target_include_directories(gui
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_precompile_headers(gui
    PRIVATE
    <SFML/Graphics.hpp>
)