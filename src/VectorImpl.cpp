#include <cstring>
#include <math.h>
#include <limits>
#include <functional>
#include "../myHeaders/VectorImpl.h"
#include "../myHeaders/LoggerImpl.h"






///////////////////IVector/////////////////


IVector::~IVector()= default;

IVector* IVector::createVector(size_t dim, double const* const& ptr_data)
{
    if (!ptr_data)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    const size_t _size = sizeof(VectorImpl) + dim * sizeof(double);
    uint8_t* pInstance = new (std::nothrow)uint8_t[_size];

    if (!pInstance)
    {
        VectorImpl::log(RC::ALLOCATION_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    uint8_t* pData = pInstance + sizeof(VectorImpl);

    memcpy(pData, (uint8_t*)ptr_data, dim * sizeof(double));

    return new(pInstance)VectorImpl(dim);
}
    RC IVector::copyInstance(IVector* const dest, IVector const* const& src)
    {
        if (!src || !dest)
        {
            VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::NULLPTR_ERROR;
        }

        if (abs(reinterpret_cast<uint8_t*>(dest) - reinterpret_cast<uint8_t const*>(src)) < src->sizeAllocated())
        {
            VectorImpl::log(RC::UNKNOWN, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::UNKNOWN;
        }

        memcpy((void*)dest, (void*)src, src->sizeAllocated());

        return RC::SUCCESS;
    }


    IVector* VectorImpl::clone() const
    {
        int size = sizeof(VectorImpl) + this->getDim() * sizeof(double);

        auto* pBlock = new uint8_t[size];
        auto* pVector = (uint8_t*)this;

        memcpy(pBlock, pVector, size);

        return (IVector*) pBlock;
    }

IVector* IVector::sub(IVector const* const& op1, IVector const* const& op2)
{
    if (op1 == nullptr || op2 == nullptr)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    if (op1->getDim() != op2->getDim())
    {
        VectorImpl::log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector *newVec = op1->clone();
    newVec->dec(op2);
    return newVec;
}

IVector* IVector::add(IVector const* const& op1, IVector const* const& op2)
{
    if (!op1 || !op2 || op1->getDim() != op2->getDim())
        return nullptr;

    const double _dimension = op1->getDim();
    double result;
    IVector* newVec = op1->clone();

    if (!newVec)
        return nullptr;

    const double* data1 = op1->getData();
    const double* data2 = op2->getData();
    auto* ptr = (double*)((uint8_t*)newVec + sizeof(VectorImpl));

    for (size_t i = 0; i < _dimension; i++)
    {
        result = data1[i] + data2[i];

        if (fabs(result) == std::numeric_limits<double>::infinity())
        {
            VectorImpl::log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            delete newVec;
            newVec = nullptr;
            return newVec;
        }

        ptr[i] = result;
    }

    return newVec;
}

double IVector::dot(IVector const* const& op1, IVector const* const& op2)
{

    if (!op1 || !op2 || op1->getDim() != op2->getDim())
        return std::numeric_limits<double>::quiet_NaN();


    double result = 0;
    const double dim = op1->getDim();

    double const * data1 = op1->getData();
    double const * data2 = op2->getData();

    for (size_t i = 0; i < dim; i++)
    {
        result += data1[i] * data2[i];

        if(!std::isfinite(fabs(result)))
        {
            VectorImpl::log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    return result;
}

bool IVector::equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol)
{
    if (op1 == nullptr || op2 == nullptr)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if (op1->getDim() != op2->getDim())
    {
        VectorImpl::log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if (tol < 0)
    {
        VectorImpl::log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    IVector *sub = IVector::sub(op1, op2);

    if (sub->norm(n) > tol)
    {
        delete sub;
        return false;
    }

    delete sub;
    return true;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src)
{
    if (dest == nullptr || src == nullptr)
    {
        return RC::NULLPTR_ERROR;
    }

    if (dest->sizeAllocated() != src->sizeAllocated() || dest->getDim() != src->getDim())
    {
        return RC::MISMATCHING_DIMENSIONS;
    }

    const size_t srcSizeAllocated = src->sizeAllocated();

    memmove(reinterpret_cast<uint8_t*>(dest), reinterpret_cast<const uint8_t*>(src), srcSizeAllocated);

    delete src;

    src = nullptr;

    return RC::SUCCESS;
}

RC IVector::setLogger(ILogger *const logger)
{
    return VectorImpl::setLogger(logger);
}

///////////////////VectorImpl////////////////


    VectorImpl::~VectorImpl() = default;



    size_t  VectorImpl::sizeAllocated() const
    {
        return dimension*sizeof(double) + sizeof(VectorImpl);
    }

    RC VectorImpl::setLogger(ILogger *const logger)
    {
        pLogger = logger;
        return RC::SUCCESS;
    }

    void VectorImpl::log(RC code, ILogger::Level level, const char* const& srcfile, const char* const& function, int line)
    {
        if (pLogger != nullptr)
            pLogger->log(code, level, srcfile, function, line);
    }



    RC VectorImpl::scale(double multiplier)
    {
        if (multiplier == 1.0)
            return RC::SUCCESS;

        if (!std::isfinite(fabs(multiplier)))
        {
            log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::INVALID_ARGUMENT;
        }


        double* ptr = (double*)((uint8_t*)this + sizeof(VectorImpl));

        for (size_t i = 0; i < dimension; i++)
        {
            if (!std::isfinite(fabs(ptr[i] * multiplier)));
            {
                log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                return RC::INVALID_ARGUMENT;
            }
        }


        for (size_t i = 0; i < dimension; i++)
        {
            ptr[i] *= multiplier;
        }
        return RC::SUCCESS;
    }

    RC VectorImpl::inc(IVector const* const& op)
    {
        if (!op)
            return RC::INVALID_ARGUMENT;

        if (dimension != op->getDim())
        {
            log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::MISMATCHING_DIMENSIONS;
        }

        const double* ptr2 = op->getData();
        double* ptr1 = (double*)((uint8_t*)this + sizeof(VectorImpl));

        for (size_t i = 0; i < dimension; i++)
        {
            if (!std::isfinite(fabs(ptr1[i] + ptr2[i])))
            {
                log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                return RC::INFINITY_OVERFLOW;
            }
        }

        for (size_t i = 0; i < dimension; i++)
        {
            ptr1[i] += ptr2[i];
        }

        return RC::SUCCESS;
    }

    RC VectorImpl::dec(IVector const* const& op)
    {
        if (!op)
            return RC::INVALID_ARGUMENT;

        if (dimension != op->getDim())
        {
            log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::MISMATCHING_DIMENSIONS;
        }

        const double* ptr2 = op->getData();
        double* ptr1 = (double*)((uint8_t*)this + sizeof(VectorImpl));

        for (size_t i = 0; i < dimension; i++)
        {
            if (!std::isfinite(fabs(ptr1[i] - ptr2[i])))
            {
                log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                return RC::INFINITY_OVERFLOW;
            }
        }

        for (size_t i = 0; i < dimension; i++)
        {
            ptr1[i] -= ptr2[i];
        }

        return RC::SUCCESS;
    }

    RC VectorImpl::applyFunction(const std::function<double(double)>& fun)
    {
        double* data = (double*)(reinterpret_cast<const uint8_t*>(this) + sizeof(VectorImpl));

        for (int i = 0; i < dimension; i++)
        {
            if (!std::isfinite(fabs(fun(data[i]))))
            {
                log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                return RC::INFINITY_OVERFLOW;
            }
        }

        for (int i = 0; i < dimension; i++)
        {
            data[i] = fun(data[i]);
        }

        return RC::SUCCESS;
    }



    //////////////////////GETTERS && SETTERS///////////////


    double const *VectorImpl::getData() const
    {
        return (double *)((uint8_t *)(this) + sizeof(VectorImpl));
    }

    size_t VectorImpl::getDim() const
    {
        return dimension;
    }

    RC VectorImpl::getCord(size_t index, double& val) const
    {
        if (index > this->getDim() - 1)
        {
            log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::INDEX_OUT_OF_BOUND;
        }

        if (!std::isfinite(fabs(val)))
        {
            log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::INVALID_ARGUMENT;
        }

        auto* ptr = (double*)((uint8_t*)this + sizeof(VectorImpl) + index * sizeof(double));
        val = *ptr;
        return RC::SUCCESS;
    }

double VectorImpl::norm(NORM n) const
{
    double result = 0;
    const double* data = this->getData();

    switch (n)
    {
        case NORM::FIRST:
            for (size_t i = 0; i < this->getDim(); i++)
            {
                result += fabs(data[i]);

                if (!std::isfinite(fabs(result)))
                {
                    log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                    return std::numeric_limits<double>::quiet_NaN();;
                }
            }

            break;

        case NORM::SECOND:
            for (size_t i = 0; i < this->getDim(); i++)
            {
                result += data[i] * data[i];

                if (!std::isfinite(fabs(result)))
                {
                    log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                    return std::numeric_limits<double>::quiet_NaN();;
                }
            }
            result = sqrt(result);
            break;

        case NORM::CHEBYSHEV:
            result = 0;
            for (size_t i = 0; i < this->getDim(); i++)
            {
                if (fabs(data[i]) > result)
                    result = fabs(data[i]);

                if (!std::isfinite(fabs(result)))
                {
                    log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                    return std::numeric_limits<double>::quiet_NaN();
                }
            }
            break;
        default:
        {
            log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    return result;
}

RC VectorImpl::foreach(const std::function<void(double)>& fun) const
{
    double* data = (double*)((uint8_t*)this + sizeof(VectorImpl));

    for (size_t i = 0; i < dimension; i++)
    {
        fun(data[i]);
    }

    return RC::SUCCESS;
}

RC VectorImpl::setCord(size_t index, double val)
{
    if (index > this->getDim() - 1)
    {
        log(RC::INDEX_OUT_OF_BOUND, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    if (!std::isfinite(fabs(val)))
    {
        log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    auto* ptr = (double*)((uint8_t*)this + sizeof(VectorImpl) + index * sizeof(double));
    *ptr = val;

    return RC::SUCCESS;
}

RC VectorImpl::setData(size_t dim, double const* const& ptr_data)
{
    if (this->dimension != dim)
    {
        VectorImpl::log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (ptr_data == nullptr)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    for (size_t idx = 0; idx < dim; ++idx)
        if (std::isnan(ptr_data[idx]) || std::isinf(ptr_data[idx]))
        {
            VectorImpl::log(RC::NOT_NUMBER, ILogger::Level::SEVERE, __FILE__, __func__, __LINE__);
            return RC::NOT_NUMBER;
        }

    for (size_t idx = 0; idx < dim; ++idx)
    {
        *( (double *)((uint8_t *)(this) + sizeof(VectorImpl)) + idx) = ptr_data[idx];
    }

    return RC::SUCCESS;
}





