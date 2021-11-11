#include "../myHeaders/CompactControlBlockImpl.h"

ICompactControlBlock::~ICompactControlBlock() = default;

CompactControlBlockImpl::CompactControlBlockImpl(CompactImpl* _compact)
{
    compact = _compact;
}

RC CompactControlBlockImpl::get(IMultiIndex* const& currentIndex, const IMultiIndex* const& bypassOrder) const
{
    if (currentIndex == nullptr || bypassOrder == nullptr)
    {
        CompactImpl::logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (currentIndex->getDim() != bypassOrder->getDim())
    {
        CompactImpl::logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    const size_t* order = bypassOrder->getData();
    for (size_t i = 0; i < bypassOrder->getDim(); ++i)
    {
        if (currentIndex->getData()[order[i]] == compact->getGrid()->getData()[order[i]])
        {
            currentIndex->setAxisIndex(order[i], 1);
        }
        else if(currentIndex->getData()[order[i]] < compact->getGrid()->getData()[order[i]])
        {
            currentIndex->setAxisIndex(order[i], currentIndex->getData()[order[i]] + 1);
            return RC::SUCCESS;
        }
    }
    currentIndex->setAxisIndex(0, std::numeric_limits<size_t>::max());
    return RC::INDEX_OUT_OF_BOUND;
}

RC CompactControlBlockImpl::get(const IMultiIndex* const& currentIndex, IVector* const& val) const
{
    if (currentIndex == nullptr || val == nullptr)
    {
        CompactImpl::logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (currentIndex->getDim() != compact->getGrid()->getDim())
    {
        CompactImpl::logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    for (size_t i = 0; i < compact->getDim(); ++i)
    {
        if (currentIndex->getData()[i] > compact->getGrid()->getData()[i])
        {
            CompactImpl::logger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);
            return RC::INDEX_OUT_OF_BOUND;
        }
    }

    IVector* vec = nullptr;
    RC rc = compact->getVectorCopy(currentIndex, vec);

    if(rc != RC::SUCCESS)
    {
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return rc;
    }

    val->setData(currentIndex->getDim(),vec->getData());
    delete vec;
    return RC::SUCCESS;
}

void CompactControlBlockImpl::free_compact()
{
    compact = nullptr;
}

CompactControlBlockImpl::~CompactControlBlockImpl() = default;
