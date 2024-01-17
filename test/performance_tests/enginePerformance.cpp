/**
 * Run tests in release mode for accurate results.
 *
 * Results Rok's Laptop:
 *      Results with new evaluation, transpositions, iterative deepening:
 *          13/01/2024:
 *              getBestMove(depth = 4): average time = 118 ms
 *              getBestMove(depth = 5): average time = 241 ms
 *              getBestMove(depth = 6): average time = 630 ms
 *              getBestMove(timeLimit = 500 ms): average depth reached = 6.59722
 */

#include <gtest/gtest.h>

#include "core/Engine.h"
#include "core/PieceBitBoards.h"

#include <fstream>
#include <iostream>

namespace chessAi
{

std::vector<std::string> splitString(const std::string& input, char delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = input.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(delimiter, start);
    }

    tokens.push_back(input.substr(start, end));

    return tokens;
}

void runPerformanceTestDepth(int depth)
{
    std::chrono::milliseconds time(0);
    unsigned int count = 0;

    for (int i = 0; i < 3; ++i) {
        std::ifstream file("positions/mostly_middle_game_positions.epd");
        if (!file.is_open())
            FAIL() << "File with test positions couldn't be opened.";

        std::string line;
        while (std::getline(file, line)) {
            auto tokens = splitString(line, ';');
            tokens[0].pop_back();
            PieceBitBoards board(tokens[0]);

            // Initialize here, so transposition tables are cleared.
            Engine engine(std::chrono::milliseconds(1000000), depth);
            auto start = std::chrono::high_resolution_clock::now();
            engine.findBestMove(board);
            time += std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start);

            ++count;
        }
        file.close();
    }

    std::cout << "getBestMove(depth = " << depth << "): average time = " << time.count() / count
              << " ms" << std::endl;
}

void runPerformanceTestTime(std::chrono::milliseconds timeLimit)
{
    unsigned int count = 0;
    float depthSum = 0;

    for (int i = 0; i < 3; ++i) {
        std::ifstream file("positions/mostly_middle_game_positions.epd");
        if (!file.is_open())
            FAIL() << "File with test positions couldn't be opened.";

        std::string line;
        while (std::getline(file, line)) {
            auto tokens = splitString(line, ';');
            tokens[0].pop_back();
            PieceBitBoards board(tokens[0]);

            // Initialize here, so transposition tables are cleared.
            Engine engine(timeLimit);
            auto [move, depth] = engine.findBestMove(board);
            depthSum += static_cast<float>(depth);
            ++count;
        }
        file.close();
    }

    std::cout << "getBestMove(timeLimit = " << timeLimit.count() << " ms"
              << "): average depth reached = " << depthSum / static_cast<float>(count) << std::endl;
}

TEST(PerformanceOfFindBestMove, TestFixedDepth)
{
    runPerformanceTestDepth(4);
    runPerformanceTestDepth(5);
    runPerformanceTestDepth(6);
}

TEST(PerformanceOfFindBestMove, TestFixedTime)
{
    runPerformanceTestTime(std::chrono::milliseconds(500));
}

} // namespace chessAi
