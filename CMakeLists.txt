cmake_minimum_required(VERSION 3.22)

project(chess_ai)

enable_language(CXX)

# Set C++ 17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Install SFML's Linux Dependencies
if(UNIX AND NOT APPLE)
    execute_process (
    COMMAND
    bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/install_SFML_linux_dependencies.sh)
endif()

# CHESS-AI Project Dependencies
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_Declare(GTest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.x
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SFML spdlog GTest)

# PROJECT CHESS-AI
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(${PROJECT_NAME} src/main.cpp)
else()
    add_executable(${PROJECT_NAME} WIN32 src/main.cpp)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    sfml-graphics
    $<$<PLATFORM_ID:Windows>:sfml-main>
    gui
    logger    
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:RelWithDebInfo>:DEBUG>
)

# Set warning level and treat warnings as errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra -Wpedantic -Wconversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /permissive /W4 /WX)
else()
    message(FATAL_ERROR "Compiler not supported for this project.")
endif()

target_precompile_headers(${PROJECT_NAME}
    PUBLIC
    [["logger/Logger.h"]]
)

# SFML needs this for audio
# if(WIN32)
#     add_custom_command(
#         TARGET ${PROJECT_NAME}
#         COMMENT "Copy OpenAL DLL"
#         PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
#         VERBATIM)
# endif()

# Copy chess pieces images to binary dir.
file(COPY pieces_images DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY fonts DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY book DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

enable_testing()
include(CTest)

add_subdirectory(src)
add_subdirectory(test)