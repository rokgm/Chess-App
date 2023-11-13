# Chess AI

## Requirements
- **CMake** (minimum required VERSION 3.22) with Ninja generator.
- **g++** (C++ 17).
- **SFML** is automatically installed with this CMake configuration (fetch content).
SFML's Linux dependencies must be installed with command:
```
sudo apt update
sudo apt install \
    libxrandr-dev \
    libxcursor-dev \
    libudev-dev \
    libfreetype-dev \
    libopenal-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev
```

## Developing on Windows
Recommended:
- **WSL2** is used by us to develop this application, so we would recommend it.
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
Overview of minimax engine: https://www.youtube.com/watch?v=w4FFX_otR-4&list=LL&index=2&t=313s
