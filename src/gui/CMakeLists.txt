add_library(gui Board.cpp Board.h Game.cpp Game.h GameMenu.cpp GameMenu.h)

target_link_libraries(gui
    PRIVATE
    sfml-graphics
    logger
    core
)

target_include_directories(gui
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)

target_precompile_headers(gui
    PRIVATE
    [["logger/Logger.h"]]
    <SFML/Graphics.hpp>
)

target_compile_definitions(gui
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:RelWithDebInfo>:DEBUG>
)

# Set warning level and treat warnings as errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(gui PRIVATE -Werror -Wall -Wextra -Wpedantic -Wconversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(gui PRIVATE /permissive /W4 /WX)
else()
    message(FATAL_ERROR "Compiler not supported for this project.")
endif()