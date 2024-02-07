#include "OpeningBook.h"

#include <algorithm>
#include <fstream>
#include <optional>
#include <random>

namespace chessAi
{

namespace
{

std::optional<uint16_t> fieldNotationToPosition(const std::string& field)
{
    if (field.length() != 2) {
        CHESS_LOG_ERROR("Book field is not length 2.");
        return {};
    }

    int x = field[0] - 97;
    if (x < 0 || x > 7) {
        CHESS_LOG_ERROR("Book field algebraic notation error: file. {}", field[0]);
        return {};
    }
    if (!std::isdigit(field[1])) {
        CHESS_LOG_ERROR("Book field algebraic notation error: rank. {}", field[1]);
        return {};
    }

    int y = field[1] - '0';
    y = 8 - y;
    return static_cast<uint16_t>(x + 8 * y);
}

Move uciNotationToMove(const std::string& notation)
{
    auto origin = fieldNotationToPosition(notation.substr(0, 2));
    if (!origin.has_value()) {
        CHESS_LOG_ERROR("Book parsing error, move doesn't have an origin.");
        return Move(0, 0, 0, 0);
    }
    auto destination = fieldNotationToPosition(notation.substr(2, 2));
    if (!destination.has_value()) {
        CHESS_LOG_ERROR("Book parsing error, move doesn't have an origin.");
        return Move(0, 0, 0, 0);
    }

    return Move(*origin, *destination, 0, 0);
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

auto findMoveEntry(const std::vector<std::unique_ptr<BookEntry>>& nextMoves, Move move)
{
    for (auto it = nextMoves.begin(); it != nextMoves.end(); it++) {
        if (*it == nullptr)
            continue;
        // We use special move compare as we only care about origin and destination.
        SpecialMoveCompare compare(move);
        if (compare((*it)->move)) {
            return it;
        }
    }
    return nextMoves.end();
}

void storeEntries(const std::vector<Move>& moves, BookEntry& entry, unsigned int halfMoveCount)
{
    if (moves.size() < halfMoveCount || halfMoveCount < 1)
        return;

    // Find the move in next moves or store it.
    auto nextMovesEntry = findMoveEntry(entry.nextMoves, moves[halfMoveCount - 1]);
    if (nextMovesEntry == entry.nextMoves.end()) {
        std::unique_ptr<BookEntry>& inserted =
            entry.nextMoves.emplace_back(std::make_unique<BookEntry>(moves[halfMoveCount - 1]));
        storeEntries(moves, *inserted, halfMoveCount + 1);
    }
    else
        storeEntries(moves, **nextMovesEntry, halfMoveCount + 1);
}

} // namespace

bool OpeningBook::Init()
{
    try {
        std::ifstream file("book/book.csv");

        if (!file.is_open()) {
            CHESS_LOG_ERROR("Couldn't open book/book.csv.");
            return false;
        }

        s_book = std::make_unique<BookEntry>();
        s_book->move = Move(0, 0, 0, 0);

        std::string line;
        while (std::getline(file, line)) {

            std::vector<Move> moves;
            auto movesNotation = splitString(line, ',');
            moves.reserve(movesNotation.size());
            std::transform(movesNotation.begin(), movesNotation.end(), std::back_inserter(moves),
                           uciNotationToMove);

            storeEntries(moves, *s_book, 1);
        }
        file.close();
        return true;
    }
    catch (const std::exception& ex) {
        CHESS_LOG_ERROR("When initializing book moves ex: {}", ex.what());
        s_book = nullptr;
        return false;
    }
}

namespace
{

Move traverseBookTree(const std::vector<Move>& movesHistory, const BookEntry& entry,
                      unsigned int halfMoveCount)
{
    if (halfMoveCount < 1) {
        CHESS_LOG_ERROR("Book half move count smaller then 1.");
        return Move(0, 0, 0, 0);
    }

    // If next move exists, return random one.
    if (movesHistory.size() == halfMoveCount - 1) {
        if (entry.nextMoves.empty())
            return Move(0, 0, 0, 0);

        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, entry.nextMoves.size() - 1);
        return entry.nextMoves.at(dist(gen))->move;
    }

    auto nextMovesEntry = findMoveEntry(entry.nextMoves, movesHistory[halfMoveCount - 1]);
    if (nextMovesEntry == entry.nextMoves.end())
        return Move(0, 0, 0, 0);
    else
        return traverseBookTree(movesHistory, **nextMovesEntry, halfMoveCount + 1);
}

} // namespace

std::optional<Move> OpeningBook::getBookMove(const std::vector<Move>& movesHistory)
{
    if (s_book == nullptr)
        return {};

    auto move = traverseBookTree(movesHistory, *s_book, 1);
    if (move == Move(0, 0, 0, 0))
        return {};
    return move;
}

} // namespace chessAi