#include "Engine.h"
#include "Evaluate.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "OpeningBook.h"
#include "Pawn.h"
#include "PieceBitBoards.h"

#include <algorithm>

namespace chessAi
{

Engine::Engine(bool useBook, const std::chrono::milliseconds& timeLimit, unsigned int depthLimit)
    : m_useOpeningBook(useBook), m_transpositionTable(), m_depthLimit(depthLimit),
      m_currentIterativeDepth(0), m_depthSearched(0), m_countTranspositions(0),
      m_countMaxCheckExtensions(0), m_timer(timeLimit), m_runSearch(false)
{
    if (m_useOpeningBook)
        m_useOpeningBook = OpeningBook::Init();
}

int Engine::evaluateEndGameType(const PieceBitBoards& bitBoards, int depth,
                                unsigned int numCheckExtensions)
{
    if (bitBoards.currentMoveColor == PieceColor::White) {
        // Must add depth so we find the shortest mate (mate pruning), also add check extensions.
        if (MoveGenerator<PieceColor::White>::isKingInCheck(bitBoards))
            return Evaluate::negativeMateScore + (m_currentIterativeDepth + numCheckExtensions) -
                   depth;
        return 0;
    }
    else {
        if (MoveGenerator<PieceColor::Black>::isKingInCheck(bitBoards))
            return Evaluate::negativeMateScore + (m_currentIterativeDepth + numCheckExtensions) -
                   depth;
        return 0;
    }
}

int Engine::quiescenceSearch(const PieceBitBoards& bitBoards, int alpha, int beta, int depth)
{
    if (!m_runSearch)
        return Evaluate::negativeInfinity;

    auto evaluation = Evaluate::getEvaluation(bitBoards);

    if (depth == 0)
        return evaluation;

    if (evaluation >= beta)
        return beta;
    alpha = std::max(evaluation, alpha);

    PieceBitBoards tempBoards = bitBoards;
    for (const auto& [moveScore, move] : orderMoves(
             MoveGeneratorWrapper::generateLegalMoves<MoveType::Capture>(bitBoards), bitBoards)) {
        tempBoards.applyMove(move);
        evaluation = -quiescenceSearch(tempBoards, -beta, -alpha, depth - 1);
        tempBoards = bitBoards;

        if (evaluation >= beta)
            return beta;
        alpha = std::max(evaluation, alpha);
    }

    return alpha;
}

int Engine::negamax(const PieceBitBoards& bitBoards, unsigned int depth, int alpha, int beta,
                    unsigned int numCheckExtensions,
                    const std::vector<uint64_t>& zobristKeysHistory)
{
    if (!m_runSearch)
        return Evaluate::negativeInfinity;

    int previousAlpha = alpha;

    auto tableEval = m_transpositionTable.getEntry(bitBoards.zobristKey);

    if (tableEval != nullptr && tableEval->depth >= depth) {
        m_countTranspositions++;
        if (tableEval->typeOfNode == TranspositionTable::TypeOfNode::exact) {
            return tableEval->evaluation;
        }
        else if (tableEval->typeOfNode == TranspositionTable::TypeOfNode::lower) {
            alpha = std::max(alpha, tableEval->evaluation);
        }
        else if (tableEval->typeOfNode == TranspositionTable::TypeOfNode::upper) {
            beta = std::min(beta, tableEval->evaluation);
        }
        else
            CHESS_LOG_ERROR("Evaluation in table with node type none.");
    }

    if (alpha >= beta)
        return tableEval->evaluation;

    if (depth == 0)
        // We pass alpha, beta and not -beta, -alpha because it is still our move.
        return quiescenceSearch(bitBoards, alpha, beta);

    std::vector<Move> moves = MoveGeneratorWrapper::generateLegalMoves<MoveType::Normal>(bitBoards);

    if (moves.empty())
        return evaluateEndGameType(bitBoards, depth, numCheckExtensions);

    int bestEvaluation = Evaluate::negativeInfinity;
    Move bestMove(0, 0, 0, 0);
    PieceBitBoards tempBoards = bitBoards;

    for (const auto& [moveScore, move] : orderMoves(moves, bitBoards)) {
        tempBoards.applyMove(move);

        int evaluation = 0;

        // Detect 3 fold repetition.
        if (std::count(zobristKeysHistory.begin(), zobristKeysHistory.end(),
                       tempBoards.zobristKey) < 1) {
            // Check extensions
            bool extension = false;
            // Limit check number of check extensions to 10.
            if (numCheckExtensions <= 9) {
                extension = (tempBoards.currentMoveColor == PieceColor::White)
                                ? MoveGenerator<PieceColor::White>::isKingInCheck(tempBoards)
                                : MoveGenerator<PieceColor::Black>::isKingInCheck(tempBoards);
            }
            m_countMaxCheckExtensions = std::max(numCheckExtensions, m_countMaxCheckExtensions);

            // Minus sign is needed because we evaluate the position from the perspective of current
            // move color. Good for the opponent, bad for us.
            evaluation = -negamax(tempBoards, depth - 1 + extension, -beta, -alpha,
                                  numCheckExtensions + extension, zobristKeysHistory);
        }

        if (evaluation > bestEvaluation) {
            bestEvaluation = evaluation;
            bestMove = move;
            alpha = std::max(evaluation, alpha);
        }

        if (alpha >= beta)
            break;

        tempBoards = bitBoards;
    }

    // Only store if leaf nodes were reached.
    if (m_runSearch && !(bestMove == Move(0, 0, 0, 0))) {
        auto nodeType = TranspositionTable::TypeOfNode::exact;
        if (bestEvaluation <= previousAlpha)
            nodeType = TranspositionTable::TypeOfNode::upper;
        else if (bestEvaluation >= beta)
            nodeType = TranspositionTable::TypeOfNode::lower;
        m_transpositionTable.store(bitBoards.zobristKey, bestEvaluation, depth, nodeType, bestMove);
    }
    return bestEvaluation;
}

std::pair<Move, bool> Engine::iterativeDeepening(const PieceBitBoards& bitBoards,
                                                 unsigned int depth,
                                                 const std::vector<uint64_t>& zobristKeysHistory)
{
    std::vector<Move> moves = MoveGeneratorWrapper::generateLegalMoves<MoveType::Normal>(bitBoards);

    int bestEvaluation = Evaluate::negativeMateScore;
    Move bestMove(0, 0, 0, 0);
    auto foundShortestMate = false;
    PieceBitBoards tempBoards = bitBoards;

    // Here we must guarantee that the best move from the previous iteration is searched first.
    for (const auto& [moveScore, move] : orderMoves(moves, bitBoards)) {
        tempBoards.applyMove(move);
        int evaluation = 0;

        // Detect 3 fold repetition.
        if (std::count(zobristKeysHistory.begin(), zobristKeysHistory.end(),
                       tempBoards.zobristKey) < 1) {
            // Check extensions
            bool extension = (tempBoards.currentMoveColor == PieceColor::White)
                                 ? MoveGenerator<PieceColor::White>::isKingInCheck(tempBoards)
                                 : MoveGenerator<PieceColor::Black>::isKingInCheck(tempBoards);
            evaluation = -negamax(tempBoards, depth - 1 + extension, -Evaluate::infinity,
                                  -bestEvaluation, extension, zobristKeysHistory);
        }

        // If search was canceled, evaluation from this negamax search didn't reach leaf nodes,
        // evaluation is useless.
        if (!m_runSearch)
            break;

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

    // Important for move ordering in iterative deepening, search previous move first. Do not store
    // false evaluation.
    if (m_runSearch && !(bestMove == Move(0, 0, 0, 0))) {
        m_transpositionTable.store(bitBoards.zobristKey, bestEvaluation, depth,
                                   TranspositionTable::TypeOfNode::exact, bestMove);
        CHESS_LOG_INFO("Iterative deepening depth {} search evaluation: {}", depth, bestEvaluation);
    }

    return {bestMove, foundShortestMate};
}

namespace
{

std::optional<Move> getBookMove(const std::vector<Move>& movesHistory,
                                const PieceBitBoards& bitBoards)
{
    try {
        auto bookMove = OpeningBook::getBookMove(movesHistory);

        if (!bookMove.has_value())
            return {};

        // Check if legal. Return generated move which has flags set.
        for (const auto& generatedMove :
             MoveGeneratorWrapper::generateLegalMoves<MoveType::Normal>(bitBoards)) {
            SpecialMoveCompare compare(*bookMove);
            if (compare(generatedMove))
                return generatedMove;
        }
        CHESS_LOG_WARN("Book move is not legal.");
        return {};
    }
    catch (const std::exception& ex) {
        CHESS_LOG_ERROR("Unhandled error when getting book move: {}", ex.what());
        return {};
    }
}

} // namespace

std::pair<std::optional<Move>, unsigned int> Engine::findBestMove(
    const PieceBitBoards& bitBoards, const std::vector<uint64_t>& zobristKeysHistory,
    const std::vector<Move>& movesHistory)
{
    CHESS_LOG_INFO("Half move count: {}", bitBoards.halfMoveCount);

    if (m_useOpeningBook) {
        auto move = getBookMove(movesHistory, bitBoards);
        if (move.has_value())
            return {*move, 0};
    }

    m_runSearch = true;
    m_timer.resetStartTime();
    m_timerThread = std::thread(&Engine::runTimer, this);

    Move bestMove(0, 0, 0, 0);

    // Iterative deepening
    for (unsigned int depth = 1; depth <= m_depthLimit; depth++) {
        if (!m_runSearch)
            break;
        m_currentIterativeDepth = depth;
        auto [bestMoveThisIteration, isShortestMate] =
            iterativeDeepening(bitBoards, depth, zobristKeysHistory);

        m_depthSearched = depth;
        // We can update previous move even if search was canceled, because best move from
        // previous iteration is searched first (and next move in the search must be searched to
        // the leafs). We still have to check for null move, as it can be returned, if iterative
        // deepening was canceled during first iteration.
        if (bestMoveThisIteration == Move(0, 0, 0, 0))
            continue;
        bestMove = bestMoveThisIteration;
        if (isShortestMate)
            break;
    }

    CHESS_LOG_INFO("Number of transpositions: {}", m_countTranspositions);
    CHESS_LOG_INFO("Number of max check extension: {}", m_countMaxCheckExtensions);
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

void pawnDefendedScore(Move move, int& moveScore, const PieceBitBoards& boards,
                       PieceFigure movingPiece)
{
    uint64_t positionMask = 0;
    uint64_t attack = 0;
    if (boards.currentMoveColor == PieceColor::White) {
        PieceBitBoards::setBit(positionMask, move.destination);
        for (auto position : boards.blackPawnPositions) {
            attack |= Pawn<PieceColor::Black>::originToAttacks[position];
        }
        if ((attack & positionMask) != 0)
            moveScore -= Evaluate::getFigureValue(movingPiece);
    }
    else {
        PieceBitBoards::setBit(positionMask, move.destination);
        for (auto position : boards.whitePawnPositions) {
            attack |= Pawn<PieceColor::White>::originToAttacks[position];
        }
        if ((attack & positionMask) != 0)
            moveScore -= Evaluate::getFigureValue(movingPiece);
    }
}

} // namespace

[[nodiscard]] std::multimap<int, Move, std::greater<int>> Engine::orderMoves(
    const std::vector<Move>& moves, const PieceBitBoards& boards, bool useTranspositions)
{
    Move bestMove(0, 0, 0, 0);

    // Important so best move from previous search is searched first.
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
        pawnDefendedScore(move, moveScore, boards, movingPiece.getPieceFigure());

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