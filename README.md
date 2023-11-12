# Chess AI

## Requirements
- CMake (minimum required VERSION 3.22) with Ninja generator
- g++ (C++ 17)
- TODO: add lib requirements (sfml)

## Developing on Windows
Recommended:
- Wsl2 is used by us to develop this application, so we would recommend it.
Set wsl2 `wsl --set-default-version 2`. Check version `wsl -l -v`.

## Contributing
- Make a branch with name that summaries what will be implemented and then make a merge request
to version1.

## Clang Format
- Use .`clang-format` to format code.
- Set auto format on.
- `clang-format` minimum required version `14.0.0`.

## Build
This applies to all build configurations (Debug, Release, RelWithDebInfo).
- Warnings are treated as errors.
- Warning levels are increased (see CMakeLists.txt for compile flags).

## TODO when Chess AI is Implemented
- Implemented install.
- Implement release binary on GitLab.

## Literature
Overview of minimax engine: https://www.youtube.com/watch?v=w4FFX_otR-4&list=LL&index=2&t=313s
