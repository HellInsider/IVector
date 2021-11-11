#include <cmath>
#include <cstring>
#include "../include/ICompact.h"
#include "../myHeaders/CompactImpl.h"

size_t const projection = 2;

static bool checkIntersection(const double arr1[], const double arr2[], double& resLeft, double& resRight)
{
    if (arr1[1] < arr2[0] || arr2[1] < arr1[0])
    {
        resLeft = 0;
        resRight = 0;
        return false;
    }

    if (arr2[0] < arr1[0] && arr1[1] < arr2[1])
    {
        resLeft = arr1[0];
        resRight = arr1[1];
        return true;
    }

    if (arr1[0] < arr2[0] && arr2[1] < arr1[1])
    {
        resLeft = arr2[0];
        resRight = arr2[1];
        return true;
    }

    if (arr1[0] < arr2[0] && arr2[0] < arr1[1] && arr1[1] < arr2[1])
    {
        resLeft = arr2[0];
        resRight = arr1[1];
        return true;
    }

    if (arr2[0] < arr1[0] && arr1[0] < arr2[1] && arr2[1] < arr1[1])
    {
        resLeft = arr1[0];
        resRight = arr2[1];
        return true;
    }
    else return false;
}

static void checkUnion(const double arr[], double& resLeft, double& resRight)
{
    resLeft = arr[0];
    resRight = arr[0];
    for (size_t i = 1; i < projection * 2; ++i)
    {
        if (arr[i] < resLeft)
        {
            resLeft = arr[i];
        }

        if (arr[i] > resRight)
        {
            resRight = arr[i];
        }
    }
}

ICompact* ICompact::createCompact(const IVector* vec1, const IVector* vec2, const IMultiIndex* nodeQuantities)
{
    if (vec1 == nullptr || vec2 == nullptr || nodeQuantities == nullptr)
    {
        CompactImpl::logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    if (vec1->getDim() != vec2->getDim() || vec2->getDim() != nodeQuantities->getDim())
    {
        CompactImpl::logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    const size_t dim_ = vec1->getDim();
    for (size_t i = 0; i < dim_; ++i)
    {
        if (nodeQuantities->getData()[i] < 1)
        {
            CompactImpl::logger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
            return nullptr;
        }
    }

    double* beginData = new(std::nothrow) double[dim_];
    if (beginData == nullptr)
    {
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    memcpy(beginData, vec1->getData(), sizeof(double) * dim_);

    double* endData = new(std::nothrow) double[dim_];
    if (endData == nullptr)
    {
        delete[] beginData;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    memcpy(endData, vec2->getData(), sizeof(double) * dim_);

    for (size_t i = 0; i < dim_; ++i)
    {
        if (beginData[i] > endData[i])
        {
            std::swap(beginData[i], endData[i]);
        }
    }

    IVector* begin = IVector::createVector(vec1->getDim(), beginData);
    if (begin == nullptr)
    {
        delete[] beginData;
        delete[] endData;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* end = IVector::createVector(vec2->getDim(), endData);
    if (end == nullptr)
    {
        delete[] beginData;
        delete[] endData;
        delete begin;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* order = nodeQuantities->clone();
    if (order == nullptr)
    {
        delete[] beginData;
        delete[] endData;
        delete begin;
        delete end;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    ICompact* compact = new(std::nothrow) CompactImpl(vec1->getDim(), begin, end, order);
    if (compact == nullptr)
    {
        delete[] beginData;
        delete[] endData;
        delete begin;
        delete end;
        delete order;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    delete[] beginData;
    delete[] endData;
    return compact;
}

RC ICompact::setLogger(ILogger* const logger)
{
    if (logger == nullptr)
    {
        CompactImpl::logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    CompactImpl::logger = logger;
    return RC::SUCCESS;
}

ILogger* ICompact::getLogger()
{
    return CompactImpl::logger;
}

ICompact*
ICompact::createIntersection(const ICompact* op1, const ICompact* op2, const IMultiIndex* const grid, double tol)
{
    if (op1 == nullptr || op2 == nullptr || grid == nullptr)
    {
        CompactImpl::logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    if (std::isnan(tol) || tol == INFINITY || -tol == INFINITY)
    {
        CompactImpl::logger->warning(RC::INVALID_ARGUMENT, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    if (op1->getDim() != op2->getDim() || op2->getDim() != grid->getDim())
    {
        CompactImpl::logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* op1Left;
    IVector* op1Right;
    IVector* op2Left;
    IVector* op2Right;
    RC rc = op1->getLeftBoundary(op1Left);
    if (rc != RC::SUCCESS)
    {
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    rc = op1->getRightBoundary(op1Right);
    if (rc != RC::SUCCESS)
    {
        delete op1Left;
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    rc = op2->getLeftBoundary(op2Left);
    if (rc != RC::SUCCESS)
    {
        delete op1Left;
        delete op1Right;
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    rc = op2->getRightBoundary(op2Right);
    if (rc != RC::SUCCESS)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IMultiIndex* interGrid = grid->clone();
    if (interGrid == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    const size_t dim_ = op1->getDim();
    double arr1[projection] = {0, 0};
    double arr2[projection] = {0, 0};
    double resLeft[dim_];
    double resRight[dim_];
    double temp[projection] = {0, 0};

    for (size_t i = 0; i < dim_; ++i)
    {
        arr1[0] = op1Left->getData()[i];
        arr1[1] = op1Right->getData()[i];
        arr2[0] = op2Left->getData()[i];
        arr2[1] = op2Right->getData()[i];
        if (!checkIntersection(arr1, arr2, resLeft[i], resRight[i]))
        {
            delete op1Left;
            delete op1Right;
            delete op2Left;
            delete op2Right;
            delete interGrid;
            return nullptr;
        }

        // Проверить вырожденность
        temp[0] = resLeft[i];
        temp[1] = resRight[i];
        IVector* temp_intersection = IVector::createVector(projection, temp);
        if (temp_intersection == nullptr)
        {
            delete op1Left;
            delete op1Right;
            delete op2Left;
            delete op2Right;
            delete interGrid;
            CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
            return nullptr;
        }
        if (temp_intersection->norm(IVector::NORM::FIRST) < tol)
        {
            interGrid->setAxisIndex(i, 1);
        }
        delete temp_intersection;
    }

    IVector* start = IVector::createVector(dim_, resLeft);
    if (start == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        delete interGrid;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* end = IVector::createVector(dim_, resRight);
    if (end == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        delete interGrid;
        delete start;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    ICompact* intersection = ICompact::createCompact(start, end, interGrid);
    if (intersection == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        delete interGrid;
        delete start;
        delete end;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    delete op1Left;
    delete op1Right;
    delete op2Left;
    delete op2Right;
    delete interGrid;
    delete start;
    delete end;

    return intersection;
}

ICompact* ICompact::createCompactSpan(const ICompact* op1, const ICompact* op2, const IMultiIndex* const grid)
{
    if (op1 == nullptr || op2 == nullptr || grid == nullptr)
    {
        CompactImpl::logger->warning(RC::NULLPTR_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim())
    {
        CompactImpl::logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* op1Left;
    IVector* op1Right;
    IVector* op2Left;
    IVector* op2Right;
    RC rc = op1->getLeftBoundary(op1Left);
    if (rc != RC::SUCCESS)
    {
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    rc = op1->getRightBoundary(op1Right);
    if (rc != RC::SUCCESS)
    {
        delete op1Left;
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    rc = op2->getLeftBoundary(op2Left);
    if (rc != RC::SUCCESS)
    {
        delete op1Left;
        delete op1Right;
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    rc = op2->getRightBoundary(op2Right);
    if (rc != RC::SUCCESS)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        CompactImpl::logger->severe(rc, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    const size_t dim_ = op1->getDim();
    double arr[projection * 2] = {0, 0, 0, 0};
    double resLeft[dim_];
    double resRight[dim_];
    for (size_t i = 0; i < dim_; ++i)
    {
        arr[0] = op1Left->getData()[i];
        arr[1] = op1Right->getData()[i];
        arr[2] = op2Left->getData()[i];
        arr[3] = op2Right->getData()[i];
        checkUnion(arr, resLeft[i], resRight[i]);
    }

    IVector* start = IVector::createVector(dim_, resLeft);
    if (start == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    IVector* end = IVector::createVector(dim_, resRight);
    if (end == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        delete start;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    ICompact* Union = ICompact::createCompact(start, end, grid);
    if (Union == nullptr)
    {
        delete op1Left;
        delete op1Right;
        delete op2Left;
        delete op2Right;
        delete start;
        delete end;
        CompactImpl::logger->severe(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    delete op1Left;
    delete op1Right;
    delete op2Left;
    delete op2Right;
    delete start;
    delete end;
    return Union;
}

RC ICompact::IIterator::setLogger(ILogger* const pLogger)
{
    if (pLogger == nullptr)
    {
        return RC::NULLPTR_ERROR;
    }
    CompactImpl::Iterator::LoggerIterator = pLogger;
    return RC::SUCCESS;
}

ILogger* ICompact::IIterator::getLogger()
{
    return CompactImpl::Iterator::LoggerIterator;
}

ICompact::~ICompact() = default;
