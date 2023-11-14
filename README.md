# Chess AI

## Requirements
- **CMake** (minimum required VERSION 3.22) with **Ninja** generator.
- **g++** (support for C++ 17) or **MSVC** (Microsoft Visual Studio 2022).
- SFML, spdlog and SFML's Linux dependencies are automatically installed with this CMake configuration.


## Developing on Windows

- sing **VS Code**, open VS Code with `scripts/msvc_env.bat` to setup the correct environment for MSVC or open Code through `Visual Studio Developer Command Prompt`.
- Using **WSL2** to run g++.
Set wsl2 `wsl --set-default-version 2`. Check version `wsl -l -v`.

## Contributing
- Make a branch with name that summaries what will be implemented and then make a merge request
to version1.

## Clang Format
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
