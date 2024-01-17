# Chess AI

Chess app written as a bonus project for subject Advanced Programming. GUI was implemented using SFML library.

## Engine Search:
- No opening books or endgame tablebases are used.
- Bit board approach.
- NEGAMAX (minimax variant) with Alpha-Beta pruning
- Iterative deepening.
- Transposition table (Zobrist hashing).
- Move ordering.
- MVV/LVA.
- Piece-Square Tables.
- Mop-up Evaluation.
##Move Generation Correctness:
- **PERFT** tests done on 132 different positions, evaluated to depth 5 (up to 190 million moves per position).


## Requirements
* **CMake** (minimum required VERSION 3.22) with **Ninja** generator.
* Compiler:
    - **MSVC** (Microsoft Visual Studio 2022),
    - **g++** with C++ 17  support. **WIP** (should work, but was not checked for most recent verison).
- SFML, spdlog and SFML's Linux dependencies are automatically installed with this CMake configuration.
- If SFML's Linux dependencies aren't installed automatically, run `install_SFML_linux_dependencies.sh` manually. If there is a problem with `ft2build.h`, include directory `/usr/include/freetype` or `/usr/include/freetype2`.

## Build
- Run CMake Configure `cmake --preset Release` or `Debug`.
- Build chessAi target `cmake --build ./build/Release --parallel 10 --target chess_ai`.
- Or just use VS Code CMake extensions.

This applies to all build configurations (Debug, Release).
- Warnings are treated as errors.
- Warning levels are increased (see CMakeLists.txt for compile flags).


## Developing on Windows
- Using **VS Code**, open VS Code with `scripts/msvc_env.bat` to setup the correct environment for MSVC or open Code through `Visual Studio Developer Command Prompt`.

## Contributing
- Make a branch with name that summaries what will be implemented and then make a merge request
to version2.
- Use camel case.
- Use **`.clang-format`** to format code.
- Set auto format on.
- `clang-format` minimum required version `14.0.0`.

## Literature
General: https://www.chessprogramming.org/Main_Page
Overview of the minimax engine: https://www.youtube.com/watch?v=w4FFX_otR-4&list=LL&index=2&t=313s
Bit Boards: https://ameye.dev/notes/chess-engine/
