#pragma once
#include <memory>
#include <iostream>
#include <cmath>
#include <limits>
#include "../myHeaders/CompactControlBlockImpl.h"
#include "../include/ICompact.h"

class CompactControlBlockImpl;

class CompactImpl : public ICompact
{
public:
    static ILogger* logger;

public:
    class Iterator : public IIterator
    {
    public:
        Iterator(IVector* iVector_, IMultiIndex* index_, IMultiIndex* bypassOrder_, const std::shared_ptr<CompactControlBlockImpl>& pBlock_);

        IIterator* getNext() override;

        IIterator* clone() const override;

        RC next() override;

        bool isValid() const override;

        RC getVectorCopy(IVector*& val) const override;

        RC getVectorCoords(IVector* const& val) const override;

    public:
        static ILogger* LoggerIterator;

    public:
        ~Iterator() override;

    private:
        IVector* iVector;
        IMultiIndex* index;
        IMultiIndex* bypassOrder;
        std::shared_ptr<CompactControlBlockImpl> pBlock;
    };

public:
    CompactImpl(size_t dim_, IVector* begin_, IVector* end_, IMultiIndex* order_);

    ICompact* clone() const override;

    bool isInside(const IVector* const& vec) const override;

    RC getVectorCopy(const IMultiIndex* index, IVector*& val) const override;

    RC getVectorCoords(const IMultiIndex* index, IVector* const& val) const override;

    RC getLeftBoundary(IVector*& vec) const override;

    RC getRightBoundary(IVector*& vec) const override;

    size_t getDim() const override;

    IMultiIndex* getGrid() const override;

    IIterator* getIterator(const IMultiIndex* const& index, const IMultiIndex* const& bypassOrder) const override;

    IIterator* getBegin(const IMultiIndex* const& bypassOrder) const override;

    IIterator* getEnd(const IMultiIndex* const& bypassOrder) const override;

    ~CompactImpl() override;

private:
    size_t dim;
    IVector* begin;
    IVector* end;
    IMultiIndex* order;

    std::shared_ptr<CompactControlBlockImpl> pBlock;
};
