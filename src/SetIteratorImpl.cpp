#include "../include/ISet.h"
#include "../myHeaders/SetImpl.h"
#include <stdexcept>

SetImpl::IteratorImpl::IteratorImpl(SetImplControlBlock *const &cb, size_t index, IVector *vector)
{
    control_block = cb;
    cur_unique_idx = index;
    cur_vector = vector;
    valid = true;
}

ISet::IIterator *SetImpl::IteratorImpl::getNext(size_t indexInc) const
{
    IIterator *copy = clone();
    if (copy == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    copy->next(indexInc);
    return copy;
}

ISet::IIterator *SetImpl::IteratorImpl::getPrevious(size_t indexInc) const
{
    IIterator *copy = clone();
    if (copy == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    copy->previous(indexInc);
    return copy;
}

ISet::IIterator *SetImpl::IteratorImpl::clone() const
{
    return new (std::nothrow) IteratorImpl(control_block, cur_unique_idx, cur_vector->clone());
}

RC SetImpl::IteratorImpl::setLogger(ILogger *const pLogger)
{
    if (pLogger == nullptr)
    {
        return RC::NULLPTR_ERROR;
    }

    logger = pLogger;
    return RC::SUCCESS;
}

RC SetImpl::IteratorImpl::next(size_t indexInc)
{
    RC err = control_block->getNext(cur_vector, cur_unique_idx, indexInc);
    if (err == RC::INDEX_OUT_OF_BOUND)
    {
        valid = false;
    }
    return err;
}

RC SetImpl::IteratorImpl::previous(size_t indexInc)
{
    RC err = control_block->getPrevious(cur_vector, cur_unique_idx, indexInc);
    if (err == RC::INDEX_OUT_OF_BOUND)
    {
        valid = false;
    }

    return err;
}

bool SetImpl::IteratorImpl::isValid() const
{
    return valid;
}

RC SetImpl::IteratorImpl::makeBegin()
{
    return control_block->getBegin(cur_vector, cur_unique_idx);
}

RC SetImpl::IteratorImpl::makeEnd()
{
    return control_block->getEnd(cur_vector, cur_unique_idx);
}

RC SetImpl::IteratorImpl::getVectorCopy(IVector *&val) const
{
    IVector *copy = cur_vector->clone();
    if (copy == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    val = copy;
    return RC::SUCCESS;
}

RC SetImpl::IteratorImpl::getVectorCoords(IVector *const &val) const
{
    return val->setData(cur_vector->getDim(), cur_vector->getData());
}

SetImpl::IteratorImpl::~IteratorImpl()
{
    delete cur_vector;
}

ISet::IIterator::~IIterator() = default;

ISet::IIterator *SetImpl::getIterator(size_t index) const
{
    if (index > size - 1)
    {
        logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    IVector *vec;
    RC err = getCopy(index, vec);
    if (err != RC::SUCCESS)
    {
        logger->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IteratorImpl *iter = new (std::nothrow) IteratorImpl(control_block, order_idxs_to_unique.at(index), vec);
    if (iter == nullptr)
    {
        delete vec;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IteratorImpl::setLogger(logger);
    return iter;
}

ISet::IIterator *SetImpl::getBegin() const
{
    return getIterator(0);
}

ISet::IIterator *SetImpl::getEnd() const
{
    return getIterator(size - 1);
}

RC SetImpl::getNextByUniqueIndex(IVector *const &vec, size_t &index, size_t indexInc)
{
    try
    {
        index = order_idxs_to_unique.at(unique_idxs_to_order.at(index) + indexInc);
    }
    catch (std::out_of_range)
    {
        return RC::INDEX_OUT_OF_BOUND;
    }
    return getCoords(unique_idxs_to_order.at(index), vec);
}

RC SetImpl::getPrevByUniqueIndex(IVector *const &vec, size_t &index, size_t indexInc)
{
    try
    {
        index = order_idxs_to_unique.at(unique_idxs_to_order.at(index) - indexInc);
    }
    catch (std::out_of_range)
    {
        return RC::INDEX_OUT_OF_BOUND;
    }
    return getCoords(unique_idxs_to_order.at(index), vec);
}

RC SetImpl::getFirstByUniqueIndex(IVector *const &vec, size_t &index)
{
    index = order_idxs_to_unique.at(0);
    return getCoords(0, vec);
}

RC SetImpl::getLastByUniqueIndex(IVector *const &vec, size_t &index)
{
    index = order_idxs_to_unique.at(size - 1);
    return getCoords(size - 1, vec);
}
