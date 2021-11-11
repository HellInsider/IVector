#pragma once

#include "../myHeaders/CompactImpl.h"
#include "../include/ICompactControlBlock.h"

class CompactImpl;

class CompactControlBlockImpl : public ICompactControlBlock
{
public:
    CompactControlBlockImpl(CompactImpl* compact_);

public:
    RC get(IMultiIndex* const& currentIndex, const IMultiIndex* const& bypassOrder) const override;

    RC get(const IMultiIndex* const& currentIndex, IVector* const& val) const override;

    ~CompactControlBlockImpl() override;

    void free_compact();

private:
    CompactImpl* compact;
};
