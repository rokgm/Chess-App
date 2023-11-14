cmake_minimum_required(VERSION 3.22)

project(chess_ai)

enable_language(CXX)

# Set C++ 17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set warning level and treat warning as errors.
add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wconversion)

add_subdirectory(src)

# Install SFML Linux Dependencies
if(UNIX AND NOT APPLE)
    execute_process (
    COMMAND
    bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/install_SFML_linux_dependencies.sh)
endif()

# Project Dependencies
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(SFML spdlog)
