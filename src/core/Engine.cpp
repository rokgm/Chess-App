#include "Engine.h"
#include "Evaluate.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Pawn.h"
#include "PieceBitBoards.h"

namespace chessAi
{

Engine::Engine(const std::chrono::milliseconds& timeLimit, unsigned int depthLimit)
    : m_transpositionTable(), m_depthLimit(depthLimit), m_currentIterativeDepth(0),
      m_depthSearched(0), m_countTranspositions(0), m_timer(timeLimit), m_runSearch(false)
{
}

int Engine::evaluateEndGameType(const PieceBitBoards& bitBoards, int depth)
{
    if (bitBoards.currentMoveColor == PieceColor::White) {
        // Must add depth so we find the shortest mate (mate pruning).
        if (MoveGenerator<PieceColor::White>::isKingInCheck(bitBoards))
            return Evaluate::negativeMateScore + m_currentIterativeDepth - depth;
        return 0;
    }
    else {
        if (MoveGenerator<PieceColor::Black>::isKingInCheck(bitBoards))
            return Evaluate::negativeMateScore + m_currentIterativeDepth - depth;
        return 0;
    }
}

int Engine::negamax(const PieceBitBoards& bitBoards, unsigned int depth, int alpha, int beta)
{
    if (!m_runSearch)
        return 0;

    int previousAlpha = alpha;

    auto tableEval = m_transpositionTable.getEntry(bitBoards.zobristKey);

    if (tableEval != nullptr && tableEval->depth >= depth) {
        if (tableEval->typeOfNode == TranspositionTable::TypeOfNode::exact) {
            m_countTranspositions++;
            return tableEval->evaluation;
        }
        else if (tableEval->typeOfNode == TranspositionTable::TypeOfNode::lower) {
            alpha = std::max(alpha, tableEval->evaluation);
            m_countTranspositions++;
        }
        else if (tableEval->typeOfNode == TranspositionTable::TypeOfNode::upper) {
            beta = std::min(beta, tableEval->evaluation);
            m_countTranspositions++;
        }
        else
            CHESS_LOG_ERROR("Evaluation in table with node type none.");
    }

    if (alpha >= beta)
        return tableEval->evaluation;

    if (depth == 0)
        return Evaluate::getEvaluation(bitBoards);

    std::vector<Move> moves = MoveGeneratorWrapper::generateLegalMoves(bitBoards);

    if (moves.empty())
        return evaluateEndGameType(bitBoards, depth);

    int bestEvaluation = Evaluate::negativeInfinity;
    Move bestMove(0, 0, 0, 0);
    PieceBitBoards tempBoards = bitBoards;

    for (const auto& [moveScore, move] : orderMoves(moves, bitBoards)) {
        tempBoards.applyMove(move);
        // Minus sign is needed because we evaluate the position from the perspective of current
        // move color. Good for the opponent, bad for us (minus sign).
        auto evaluation = -negamax(tempBoards, depth - 1, -beta, -alpha);

        if (evaluation > bestEvaluation) {
            bestEvaluation = evaluation;
            bestMove = move;
            alpha = std::max(evaluation, alpha);
        }

        if (alpha >= beta)
            break;

        tempBoards = bitBoards;
    }

    auto nodeType = TranspositionTable::TypeOfNode::exact;
    if (bestEvaluation <= previousAlpha)
        nodeType = TranspositionTable::TypeOfNode::upper;
    else if (bestEvaluation >= beta)
        nodeType = TranspositionTable::TypeOfNode::lower;
    m_transpositionTable.store(bitBoards.zobristKey, bestEvaluation, depth, nodeType, bestMove);

    return bestEvaluation;
}

std::pair<Move, bool> Engine::iterativeDeepening(const PieceBitBoards& bitBoards,
                                                 unsigned int depth)
{
    std::vector<Move> moves = MoveGeneratorWrapper::generateLegalMoves(bitBoards);

    int bestEvaluation = Evaluate::negativeMateScore;
    Move bestMove(0, 0, 0, 0);
    auto foundShortestMate = false;
    PieceBitBoards tempBoards = bitBoards;

    for (const auto& [moveScore, move] : orderMoves(moves, bitBoards)) {
        tempBoards.applyMove(move);
        int evaluation = -negamax(tempBoards, depth - 1, -Evaluate::infinity, -bestEvaluation);

        if (evaluation > bestEvaluation) {
            bestEvaluation = evaluation;
            bestMove = move;
        }

        if (bestEvaluation >= Evaluate::mateScore - static_cast<int>(m_currentIterativeDepth)) {
            foundShortestMate = true;
            break;
        }

        tempBoards = bitBoards;
    }

    // Important for move ordering in iterative deepening.
    m_transpositionTable.store(bitBoards.zobristKey, bestEvaluation, depth,
                               TranspositionTable::TypeOfNode::exact, bestMove);

    CHESS_LOG_TRACE("Iterative deepening depth {} search evaluation: {}", depth, bestEvaluation);

    return {bestMove, foundShortestMate};
}

std::pair<std::optional<Move>, unsigned int> Engine::findBestMove(const PieceBitBoards& bitBoards)
{
    m_transpositionTable.clear();

    m_runSearch = true;
    m_timer.resetStartTime();
    m_timerThread = std::thread(&Engine::runTimer, this);

    CHESS_LOG_TRACE("Half move count: {}", bitBoards.halfMoveCount);

    Move bestMove(0, 0, 0, 0);

    // Iterative deepening
    for (unsigned int depth = 1; depth <= m_depthLimit; depth++) {
        if (!m_runSearch)
            break;
        m_currentIterativeDepth = depth;
        auto [bestMoveThisIteration, isShortestMate] = iterativeDeepening(bitBoards, depth);

        // Update best move only if search wasn't canceled during iteration.
        if (m_runSearch) {
            bestMove = bestMoveThisIteration;
            m_depthSearched = depth;
            if (isShortestMate)
                break;
        }
    }

    CHESS_LOG_TRACE("Number of transpositions: {}", m_countTranspositions);
    m_runSearch = false;
    m_timerThread.join();
    return {bestMove, m_depthSearched};
}

namespace
{

void scorePromotion(Move move, int& moveScore)
{
    if (move.specialMoveFlag == 1) {
        if (move.promotion == 0)
            moveScore += Evaluate::getFigureValue(PieceFigure::Knight);
        if (move.promotion == 1)
            moveScore += Evaluate::getFigureValue(PieceFigure::Bishop);
        if (move.promotion == 2)
            moveScore += Evaluate::getFigureValue(PieceFigure::Rook);
        if (move.promotion == 3)
            moveScore += Evaluate::getFigureValue(PieceFigure::Queen);
    }
}

#if 0
void pawnDefendedScore(Move move, int& moveScore, const PieceBitBoards& boards,
                       PieceFigure movingPiece)
{
    uint64_t positionMask = 0;
    uint64_t attack = 0;
    if (boards.currentMoveColor == PieceColor::White) {
        PieceBitBoards::setBit(positionMask, move.destination);
        for (auto position : PieceBitBoards::getSetBitPositions(boards.blackPawns)) {
            attack |= Pawn<PieceColor::Black>::originToAttacks[position];
        }
        if ((attack & positionMask) != 0)
            moveScore -= Evaluate::getFigureValue(movingPiece);
    }
    else {
        PieceBitBoards::setBit(positionMask, move.destination);
        for (auto position : PieceBitBoards::getSetBitPositions(boards.whitePawns)) {
            attack |= Pawn<PieceColor::White>::originToAttacks[position];
        }
        if ((attack & positionMask) != 0)
            moveScore -= Evaluate::getFigureValue(movingPiece);
    }
}
#endif

} // namespace

[[nodiscard]] std::multimap<int, Move, std::greater<int>> Engine::orderMoves(
    const std::vector<Move>& moves, const PieceBitBoards& boards, bool useTranspositions)
{
    Move bestMove(0, 0, 0, 0);

    if (useTranspositions) {
        auto entry = m_transpositionTable.getEntry(boards.zobristKey);
        if (entry != nullptr)
            bestMove = entry->bestMove;
    }

    std::multimap<int, Move, std::greater<int>> sortedMoves;

    for (auto move : moves) {
        int moveScore = 0;

        if (move == bestMove) {
            moveScore = 100000;
            sortedMoves.insert({moveScore, move});
            continue;
        }

        auto movingPiece = boards.getPieceTypeWithSetBitAtPosition(move.origin);
        if (movingPiece.getPieceFigure() == PieceFigure::Empty) {
            CHESS_LOG_ERROR("No piece at origin of move.");
            continue;
        }
        auto capturedPiece = boards.getPieceTypeWithSetBitAtPosition(move.destination);

        // MVV-LVA, offset a bit so captures still score high
        if (capturedPiece.getPieceFigure() != PieceFigure::Empty)
            moveScore = Evaluate::getFigureValue(capturedPiece.getPieceFigure()) -
                        Evaluate::getFigureValue(movingPiece.getPieceFigure());

        scorePromotion(move, moveScore);
// Performance test showed no improvement or even a bit worse performance with pawn check.
#if 0
        pawnDefendedScore(move, moveScore, boards, movingPiece);
#endif

        sortedMoves.insert({moveScore, move});
    }
    return sortedMoves;
}

void Engine::runTimer()
{
    // Check in case it is modified elsewhere.
    while (m_runSearch) {
        if (m_timer.timeUp()) {
            m_runSearch = false;
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

Timer::Timer(std::chrono::milliseconds timeLimit)
    : m_timeLimit(timeLimit), m_startTime(std::chrono::high_resolution_clock::now())
{
}

bool Timer::timeUp() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now() - m_startTime) >= m_timeLimit;
}

void Timer::resetStartTime()
{
    m_startTime = std::chrono::high_resolution_clock::now();
}

} // namespace chessAi