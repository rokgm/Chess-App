# Chess AI

## Requirements
* **CMake** (minimum required VERSION 3.22) with **Ninja** generator.
* Compiler:
    - **MSVC** (Microsoft Visual Studio 2022),
    - **g++** with C++ 17  support. **WIP** (should work, but was not checked for most recent verison).
- SFML, spdlog and SFML's Linux dependencies are automatically installed with this CMake configuration.


## Developing on Windows
- Using **VS Code**, open VS Code with `scripts/msvc_env.bat` to setup the correct environment for MSVC or open Code through `Visual Studio Developer Command Prompt`.

## Contributing
- Make a branch with name that summaries what will be implemented and then make a merge request
to version1.
- Use camel case.
- Use **`.clang-format`** to format code.
- Set auto format on.
- `clang-format` minimum required version `14.0.0`.

## Build
This applies to all build configurations (Debug, Release).
- Warnings are treated as errors.
- Warning levels are increased (see CMakeLists.txt for compile flags).

## TODO when Chess AI is Implemented
- Implemented install.
- Implement release binary on GitLab.

## Literature
Overview of the minimax engine: https://www.youtube.com/watch?v=w4FFX_otR-4&list=LL&index=2&t=313s
