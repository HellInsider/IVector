#pragma once
#include "../include/ISet.h"
#include "../myHeaders/SetControlBlockImpl.h"
#include <map>

class SetImpl : public ISet
{
public:
    static RC setLogger(ILogger *const pLogger);
    static ILogger *getLogger();

    static ISet *createSet();
    ISet *clone() const override;

    size_t getDim() const override;
    size_t getSize() const override;

    RC getCopy(size_t index, IVector *&val) const override;
    RC findFirst(IVector const *const &pat, IVector::NORM n, double tol) const override;
    RC findFirstAndCopy(IVector const *const &pat, IVector::NORM n, double tol, IVector *&val) const override;

    RC getCoords(size_t index, IVector *const &val) const override;
    RC findFirstAndCopyCoords(IVector const *const &pat, IVector::NORM n, double tol,
                              IVector *const &val) const override;

    RC insert(IVector const *const &val, IVector::NORM n, double tol) override;

    RC remove(size_t index) override;
    RC remove(IVector const *const &pat, IVector::NORM n, double tol) override;

    class IteratorImpl : public IIterator
    {
    public:
        IteratorImpl(SetImplControlBlock *const &controlBlock, size_t index, IVector *vector);

        IIterator *getNext(size_t indexInc = 1) const override;
        IIterator *getPrevious(size_t indexInc = 1) const override;
        IIterator *clone() const override;

        static RC setLogger(ILogger *const pLogger);

        RC next(size_t indexInc = 1) override;
        RC previous(size_t indexInc = 1) override;

        bool isValid() const override;

        RC makeBegin() override;
        RC makeEnd() override;

        RC getVectorCopy(IVector *&val) const override;
        RC getVectorCoords(IVector *const &val) const override;

        ~IteratorImpl();

    protected:
        IteratorImpl();

    private:
        size_t cur_unique_idx;
        IVector *cur_vector;
        SetImplControlBlock *control_block;
        static ILogger *logger;
        bool valid;
    };

    IIterator *getIterator(size_t index) const override;
    IIterator *getBegin() const override;
    IIterator *getEnd() const override;

    ~SetImpl();

    RC getNextByUniqueIndex(IVector *const &vec, size_t &index, size_t indexInc);
    RC getPrevByUniqueIndex(IVector *const &vec, size_t &index, size_t indexInc);
    RC getFirstByUniqueIndex(IVector *const &vec, size_t &index);
    RC getLastByUniqueIndex(IVector *const &vec, size_t &index);

private:
    static ILogger *logger;
    double *data;
    SetImplControlBlock *control_block;
    std::map<size_t, size_t> unique_idxs_to_order;
    std::map<size_t, size_t> order_idxs_to_unique;
    size_t last_vec_idx;
    size_t capacity; // amount of allocated double values
    size_t size;     // amount of vectors in set
    size_t dim;      // size of a single vector

protected:
    SetImpl();
    SetImpl(double const *const &other_data, size_t other_size, size_t other_dim,
            const std::map<size_t, size_t> &other_unique_map, const std::map<size_t, size_t> &other_order_map,
            size_t other_last_idx);
};
