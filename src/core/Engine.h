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
 */
class Engine
{
public:
    /**
     * Engine that terminates search at depth or time limit. Which ever is reached first.
     * Preferably just leave depth limit to 100 and limit by time.
     */
    Engine(bool useBook, const std::chrono::milliseconds& timeLimit, unsigned int depthLimit = 100);

    /**
     * @param zobristKeysHistory Used to detect 3 fold repetition.
     * @param movesHistory Used for book moves.
     *
     * @return Best move and depth to which the search was done.
     * Depth search is from iterative deepening.
     */
    std::pair<std::optional<Move>, unsigned int> findBestMove(
        const PieceBitBoards& bitBoards, const std::vector<uint64_t>& zobristKeysHistory,
        const std::vector<Move>& movesHistory);

private:
    /**
     * Alpha-Beta pruning, alpha keeps best score current active color could achieve, beta keeps
     * opponents.
     *
     * If search is canceled during the search, return positive or negative infinity evaluation.
     */
    int negamax(const PieceBitBoards& bitBoards, unsigned int depth, int alpha, int beta,
                unsigned int numCheckExtensions, const std::vector<uint64_t>& zobristKeysHistory);

    /**
     * Run iterative deepening, with ordered moves from previous search.
     * Return best move and true if move is shortest mate.
     *
     * Because we order moves, best move from previous search is searched first. In that case we can
     * update best move even if search for this iteration depth was not completed fully. Current
     * move is better than previous best move.
     */
    std::pair<Move, bool> iterativeDeepening(const PieceBitBoards& bitBoards, unsigned int depth,
                                             const std::vector<uint64_t>& zobristKeysHistory);

    /**
     * Order from best to worst. We can (hopefully) prune more
     * branches if moves are order from best to worst in negamax.
     *
     * @param useTranspositions Set to false to not use transpositions.
     *
     * Checks for Most Valuable Victim - Least Valuable Aggressor, pawn promotion. (Moving to pawn
     * guarded square currently disabled because of performance test.)
     */
    [[nodiscard]] std::multimap<int, Move, std::greater<int>> orderMoves(
        const std::vector<Move>& moves, const PieceBitBoards& boards,
        bool useTranspositions = true);

    int evaluateEndGameType(const PieceBitBoards& boards, int depth,
                            unsigned int numCheckExtensions);

    /**
     * Search position until quite and then return evaluation. Depth is the limit of captures
     * search.
     */
    int quiescenceSearch(const PieceBitBoards& bitBoards, int alpha, int beta, int depth = 20);

    void runTimer();

private:
    bool m_useOpeningBook;
    TranspositionTable m_transpositionTable;
    unsigned int m_depthLimit;
    unsigned int m_currentIterativeDepth;
    unsigned int m_depthSearched;
    unsigned int m_countTranspositions;
    unsigned int m_countMaxCheckExtensions;
    Timer m_timer;
    std::thread m_timerThread;
    std::atomic<bool> m_runSearch;
};

} // namespace chessAi