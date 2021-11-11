#include "../myHeaders/NecessaryFuncs.h"
#include <iostream>


void NecessaryFuncs::log(ILogger* logger, RC code, const char* const& srcfile, const char* const& function, int line)
{
    if (logger != nullptr)
    {
        logger->warning(code, srcfile, function, line);
    }
}

void NecessaryFuncs::Print(const IVector* const& vec, std::string name)
{
    if(name!="")
    {
        std::cout<<name<<" == \n";
    }
    std::cout<<"  ";

    if (vec != nullptr)
    {
        const double *data = vec->getData();
        for (size_t i = 0; i < vec->getDim(); i++)
        {
            std::cout<< data[i] << "; ";
        }
        std::cout<< std::endl;
    }
    else
    {
        std::cout<< "Empty. Nullptr.\n"<< std::endl;
    }

    return;
}

void NecessaryFuncs::Print(const ISet* const& _set, std::string name)
{
    if(name=="")
    {
        name = "Set";
    }

    if (_set != nullptr)
    {
        std::cout<< name << " contains: "<< std::endl;
        IVector* vec;
        for (size_t i = 0; i < _set->getSize(); i++)
        {
            _set->getCopy(i, vec);
            Print(vec, "");
            delete vec;
            vec = nullptr;
        }
        std::cout<< "End of set."<< std::endl;
    }
    else
    {
        std::cout<< "Empty. Nullptr.\n"<< std::endl;
    }
    return;
}

void NecessaryFuncs::Print(const IMultiIndex* const& index, std::string name)
{
    if(name!="")
    {
        std::cout<<name<<" == \n";
    }
    std::cout<<"  ";

    if (index != nullptr)
    {
        for (size_t i = 0; i < index->getDim(); ++i)
        {
            std::cout << index->getData()[i] << "; ";
        }
        std::cout <<"\n";
    }
    else
    {
        std::cout<< "Empty. Nullptr.\n"<< std::endl;
    }
    return;
}

void NecessaryFuncs::Print(const ICompact* const& compact, std::string name)
{
    if(name=="")
    {
        name = "Compact";
    }

    if (compact != nullptr)
    {
        std::cout<< name << " contains: "<< std::endl;
        IVector* l, *r;
        compact->getLeftBoundary(l);
        compact->getRightBoundary(r);

        std::cout <<"  left:\n    ";
        double d = 0.0;
        for (size_t i = 0; i < l->getDim(); ++i)
        {
            l->getCord(i, d);
            std::cout << d << "; ";
        }

        std::cout << "\n  right:\n    ";
        for (size_t i = 0; i < r->getDim(); ++i)
        {
            r->getCord(i, d);
            std::cout << d << "; ";
        }
        std::cout<< "\nEnd of compact.\n"<< std::endl;
        delete l;
        delete r;

    }
    else
    {
        std::cout<< "Empty. Nullptr.\n"<< std::endl;
    }

    return;
}
