cmake_minimum_required(VERSION 3.22)

project(chess_ai)

enable_language(CXX)

# Set C++ 17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Install SFML's Linux Dependencies
if(UNIX AND NOT APPLE)
    message(STATUS "Installing SFML's Linux dependencies...")

    execute_process(
        COMMAND sudo apt-get update
        RESULT_VARIABLE update_result
        OUTPUT_VARIABLE update_output
        ERROR_VARIABLE update_error
    )
    if(NOT update_result EQUAL "0")
        message(FATAL_ERROR "Failed to update package list: ${update_error}")
    endif()

    # Install required packages
    set(PACKAGES 
        libxrandr-dev
        libxcursor-dev
        libudev-dev
        libfreetype-dev
        libopenal-dev
        libflac-dev
        libvorbis-dev
        libgl1-mesa-dev
        libegl1-mesa-dev
    )

    foreach(PACKAGE IN LISTS PACKAGES)
        execute_process(
            COMMAND sudo apt-get install -y ${PACKAGE}
            RESULT_VARIABLE install_result
            OUTPUT_VARIABLE install_output
            ERROR_VARIABLE install_error
        )
        if(NOT install_result EQUAL "0")
            message(FATAL_ERROR "Failed to install ${PACKAGE}: ${install_error}")
        endif()
    endforeach()

endif()

# Install CHESS-AI Project Dependencies
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
    message(WARNING "Compiler not supported for this project. Compilation might not work as expected. Compiler flags not set.")
endif()

target_precompile_headers(${PROJECT_NAME}
    PUBLIC
    [["logger/Logger.h"]]
)

# Copy chess pieces images to binary dir.
file(COPY pieces_images DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY fonts DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY book DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

enable_testing()
include(CTest)

add_subdirectory(src)
add_subdirectory(test)