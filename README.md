# Chess AI

## Requirements
- CMake (minimum required VERSION 3.22) with Ninja generator
- g++ (C++ 17)
- TODO: add lib requirements (sfml)

## Developing on Windows
Recommended:
- Wsl2 is used by us to develop this application, so we would recommend.
Set wsl2 `wsl --set-default-version 2`. Check version `wsl -l -v`.
- To open project directory in VS Code in WSL environment use `open_vscode_in_wsl.bat`.


## Clang Format
- Use .`clang-format` to format code.
- `clang-format` minimum required version `14.0.0`.
- Recommended: set auto format on save file.

## Build
This applies to all build configurations (Debug, Release, RelWithDebInfo).
- Warnings are treated as errors.
- Warning levels are increased (see CMakeLists.txt for compile flags).

## Install
- Not implemented
