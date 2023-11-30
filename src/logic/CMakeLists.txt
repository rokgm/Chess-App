add_library(logic Pieces.cpp Pieces.h)

target_include_directories(logic
    INTERFACE
    ${PROJECT_SOURCE_DIR}/src
)