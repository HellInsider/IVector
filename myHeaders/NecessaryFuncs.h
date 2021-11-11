#ifndef NECESSARYFUNCS_H
#define NECESSARYFUNCS_H

#include <limits>
#include <cmath>
#include <string.h>
#include "../include/IVector.h"
#include "../include/ISet.h"
#include "../include/IMultiIndex.h"
#include "../include/ICompact.h"

namespace NecessaryFuncs
{
    void Print(const IVector* const& vec, std::string name);
    void Print(const ISet* const& _set, std::string name);
    void Print(const ICompact* const& compact, std::string name);
    void Print(const IMultiIndex* const& index, std::string name);
    void log(ILogger* logger, RC code, const char* const& srcfile, const char* const& function, int line);
}

#endif
