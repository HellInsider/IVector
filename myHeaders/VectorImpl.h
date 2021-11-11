#pragma once
#include <limits>
#include <cmath>
#include "../include/IVector.h"

namespace
{
    class VectorImpl : public IVector
    {

    private:
        size_t dimension = 0;
        static ILogger* pLogger;

    public:

        static void log(RC code, ILogger::Level level, const char* const& srcfile, const char* const& function, int line);

        VectorImpl(size_t dim)
        {dimension = dim;}

        ~VectorImpl() override;

        virtual size_t sizeAllocated() const;

        static IVector* createVector(size_t dim, double const* const& ptr_data);

        static RC copyInstance(IVector* const dest, IVector const* const& src);

        static RC moveInstance(IVector* const dest, IVector*& src);

        virtual IVector* clone() const;

        virtual RC scale(double multiplier);

        virtual RC inc(IVector const* const& op);

        virtual RC dec(IVector const* const& op);

        virtual double norm(NORM n) const;

        virtual RC applyFunction(const std::function<double(double)>& fun);

        virtual RC foreach(const std::function<void(double)>& fun) const;

        //////////////////////GETTERS///////////////

        virtual double const* getData() const;

        virtual size_t getDim() const;

        virtual RC getCord(size_t index, double& val) const;


        //////////////////////SETTERS///////////////

        static RC setLogger(ILogger *const logger);

        virtual RC setCord(size_t index, double val);

        RC setData(size_t dim, double const* const& ptr_data) override;

    };
    ILogger* VectorImpl::pLogger = nullptr;
};

