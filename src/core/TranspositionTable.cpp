#include "TranspositionTable.h"

namespace chessAi
{

TranspositionTable::Entry::Entry()
    : key(0), evaluation(0), depth(0), typeOfNode(TypeOfNode::none), bestMove(Move(0, 0, 0, 0))
{
}

TranspositionTable::Entry::Entry(uint64_t key, int evaluation, unsigned int depth,
                                 TypeOfNode typeOfNode, Move bestMove)
    : key(key), evaluation(evaluation), depth(depth), typeOfNode(typeOfNode), bestMove(bestMove)
{
}

size_t TranspositionTable::hashFunction(uint64_t key)
{
    return key % s_numberOfEntires;
}

TranspositionTable::TranspositionTable()
    : m_table(std::make_unique<std::array<Entry, s_numberOfEntires>>())
{
}

void TranspositionTable::store(uint64_t zobristHash, int evaluation, unsigned int depth,
                               TypeOfNode typeOfNode, Move bestMove)
{
    m_table->at(hashFunction(zobristHash)) =
        std::move(Entry(zobristHash, evaluation, depth, typeOfNode, bestMove));
}

const TranspositionTable::Entry* TranspositionTable::getEntry(uint64_t zobristHash)
{
    auto entry = &m_table->at(hashFunction(zobristHash));
    if (entry->key == zobristHash)
        return entry;
    return nullptr;
}

void TranspositionTable::clear()
{
    m_table->fill(Entry());
}

} // namespace chessAi