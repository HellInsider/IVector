#include "../include/ISetControlBlock.h"
#include "../include/IVector.h"
#include "../myHeaders/SetImpl.h"
#include <cstddef>

RC SetImplControlBlock::getNext(IVector *const &vec, size_t &index, size_t indexInc) const
{
    return set->getNextByUniqueIndex(vec, index, indexInc);
}

RC SetImplControlBlock::getPrevious(IVector *const &vec, size_t &index, size_t indexInc) const
{
    return set->getPrevByUniqueIndex(vec, index, indexInc);
}

RC SetImplControlBlock::getBegin(IVector *const &vec, size_t &index) const
{
    return set->getFirstByUniqueIndex(vec, index);
}

RC SetImplControlBlock::getEnd(IVector *const &vec, size_t &index) const
{
    return set->getLastByUniqueIndex(vec, index);
}

SetImplControlBlock *SetImplControlBlock::createControlBlock(SetImpl *const &set)
{
    return new (std::nothrow) SetImplControlBlock(set);
}

SetImplControlBlock::SetImplControlBlock(SetImpl *const &set) : set(set) {};

ISetControlBlock::~ISetControlBlock() = default;
