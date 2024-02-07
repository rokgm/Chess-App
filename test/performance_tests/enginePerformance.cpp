/**
 * Run tests in release mode for accurate results.
 *
 * Results Rok's Laptop:
 *      Results with new evaluation, transpositions, iterative deepening,
 *      quiescence search, check extensions:
 *          21/01/2024:
 *              getBestMove(depth = 4): average time = 1373 ms
 *              getBestMove(timeLimit = 500 ms): average depth reached = 5.58
 *          21/01/2024, store positions in a vector (contigious):
 *              getBestMove(depth = 4): average time = 581 ms
 *              getBestMove(timeLimit = 500 ms): average depth reached = 6.4
 *          21/01/2024, pawn defended check in ordering:
 *              getBestMove(depth = 4): average time = 233 ms
 *              getBestMove(timeLimit = 500 ms): average depth reached = 6.4
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

void runPerformanceTestDepth(int depth, std::string& result)
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

            // Initialize here, so transposition tables are cleared (independent results).
            Engine engine(false, std::chrono::milliseconds(1000000), depth);
            auto start = std::chrono::high_resolution_clock::now();
            engine.findBestMove(board, {}, {});
            time += std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start);

            ++count;
        }
        file.close();
    }

    result = "getBestMove(depth = " + std::to_string(depth) +
             "): average time = " + std::to_string(time.count() / count) + " ms";
}

void runPerformanceTestTime(std::chrono::milliseconds timeLimit, std::string& result)
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
            Engine engine(false, timeLimit);
            auto [move, depth] = engine.findBestMove(board, {}, {});
            depthSum += static_cast<float>(depth);
            ++count;
        }
        file.close();
    }

    result = "getBestMove(timeLimit = " + std::to_string(timeLimit.count()) + " ms" +
             "): average depth reached = " + std::to_string(depthSum / static_cast<float>(count));
}

// The test log is long because of logging in each iteration, scroll to the and to see the result.
TEST(PerformanceOfFindBestMove, TestFixedDepth)
{
    std::string result4;
    runPerformanceTestDepth(4, result4);
    std::cout << result4 << '\n';
}

TEST(PerformanceOfFindBestMove, TestFixedTime)
{
    std::string result;
    runPerformanceTestTime(std::chrono::milliseconds(500), result);
    std::cout << result << '\n';
}

} // namespace chessAi
