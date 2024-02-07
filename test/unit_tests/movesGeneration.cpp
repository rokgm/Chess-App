/**
 * Run tests in release mode, otherwise takes a lot of time.
 *
 */

#include <gtest/gtest.h>

#include "core/MoveGenerator.h"

#include <fstream>
#include <iostream>

namespace chessAi
{

uint64_t sumNodes(const std::map<std::string, uint64_t>& map)
{
    uint64_t x = 0;
    for (const auto& [key, value] : map) {
        x += value;
    }
    return x;
}

std::string convertMoveToString(Move move)
{
    std::string string = "";

    if (move.origin < 10) {
        string.append("0");
        string.append(std::to_string(move.origin));
    }
    else
        string.append(std::to_string(move.origin));
    if (move.destination < 10) {
        string.append("0");
        string.append(std::to_string(move.destination));
    }
    else
        string.append(std::to_string(move.destination));
    string.append(std::to_string(move.promotion));
    string.append(std::to_string(move.specialMoveFlag));
    return string;
}

Move convertStringToMove(const std::string& string)
{
    return Move(static_cast<uint16_t>(std::stoi(string.substr(0, 2))),
                static_cast<uint16_t>(std::stoi(string.substr(2, 2))),
                static_cast<uint16_t>(std::stoi(string.substr(4, 1))),
                static_cast<uint16_t>(std::stoi(string.substr(5, 1))));
}

std::string fieldNumberToBoardNotation(uint16_t number)
{
    std::vector<std::string> letters = {"a", "b", "c", "d", "e", "f", "g", "h"};
    return letters[number % 8] + std::to_string((63 - number) / 8 + 1);
}

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

uint64_t perft(const PieceBitBoards boards, int depth)
{
    auto moves = MoveGeneratorWrapper::generateLegalMoves<MoveType::Normal>(boards);
    uint64_t nodes = 0;
    if (depth == 1)
        return moves.size();

    PieceBitBoards tempBoards = boards;
    for (auto move : moves) {
        tempBoards.applyMove(move);
        nodes += perft(tempBoards, depth - 1);
        tempBoards = boards;
    }
    return nodes;
}

std::map<std::string, uint64_t> perftDivided(const PieceBitBoards boards, int depth)
{
    std::map<std::string, uint64_t> dividedMap;
    for (auto move : MoveGeneratorWrapper::generateLegalMoves<MoveType::Normal>(boards)) {
        dividedMap.insert({convertMoveToString(move), 1});
    }

    if (depth == 1)
        return dividedMap;

    PieceBitBoards tempBoards = boards;
    for (auto& [moveString, nodes] : dividedMap) {
        tempBoards.applyMove(convertStringToMove(moveString));
        nodes = perft(tempBoards, depth - 1);
        tempBoards = boards;
    }

    return dividedMap;
}

TEST(Perft, StartingPosition)
{
    PieceBitBoards board1;
    EXPECT_EQ(perft(board1, 1), 20);

    // Test divided perft
    for (const auto& [moveString, nodes] : perftDivided(board1, 1)) {
        EXPECT_EQ(nodes, 1);
    }
    auto divided1 = perftDivided(board1, 1);
    EXPECT_EQ(divided1.size(), 20);
    EXPECT_EQ(sumNodes(divided1), 20);

    PieceBitBoards board4;
    EXPECT_EQ(perft(board4, 4), 197281);
    EXPECT_EQ(sumNodes(perftDivided(board4, 4)), 197281);

    // Takes more time, run in release mode.
    PieceBitBoards board5;
    auto divided5 = perftDivided(board5, 5);
    EXPECT_EQ(sumNodes(divided5), 4865609);
}

TEST(Perft, EnPassant)
{
    PieceBitBoards board("rnbqkbnr/ppp1p1pp/5p2/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
    EXPECT_EQ(perft(board, 1), 32);

    board = PieceBitBoards("rnbqkbnr/ppp1pppp/8/8/3pPP2/8/PPPP2PP/RNBQKBNR b KQkq e3 0 3");
    EXPECT_EQ(perft(board, 1), 30);
}

TEST(Perft, Promotion)
{
    // http://www.rocechess.ch/perft.html
    PieceBitBoards board("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 11");
    auto divided1 = perftDivided(board, 3);
    EXPECT_EQ(sumNodes(divided1), 9483);
}

TEST(Perft, TestManyPositions)
{
    // http://www.rocechess.ch/perft.html and 2 more added by hand

    std::ifstream file("perft_positions/perftsuite.epd");

    if (!file.is_open())
        FAIL() << "File with perft test positions couldn't be opened.";

    std::string line;
    while (std::getline(file, line)) {
        auto tokens = splitString(line, ';');
        tokens[0].pop_back();
        PieceBitBoards board(tokens[0]);

        // Set depth to desired level, takes a lot of time above 4.
        int depth = 4;
        if (static_cast<size_t>(depth) >= tokens.size())
            depth = 4;

        uint64_t count = std::stoi(splitString(tokens[depth], ' ')[1]);
        EXPECT_EQ(perft(board, depth), count);
    }

    file.close();
}

TEST(Perft, TestThatFailedOnDepth5_190millionMoves)
{
    PieceBitBoards board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    board.applyMove(Move(28, 18, 0, 0));

    auto divided = perftDivided(board, 4);
    EXPECT_EQ(sumNodes(divided), 4083458);

    board.applyMove(Move(16, 25, 0, 0));
    divided = perftDivided(board, 3);
    EXPECT_EQ(sumNodes(divided), 101489);

    board.applyMove(Move(48, 32, 0, 0));
    divided = perftDivided(board, 2);
    EXPECT_EQ(sumNodes(divided), 2217);

    board.applyMove(Move(8, 16, 0, 0));
    divided = perftDivided(board, 1);
    EXPECT_EQ(sumNodes(divided), 51);

    for (auto [move, count] : divided) {
        std::cout << fieldNumberToBoardNotation(convertStringToMove(move).origin)
                  << fieldNumberToBoardNotation(convertStringToMove(move).destination) << ": "
                  << count << '\n';
    }
}

TEST(Perft, Captures)
{
    PieceBitBoards board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto moves = MoveGeneratorWrapper::generateLegalMoves<MoveType::Capture>(board);
    EXPECT_EQ(moves.size(), 8);

    PieceBitBoards board4("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1");
    auto moves4 = MoveGeneratorWrapper::generateLegalMoves<MoveType::Capture>(board4);
    EXPECT_EQ(moves4.size(), 7);

    PieceBitBoards board1("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    auto moves1 = MoveGeneratorWrapper::generateLegalMoves<MoveType::Capture>(board1);
    EXPECT_EQ(moves1.size(), 1);

    PieceBitBoards board2("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    auto moves2 = MoveGeneratorWrapper::generateLegalMoves<MoveType::Capture>(board2);
    EXPECT_EQ(moves2.size(), 0);

    PieceBitBoards board3("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
    auto moves3 = MoveGeneratorWrapper::generateLegalMoves<MoveType::Capture>(board3);
    EXPECT_EQ(moves3.size(), 0);
}

} // namespace chessAi
