# Chess Engine and GUI

Chess application developed as a bonus project for Advanced Programming subject at TUM. GUI was implemented using SFML library.

![Gameplay](assets/gameplay.gif)

## Engine Search:
- Bit board approach.
- NEGAMAX (minimax variant) with Alpha-Beta pruning.
- Iterative Deepening.
- Transposition Table (Zobrist Hashing).
- Move Ordering.
- MVV/LVA.
- Piece-Square Tables.
- Mop-up Evaluation.
- Quiescence Search.
- Pawn Shield.
- Opening Book (currently uses 4469 GM games parsed from PGNs: https://www.pgnmentor.com/files.html#openings).
#### Move Generation Correctness:
- **PERFT** tests done on 132 different positions, evaluated to depth 5.


## Requirements
* **CMake** (minimum required VERSION 3.22) with **Ninja** generator.
* Compiler:
    - **MSVC** (Microsoft Visual Studio 2022 (MSVC 14.38.33130)),
    - **g++** with C++ 17  support (gcc 11.4.0).

## Build
### On windows set up MSVC environment:
- Open a terminal and run: `call <path to vcvarsall.bat> x64 && cmd`. For example run: `call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 && cmd`.

- Alternatively, this can also be set up through the Visual Studio Developer Command Prompt:
    - Open the x64 Native Tools Command Prompt. Ensure the environment is initialized for 'x64'. If not, run `vcvarsall.bat x64`.

Verify that the environment is set up by calling `cl` (MSVC compiler).

### On linux install SFML's dependencies:
```console
sudo apt-get update
sudo apt-get install -y \
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
If there is a problem with `ft2build.h`, add directories `/usr/include/freetype` or `/usr/include/freetype2` to $PATH.

### Build with CMake:
- Run configure: `cmake --preset Release/Debug`.
- Run build: `cmake --build --preset Release/Debug --parallel`.

**This applies to all build configurations** (Debug, Release).
- Warnings are treated as errors.
- Warning levels are increased (see CMakeLists.txt for compile flags).

## Contributing
- Use camel case.
- Use **`.clang-format`** to format code.
- Set auto format on.
- `clang-format` minimum required version `14.0.0`.

## Literature
- General: https://www.chessprogramming.org/Main_Page
- Overview of the minimax engine: https://www.youtube.com/watch?v=w4FFX_otR-4&list=LL&index=2&t=313s
- Bit Boards: https://ameye.dev/notes/chess-engine/
