#include "../myHeaders/SetImpl.h"
#include "../include/ISetControlBlock.h"
#include <cmath>
#include <map>
#include <vector>
#include <iostream>

ILogger *SetImpl::logger = nullptr;
ILogger *SetImpl::IteratorImpl::logger = nullptr;

RC ISet::setLogger(ILogger *const logger)
{
    return SetImpl::setLogger(logger);
}

ILogger *ISet::getLogger()
{
    return SetImpl::getLogger();
}

ISet *ISet::createSet()
{
    return SetImpl::createSet();
}

RC SetImpl::setLogger(ILogger *const pLogger)
{
    if (pLogger == nullptr)
        return RC::NULLPTR_ERROR;

    logger = pLogger;
    return RC::SUCCESS;
}
ILogger *SetImpl::getLogger()
{
    return logger;
}

ISet *SetImpl::createSet()
{
    return new (std::nothrow) SetImpl;
}

ISet *SetImpl::clone() const
{
    return new (std::nothrow) SetImpl(data, size, dim, unique_idxs_to_order, order_idxs_to_unique, last_vec_idx);
}

size_t SetImpl::getDim() const
{
    return dim;
}

size_t SetImpl::getSize() const
{
    return size;
}

RC SetImpl::getCopy(size_t index, IVector *&val) const
{
    if (size == 0)
    {
        logger->warning(RC::SOURCE_SET_EMPTY, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_EMPTY;
    }

    if (index > size - 1)
    {
        logger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    double *buffer = new double[dim];
    for (size_t idx = 0; idx < dim; ++idx)
    {
        buffer[idx] = data[index * dim + idx];
    }

    val = IVector::createVector(dim, buffer);
    delete[] buffer;

    return RC::SUCCESS;
}

RC SetImpl::findFirst(IVector const *const &pat, IVector::NORM n, double tol) const
{
    if (pat == nullptr)
    {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (pat->getDim() != dim)
    {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (n == IVector::NORM::AMOUNT || tol < 0)
    {
        logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    if (std::isnan(tol))
    {
        logger->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }

    if (std::isinf(tol))
    {
        logger->severe(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }

    for (size_t vec_idx = 0; vec_idx < size; ++vec_idx)
    {
        bool found = false;
        double *cur_data = new double[dim];

        for (size_t idx = 0; idx < dim; ++idx)
        {
            cur_data[idx] = data[vec_idx * dim + idx];
        }

        IVector *cur_vec = IVector::createVector(dim, cur_data);

        if (IVector::equals(pat, cur_vec, n, tol))
        {
            found = true;
        }


        delete[] cur_data;
        delete cur_vec;
        if (found)
        {
            return RC::SUCCESS;
        }

    }
    return RC::VECTOR_NOT_FOUND;
}

RC SetImpl::findFirstAndCopy(IVector const *const &pat, IVector::NORM n, double tol, IVector *&val) const
{
    if (pat == nullptr)
    {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (pat->getDim() != dim)
    {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (n == IVector::NORM::AMOUNT || tol < 0)
    {
        logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    if (std::isnan(tol))
    {
        logger->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }

    if (std::isinf(tol))
    {
        logger->severe(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }

    for (size_t vec_idx = 0; vec_idx < size; ++vec_idx)
    {
        bool found = false;
        double *cur_data = new double[dim];

        for (size_t idx = 0; idx < dim; ++idx)
        {
            cur_data[idx] = data[vec_idx * dim + idx];
        }

        IVector *cur_vec = IVector::createVector(dim, cur_data);

        if (IVector::equals(pat, cur_vec, n, tol))
        {
            found = true;

        }

        delete[] cur_data;
        if (found)
        {
            val = cur_vec;
            return RC::SUCCESS;
        }

        delete cur_vec;
    }
    val = nullptr;
    return RC::VECTOR_NOT_FOUND;
}

RC SetImpl::getCoords(size_t index, IVector *const &val) const
{
    if (size == 0)
    {
        logger->warning(RC::SOURCE_SET_EMPTY, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_EMPTY;
    }

    if (index > size - 1)
    {
        logger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    if (val == nullptr)
    {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    double *buffer = new double[dim];
    for (size_t idx = 0; idx < dim; ++idx)
    {
        buffer[idx] = data[index * dim + idx];
    }

    RC err = val->setData(dim, buffer);
    delete[] buffer;

    return err;
}

RC SetImpl::findFirstAndCopyCoords(IVector const *const &pat, IVector::NORM n, double tol, IVector *const &val) const
{
    if (pat == nullptr)
    {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (pat->getDim() != dim)
    {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (n == IVector::NORM::AMOUNT || tol < 0)
    {
        logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    if (std::isnan(tol))
    {
        logger->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }

    if (std::isinf(tol))
    {
        logger->severe(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }

    for (size_t vec_idx = 0; vec_idx < size; ++vec_idx)
    {
        bool found = false;
        double *cur_data = new double[dim];

        for (size_t idx = 0; idx < dim; ++idx)
        {
            cur_data[idx] = data[vec_idx * dim + idx];
        }
        IVector *cur_vec = IVector::createVector(dim, cur_data);

        if (IVector::equals(pat, cur_vec, n, tol))
        {
            found = true;
        }

        delete cur_vec;
        if (found)
        {
            RC err = val->setData(dim, cur_data);
            delete[] cur_data;
            return err;
        }

        delete[] cur_data;
    }
    return RC::VECTOR_NOT_FOUND;
}

RC SetImpl::insert(IVector const *const &val, IVector::NORM n, double tol)
{
    if (dim == 0)
    {
        dim = val->getDim();
    }
    else if (dim != val->getDim())
    {
        logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (size == 0)
    {
        last_vec_idx = 0;
    }

    const double *vec_data = val->getData();
    for (size_t vec_idx = 0; vec_idx < size; ++vec_idx)
    {
        double *sub = new double[dim];
        for (size_t idx = 0; idx < dim; ++idx)
        {
            sub[idx] = fabs(vec_data[idx] - data[vec_idx * dim + idx]);
        }

        IVector *tmp = IVector::createVector(dim, sub);
        if (tmp->norm(n) <= tol)
        {
            delete[] sub;

            delete tmp;
            logger->warning(RC::VECTOR_ALREADY_EXIST, __FILE__, __func__, __LINE__);
            return RC::VECTOR_ALREADY_EXIST;
        }
        delete[] sub;
        delete tmp;
    }

    while (capacity < size * dim + val->getDim())
    {
        capacity *= 2;
        double *tmp = new double[capacity];
        for (size_t idx = 0; idx < size * dim; ++idx)
        {
            tmp[idx] = data[idx];
        }

        delete[] data;
        data = tmp;
    }

    for (size_t idx = 0; idx < dim; ++idx)
    {
        data[size * dim + idx] = vec_data[idx];
    }

    unique_idxs_to_order.insert(std::pair<size_t, size_t>(last_vec_idx, size));
    order_idxs_to_unique.insert(std::pair<size_t, size_t>(size, last_vec_idx));

    ++size;
    ++last_vec_idx;

    return RC::SUCCESS;
}

RC SetImpl::remove(size_t index)
{
    if (size == 0)
    {
        logger->warning(RC::SOURCE_SET_EMPTY, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_EMPTY;
    }

    if (index > size - 1)
    {
        logger->warning(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    double *new_data = new double[capacity];
    std::map<size_t, size_t> new_unique_idxs_to_order;
    std::map<size_t, size_t> new_order_idxs_to_unique;

    for (size_t vec_idx = 0, new_idx = 0, new_vec_idx = 0; vec_idx < size; ++vec_idx)
    {
        if (vec_idx != index)
        {
            new_unique_idxs_to_order.insert(std::pair<size_t, size_t>(order_idxs_to_unique.at(vec_idx), new_vec_idx));
            new_order_idxs_to_unique.insert(std::pair<size_t, size_t>(new_vec_idx, unique_idxs_to_order.at(vec_idx)));
            ++new_vec_idx;

            for (size_t idx = 0; idx < dim; ++idx, ++new_idx)
            {
                new_data[new_idx] = data[vec_idx * size + idx];
            }
        }
    }
    delete[] data;
    data = new_data;

    unique_idxs_to_order = new_unique_idxs_to_order;
    order_idxs_to_unique = new_order_idxs_to_unique;
    --size;

    return RC::SUCCESS;
}

RC SetImpl::remove(IVector const *const &pat, IVector::NORM n, double tol)
{
    if (size == 0)
    {
        logger->warning(RC::SOURCE_SET_EMPTY, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_EMPTY;
    }

    if (pat == nullptr)
    {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (pat->getDim() != dim)
    {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (n == IVector::NORM::AMOUNT || tol < 0)
    {
        logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    if (std::isnan(tol))
    {
        logger->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }

    if (std::isinf(tol))
    {
        logger->severe(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }

    double *new_data = new double[capacity];
    std::vector<size_t> removed_idxs;
    std::map<size_t, size_t> new_unique_idxs_to_order;
    std::map<size_t, size_t> new_order_idxs_to_unique;

    for (size_t vec_idx = 0, new_idx = 0, new_vec_idx = 0; vec_idx < size; ++vec_idx)
    {
        double *cur_data = new double[dim];

        for (size_t idx = 0; idx < dim; ++idx)
        {
            cur_data[idx] = data[vec_idx * dim + idx];
        }

        IVector *cur_vec = IVector::createVector(dim, cur_data);

        if (!IVector::equals(pat, cur_vec, n, tol))
        {
            new_unique_idxs_to_order.insert(std::pair<size_t, size_t>(order_idxs_to_unique.at(vec_idx), new_vec_idx));
            new_order_idxs_to_unique.insert(std::pair<size_t, size_t>(new_vec_idx, unique_idxs_to_order.at(vec_idx)));
            ++new_vec_idx;

            for (size_t idx = 0; idx < dim; ++idx, ++new_idx)
            {
                new_data[new_idx] = data[vec_idx * size + idx];
            }

        }

        delete[] cur_data;
        delete cur_vec;
    }
    delete[] data;
    data = new_data;

    unique_idxs_to_order = new_unique_idxs_to_order;
    order_idxs_to_unique = new_order_idxs_to_unique;
    --size;

    return RC::SUCCESS;
}

SetImpl::~SetImpl()
{
    delete[] data;
    delete control_block;
}

SetImpl::SetImpl()
{
    control_block = SetImplControlBlock::createControlBlock(this);

    capacity = 100;
    data = new double[capacity];

    size = 0;
    dim = 0;
    last_vec_idx = 0;
}

SetImpl::SetImpl(double const *const &other_data, size_t other_size, size_t other_dim,
                 const std::map<size_t, size_t> &other_unique_map, const std::map<size_t, size_t> &other_order_map, size_t other_last_idx)
{
    control_block = SetImplControlBlock::createControlBlock(this);

    capacity = other_size * other_dim;
    data = new double[capacity];
    for (size_t idx = 0; idx < other_size * other_dim; ++idx)
    {
        data[idx] = other_data[idx];
    }

    size = other_size;
    dim = other_dim;
    last_vec_idx = other_last_idx;

    unique_idxs_to_order = other_unique_map;
    order_idxs_to_unique = other_order_map;
}


ISet *ISet::makeIntersection(ISet const *const &op1, ISet const *const &op2, IVector::NORM n, double tol)
{
    if (op1->getDim() != op2->getDim())
    {
        getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    size_t dim = op1->getDim();
    ISet *new_set = ISet::createSet();

    IIterator *set1_iter = op1->getBegin();
    for (; set1_iter->isValid(); set1_iter->next())
    {
        double *empty_data = new double[dim];
        IVector *tmp_vec = IVector::createVector(dim, empty_data);
        delete[] empty_data;

        RC err = set1_iter->getVectorCoords(tmp_vec);
        if (err != RC::SUCCESS)
        {
            delete tmp_vec;
            delete new_set;
            delete set1_iter;
            getLogger()->severe(err, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        err = op2->findFirst(tmp_vec, n, tol);
        if (err == RC::VECTOR_NOT_FOUND)
        {
            delete tmp_vec;
            continue;
        }
        else if (err != RC::SUCCESS)
        {
            delete tmp_vec;
            delete new_set;
            delete set1_iter;
            getLogger()->severe(err, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        new_set->insert(tmp_vec, n, tol);

        delete tmp_vec;
    }
    delete set1_iter;

    IIterator *set2_iter = op2->getBegin();
    for (; set2_iter->isValid(); set2_iter->next())
    {
        double *empty_data = new double[dim];
        IVector *tmp_vec = IVector::createVector(dim, empty_data);
        delete[] empty_data;

        RC err = set2_iter->getVectorCoords(tmp_vec);
        if (err != RC::SUCCESS)
        {
            delete tmp_vec;
            delete new_set;
            delete set2_iter;
            getLogger()->severe(err, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        err = op1->findFirst(tmp_vec, n, tol);

        if (err == RC::VECTOR_NOT_FOUND)
        {
            delete tmp_vec;
            continue;
        }
        else if (err != RC::SUCCESS)
        {
            delete tmp_vec;
            delete new_set;
            delete set2_iter;
            getLogger()->severe(err, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        new_set->insert(tmp_vec, n, tol);

        delete tmp_vec;
    }
    delete set2_iter;

    return new_set;
}

ISet *ISet::makeUnion(ISet const *const &op1, ISet const *const &op2, IVector::NORM n, double tol)
{
    if (op1->getDim() != op2->getDim())
    {
        getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    size_t dim = op1->getDim();
    ISet *new_set = op1->clone();

    IIterator *set2_iter = op2->getBegin();
    for (; set2_iter->isValid(); set2_iter->next())
    {
        double *empty_data = new double[dim];
        IVector *tmp_vec = IVector::createVector(dim, empty_data);
        delete[] empty_data;

        RC err = set2_iter->getVectorCoords(tmp_vec);
        if (err != RC::SUCCESS)
        {
            delete tmp_vec;
            delete new_set;
            delete set2_iter;
            getLogger()->severe(err, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        err = new_set->findFirst(tmp_vec, n, tol);
        if (err == RC::VECTOR_NOT_FOUND)
        {
            new_set->insert(tmp_vec, n, tol);
        }
        else if (err != RC::SUCCESS)
        {
            delete tmp_vec;
            delete new_set;
            delete set2_iter;
            getLogger()->severe(err, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        delete tmp_vec;
    }
    delete set2_iter;

    return new_set;
}

ISet *ISet::sub(ISet const *const &op1, ISet const *const &op2, IVector::NORM n, double tol)
{
    if (op1->getDim() != op2->getDim())
    {
        getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    size_t dim = op1->getDim();
    ISet *new_set = ISet::createSet();

    IIterator *set1_iter = op1->getBegin();
    for (; set1_iter->isValid(); set1_iter->next())
    {
        double *empty_data = new double[dim];
        IVector *vec1 = IVector::createVector(dim, empty_data);
        delete[] empty_data;

        RC err = set1_iter->getVectorCoords(vec1);
        if (err != RC::SUCCESS)
        {
            delete vec1;
            delete set1_iter;
            getLogger()->warning(err, __FILE__, __func__, __LINE__);
            return new_set;
        }

        err = op2->findFirst(vec1, n, tol);

        if (err == RC::VECTOR_NOT_FOUND)
        {
            new_set->insert(vec1, n, tol);
        }
        else if (err != RC::SUCCESS)
        {
            getLogger()->warning(err, __FILE__, __func__, __LINE__);
            delete vec1;
            delete set1_iter;
            return new_set;
        }

        delete vec1;
    }
    delete set1_iter;

    return new_set;
}

ISet *ISet::symSub(ISet const *const &op1, ISet const *const &op2, IVector::NORM n, double tol)
{
    ISet *un = ISet::makeUnion(op1, op2, n, tol);
    ISet *inter = ISet::makeIntersection(op1, op2, n, tol);
    ISet *sub = ISet::sub(un, inter, n, tol);

    delete un;
    delete inter;
    return sub;
}

bool ISet::equals(ISet const *const &op1, ISet const *const &op2, IVector::NORM n, double tol)
{
    if (op1->getSize() != op2->getSize())
    {
        getLogger()->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return false;
    }

    return subSet(op1, op2, n, tol);
}

bool ISet::subSet(ISet const *const &op1, ISet const *const &op2, IVector::NORM n, double tol)
{
    if (op1->getDim() != op2->getDim())
    {
        getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return false;
    }

    size_t dim = op1->getDim();

    IIterator *vec1_iter = op1->getBegin();
    for (; vec1_iter != nullptr && vec1_iter->isValid(); vec1_iter->next())
    {
        double *empty_data = new double[dim];
        IVector *vec1 = IVector::createVector(dim, empty_data);
        delete[] empty_data;

        RC err = vec1_iter->getVectorCoords(vec1);

        if (err != RC::SUCCESS)
        {
            delete vec1;
            delete vec1_iter;
            getLogger()->warning(err, __FILE__, __func__, __LINE__);
            return false;
        }

        err = op2->findFirst(vec1, n, tol);

        if (err == RC::VECTOR_NOT_FOUND)
        {
            delete vec1;
            delete vec1_iter;
            return false;
        }
        else if (err != RC::SUCCESS)
        {
            delete vec1;
            delete vec1_iter;
            getLogger()->warning(err, __FILE__, __func__, __LINE__);
            return false;
        }

        delete vec1;
    }
    delete vec1_iter;

    return true;
}

RC ISet::IIterator::setLogger(ILogger *const pLogger)
{
    return SetImpl::IteratorImpl::setLogger(pLogger);
}

ILogger *ISet::IIterator::getLogger()
{
    return SetImpl::IteratorImpl::getLogger();
}

ISet::~ISet() = default;
