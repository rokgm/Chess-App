cmake_minimum_required(VERSION 3.22)

project(chess_ai)

enable_language(CXX)

# Set C++ 17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set warning level and treat warning as errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wconversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/permissive /W4 /WX)
else()
    message(FATAL_ERROR "Compiler not supported for this project.")
endif()

add_subdirectory(src)

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
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(SFML spdlog)

# PROJECT CHESS-AI
add_executable(${PROJECT_NAME} src/main.cpp)
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    spdlog::spdlog
    sfml-graphics
    $<$<PLATFORM_ID:Windows>:sfml-main>
    gui
)

add_compile_definitions(
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>

    $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=0>
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=2>
)

# SFML needs this
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()

# Copy chess pieces images to binary dir.
file(COPY pieces_images DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
