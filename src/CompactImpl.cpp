#include <cstring>
#include "../myHeaders/CompactImpl.h"

ILogger* CompactImpl::logger = nullptr;

ILogger* CompactImpl::Iterator::LoggerIterator = nullptr;

CompactImpl::CompactImpl(size_t dim_, IVector* begin_, IVector* end_, IMultiIndex* order_) :
    dim(dim_), begin(begin_), end(end_), order(order_)
{
    pBlock = std::shared_ptr<CompactControlBlockImpl>(new(std::nothrow)CompactControlBlockImpl(this));
}

ICompact* CompactImpl::clone() const
{
    IVector* begin_ = begin->clone();
    if(begin_ == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* end_ = end->clone();
    if(end_ == nullptr)
    {
        delete begin_;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* order_ = order->clone();
    if(order_ == nullptr)
    {
        delete begin_;
        delete end_;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    CompactImpl* copy = new(std::nothrow) CompactImpl(dim, begin_, end_, order_);
    if (copy == nullptr)
    {
        delete begin_;
        delete end_;
        delete order_;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    return copy;
}

bool CompactImpl::isInside(const IVector* const& vec) const
{
    if (vec == nullptr)
    {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if (vec->getDim() != dim)
    {
        logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    double val = 0;
    double left = 0;
    double right = 0;

    for (size_t i = 0; i < dim; ++i)
    {
        left = begin->getData()[i];
        val = vec->getData()[i];
        right = vec->getData()[i];
        if (!(left <= val && val <= right))
        {
            return false;
        }
    }

    return true;
}

bool isvalid(const IMultiIndex* index)
{
    return index->getData()[0] != std::numeric_limits<size_t>::max();
}

RC CompactImpl::getVectorCopy(const IMultiIndex* index, IVector*& val) const
{
    if (index == nullptr)
    {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (index->getDim() != dim)
    {
        logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (!isvalid(index))
    {
        logger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    size_t coord1 = 0;
    size_t coord2 = 0;

    for (size_t i = 0; i < dim; ++i)
    {
        coord1 = index->getData()[i];
        coord2 = order->getData()[i];
        if (coord1 > coord2)
        {
            logger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
            return RC::INDEX_OUT_OF_BOUND;
        }
    }

    double data[dim];

    for (size_t i = 0; i < dim; ++i)
    {
        data[i] = begin->getData()[i] + (end->getData()[i] - begin->getData()[i]) * (double) (index->getData()[i] - 1) /
                  (double) (order->getData()[i] - 1);
    }

    val = IVector::createVector(dim, data);

    if (val == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }

    return RC::SUCCESS;
}

RC CompactImpl::getVectorCoords(const IMultiIndex* index, IVector* const& val) const
{
    if (index == nullptr || val == nullptr)
    {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (index->getDim() != dim || dim != val->getDim())
    {
        logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (!isvalid(index))
    {
        logger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    size_t coord1 = 0;
    size_t coord2 = 0;

    for (size_t i = 0; i < dim; ++i)
    {
        coord1 = index->getData()[i];
        coord2 = order->getData()[i];
        if (coord1 > coord2)
        {
            logger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
            return RC::INDEX_OUT_OF_BOUND;
        }
    }

    double data[dim];
    for (size_t i = 0; i < dim; ++i)
    {
        data[i] = begin->getData()[i] + (end->getData()[i] - begin->getData()[i]) * (double) (index->getData()[i] - 1) /
                  (double) (order->getData()[i] - 1);
    }

    val->setData(dim, data);
    return RC::SUCCESS;
}

RC CompactImpl::getLeftBoundary(IVector*& vec) const
{
    vec = begin->clone();
    if (vec == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }

    return RC::SUCCESS;
}

RC CompactImpl::getRightBoundary(IVector*& vec) const
{
    vec = end->clone();
    if (vec == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }

    return RC::SUCCESS;
}

size_t CompactImpl::getDim() const
{
    return dim;
}

IMultiIndex* CompactImpl::getGrid() const
{
    return order;
}

int comparator(const void* a, const void* b)
{
    return (int) (*(ssize_t*) a - *(ssize_t*) b);
}

ICompact::IIterator* CompactImpl::getIterator(const IMultiIndex* const& index, const IMultiIndex* const& bypassOrder) const
{
    if (index == nullptr || bypassOrder == nullptr)
    {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    if (index->getDim() != bypassOrder->getDim())
    {
        logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    const size_t dim_ = index->getDim();
    size_t sortOrder[dim_];
    memcpy(sortOrder, bypassOrder->getData(), sizeof(size_t) * dim_);
    qsort(sortOrder, dim_, sizeof(size_t), comparator);

    for (size_t i = 0; i < dim; ++i)
    {
        if (sortOrder[i] != i)
        {
            logger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
            return nullptr;
        }

        if (index->getData()[i] > order->getData()[i])
        {
            logger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
            return nullptr;
        }
    }

    IVector* vec = nullptr;
    RC rc = getVectorCopy(index, vec);

    if (rc != RC::SUCCESS)
    {
        logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* iMultiIndex = index->clone();
    if (iMultiIndex == nullptr)
    {
        delete vec;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* order = bypassOrder->clone();
    if (order == nullptr)
    {
        delete vec;
        delete iMultiIndex;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    ICompact::IIterator* iterator = new(std::nothrow) CompactImpl::Iterator(vec, iMultiIndex, order, pBlock);
    if (iterator == nullptr)
    {
        delete vec;
        delete iMultiIndex;
        delete order;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    return iterator;
}

ICompact::IIterator* CompactImpl::getBegin(const IMultiIndex* const& bypassOrder) const
{
    size_t* data = new(std::nothrow) size_t[getDim()];
    if (data == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    for (size_t i = 0; i < getDim(); ++i)
    {
        data[i] = 1;
    }

    IMultiIndex* index = IMultiIndex::createMultiIndex(getDim(), data);
    delete[] data;
    if (index == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IIterator* res = getIterator(index, bypassOrder);
    delete index;
    return res;
}

ICompact::IIterator* CompactImpl::getEnd(const IMultiIndex* const& bypassOrder) const
{
    size_t* data = new(std::nothrow) size_t[getDim()];
    if (data == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    for (size_t i = 0; i < getDim(); ++i)
    {
        data[i] = order->getData()[i];
    }

    IMultiIndex* index = IMultiIndex::createMultiIndex(getDim(), data);
    delete[] data;
    if (index == nullptr)
    {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IIterator* res = getIterator(index, bypassOrder);
    delete index;
    return res;
}

CompactImpl::~CompactImpl()
{
    pBlock->free_compact();
    delete begin;
    delete end;
    delete order;
}

CompactImpl::Iterator::Iterator(IVector* iVector_, IMultiIndex* index_, IMultiIndex* bypassOrder_,
                                    const std::shared_ptr<CompactControlBlockImpl>& pBlock_)
{
    iVector = iVector_;
    index = index_;
    bypassOrder = bypassOrder_;
    pBlock = pBlock_;
}

ICompact::IIterator* CompactImpl::Iterator::getNext()
{
    IIterator* iIterator = clone();
    if (iIterator == nullptr)
    {
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    iIterator->next();
    return iIterator;
}

ICompact::IIterator* CompactImpl::Iterator::clone() const
{
    IVector* vec = iVector->clone();
    if (vec == nullptr)
    {
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* multiIndex = index->clone();
    if (multiIndex == nullptr)
    {
        delete vec;
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* order = bypassOrder->clone();
    if (order == nullptr)
    {
        delete vec;
        delete multiIndex;
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IIterator* iterator = new(std::nothrow) CompactImpl::Iterator(vec, multiIndex, order, pBlock);
    if (iterator == nullptr)
    {
        delete vec;
        delete multiIndex;
        delete order;
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    return iterator;
}

RC CompactImpl::Iterator::next()
{
    RC rc = pBlock->get(index, bypassOrder);
    if (rc != RC::SUCCESS)
    {
        return rc;
    }

    return pBlock->get(index, iVector);
}

bool CompactImpl::Iterator::isValid() const
{
    return index->getData()[0] != std::numeric_limits<size_t>::max();
}

RC CompactImpl::Iterator::getVectorCopy(IVector*& val) const
{
    IVector* vec = iVector->clone();
    if (vec == nullptr)
    {
        LoggerIterator->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        val = nullptr;
        return RC::ALLOCATION_ERROR;
    }

    val = vec;
    return RC::SUCCESS;
}

RC CompactImpl::Iterator::getVectorCoords(IVector* const& val) const
{
    if (val == nullptr)
    {
        LoggerIterator->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (val->getDim() != iVector->getDim())
    {
        LoggerIterator->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    return val->setData(iVector->getDim(), iVector->getData());
}

CompactImpl::Iterator::~Iterator()
{
    delete iVector;
    delete index;
    delete bypassOrder;
}

ICompact::IIterator::~IIterator() = default;
