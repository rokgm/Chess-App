cmake_minimum_required(VERSION 3.22)

project(chess_ai)

enable_language(CXX)

# Set C++ 17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set warning level and treat warning as errors.
add_compile_options(-Werror -Wall -Wextra -Wpedantic)

add_subdirectory(src)