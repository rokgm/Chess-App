#pragma once

#include "Move.h"
#include "TranspositionTable.h"

#include <map>
#include <optional>

namespace chessAi
{

class Timer
{
public:
    Timer(std::chrono::milliseconds timeLimit);
    void resetStartTime();
    bool timeUp() const;

private:
    std::chrono::milliseconds m_timeLimit;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};

struct PieceBitBoards;

/**
 * Chess engine using negamax approach.
 *
 * Search uses:
 *      Alpha-Beta pruning with move ordering.
 *      Transposition tables (Zobrist hashing).
 *      Iterative deepening.
 *
 * Evaluation is done with Evaluate class.
 * TODO: Implement Quiescence Search.
 */
class Engine
{
public:
    /**
     * Engine that terminates search at depth or time limit. Which ever is reached first.
     * Preferably just leave depth limit to 100 and limit by time.
     */
    Engine(const std::chrono::milliseconds& timeLimit, unsigned int depthLimit = 100);

    /**
     * @return Best move and depth to which the search was done.
     */
    std::pair<std::optional<Move>, unsigned int> findBestMove(const PieceBitBoards& bitBoards);

private:
    /**
     * Alpha-Beta pruning, alpha keeps best score current active color could achieve, beta keeps
     * opponents.
     */
    int negamax(const PieceBitBoards& bitBoards, unsigned int depth, int alpha, int beta);

    /**
     * Run iterative deepening, with ordered moves from previous search.
     * Return best move and true if move is shortest mate.
     */
    std::pair<Move, bool> iterativeDeepening(const PieceBitBoards& bitBoards, unsigned int depth);

    /**
     * Order from best to worst. We can (hopefully) prune more
     * branches if moves are order from best to worst in negamax.
     *
     * @param useTranspositions Set to false to not use transpositions. For example used in
     * quiescence search (not implemented yet).
     *
     * Checks for Most Valuable Victim - Least Valuable Aggressor, pawn promotion. (Moving to pawn
     * guarded square currently disabled because of performance test.)
     */
    [[nodiscard]] std::multimap<int, Move, std::greater<int>> orderMoves(
        const std::vector<Move>& moves, const PieceBitBoards& boards,
        bool useTranspositions = true);

    int evaluateEndGameType(const PieceBitBoards& boards, int depth);

    void runTimer();

private:
    TranspositionTable m_transpositionTable;
    unsigned int m_depthLimit;
    unsigned int m_currentIterativeDepth;
    unsigned int m_depthSearched;
    unsigned int m_countTranspositions;
    Timer m_timer;
    std::thread m_timerThread;
    std::atomic<bool> m_runSearch;
};

} // namespace chessAi