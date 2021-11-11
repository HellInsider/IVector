#pragma once
#include "../include/ISetControlBlock.h"

class SetImpl;

class SetImplControlBlock : public ISetControlBlock
{
  public:
    RC getNext(IVector *const &vec, size_t &index, size_t indexInc = 1) const override;
    RC getPrevious(IVector *const &vec, size_t &index, size_t indexInc = 1) const override;

    RC getBegin(IVector *const &vec, size_t &index) const override;
    RC getEnd(IVector *const &vec, size_t &index) const override;
    static SetImplControlBlock *createControlBlock(SetImpl *const &set);

  private:
    SetImpl *set;

  protected:
    SetImplControlBlock(SetImpl *const &set);
};
