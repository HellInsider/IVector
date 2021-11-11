#include "../myHeaders/MultiIndexImpl.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <new>

ILogger *MultiIndexImpl::logger = nullptr;

RC MultiIndexImpl::setLogger(ILogger *const pLogger)
{
    if (pLogger == nullptr)
    {
        return RC::NULLPTR_ERROR;
    }

    logger = pLogger;
    return RC::SUCCESS;
}
ILogger *MultiIndexImpl::getLogger()
{
    return logger;
}

MultiIndexImpl::MultiIndexImpl(size_t dim) : dim(dim) {}

IMultiIndex *MultiIndexImpl::createMultiIndex(size_t dim, const size_t *indices)
{
    if (dim == 0)
    {
        logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    if (indices == nullptr)
    {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    uint8_t *ptr = new (std::nothrow) uint8_t[sizeof(MultiIndexImpl) + dim * sizeof(size_t)];
    if (ptr == nullptr)
    {
        logger->warning(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    IMultiIndex *multi_ind = new (ptr) MultiIndexImpl(dim);
    std::memcpy((uint8_t *)(ptr) + sizeof(MultiIndexImpl), indices, dim * sizeof(size_t));

    return multi_ind;
}

IMultiIndex *MultiIndexImpl::clone() const
{
    return createMultiIndex(dim, PTR_DATA);
}

size_t const *MultiIndexImpl::getData() const
{
    return PTR_DATA;
}

size_t MultiIndexImpl::getDim() const
{
    return dim;
}

RC MultiIndexImpl::setData(size_t dim, size_t const *const &ptr_data)
{
    if (this->dim != dim)
    {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (ptr_data == nullptr)
    {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    for (size_t idx = 0; idx < dim; ++idx)
    {
        if (std::isnan(ptr_data[idx]) || std::isinf(ptr_data[idx]))
        {
            logger->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
            return RC::NOT_NUMBER;
        }
    }


    for (size_t idx = 0; idx < dim; ++idx)
    {
        *(PTR_DATA + idx) = ptr_data[idx];
    }

    return RC::SUCCESS;
}

RC MultiIndexImpl::getAxisIndex(size_t index, size_t &val) const
{
    if (index >= dim)
    {
        logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    val = *(PTR_DATA + index);
    return RC::SUCCESS;
}

RC MultiIndexImpl::setAxisIndex(size_t index, size_t val)
{
    if (index >= dim)
    {
        logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    *(PTR_DATA + index) = val;
    return RC::SUCCESS;
}

RC MultiIndexImpl::incAxisIndex(size_t index, size_t val)
{
    if (index >= dim)
    {
        logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    *(PTR_DATA + index) += val;
    return RC::SUCCESS;
}

void MultiIndexImpl::operator delete(void *ptr, size_t size)
{
    if (ptr)
        delete[] reinterpret_cast<uint8_t *>(ptr);
}

IMultiIndex *IMultiIndex::createMultiIndex(size_t dim, const size_t *indices)
{
    return MultiIndexImpl::createMultiIndex(dim, indices);
}

RC IMultiIndex::setLogger(ILogger *const pLogger)
{
    return MultiIndexImpl::setLogger(pLogger);
}

ILogger *IMultiIndex::getLogger()
{
    return MultiIndexImpl::getLogger();
}

IMultiIndex::~IMultiIndex() = default;
