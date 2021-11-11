#include "../include/IVector.h"
#include "../include/ISet.h"
#include "../include/ILogger.h"
#include "../include/RC.h"
#include "../include/ICompact.h"
#include "../myHeaders/NecessaryFuncs.h"
#include "../test/main.h"
#include <iostream>



int main()
{
    VectorTest();
    SetTest();
    SetIteratorTest();
    MultiIndexTest();
    CompactTest();

    std::cout << "All tests completed." << std::endl;

    return 0;
}


void VectorTest()
{
    std::cout<< "\n\n\n///////////////Vector test///////////// \n";
    double data[] {2.5, 3.1, 6.7, 1.8, 9.5};

    ILogger* log = ILogger::createLogger();
    IVector::setLogger(log);

    IVector* v1 = IVector::createVector(5, data);
    IVector* v2 = IVector::createVector(5, data);


    std::cout<<"\nSize allocated v1 :\n";
    std::cout<<v1->sizeAllocated()<<std::endl;

    std::cout<<"\nAdd test v1 + v2:\n";
    IVector* v3 = IVector::add(v1, v2);
    NecessaryFuncs::Print(v3, "v3");

    std::cout<<"\nInc test v1 += v2:\n";
    v1->inc(v2);
    NecessaryFuncs::Print(v1, "v1");

    std::cout<<"\nDec test v1 -= v2:\n";
    v1->inc(v2);
    NecessaryFuncs::Print(v1, "v1");

    std::cout<<"\nScale test v1 * "<< 5 <<":\n";
    v1->scale(5);
    NecessaryFuncs::Print(v1, "v1");


    std::cout<<"\nClone test v_1 := v1 :\n";
    IVector* v_1 = v1->clone();
    NecessaryFuncs::Print(v_1, "v_1");


    std::cout<<"\nForeach test for v1:\n    ";
    v1->foreach([](double x)
    {
        std::cout << x + 10.0<< "***";
    });
    std::cout<<"\n";

    std::cout<<"\nNorms test for v1:\n    ";
    std::cout<< "Norm 1 = " << v1->norm(IVector::NORM::FIRST) << "\n   ";
    std::cout<< "Norm 2 = " << v1->norm(IVector::NORM::SECOND) << "\n   ";
    std::cout<< "Norm CHEBYSHEV = " << v1->norm(IVector::NORM::CHEBYSHEV) << "\n   ";

    std::cout<<"\nEquals test :\n    ";
    std::cout<< "v1 == v2? ans: "<<IVector::equals(v1, v2, IVector::NORM::CHEBYSHEV, 0.001)<< "\n";

    std::cout<<"\ncopyInstance test v1->v3:\n";
    IVector::copyInstance(v3, v1);
    NecessaryFuncs::Print(v3, "v3");

    std::cout<<"\nmoveInstance test v1->v2:\n";
    IVector::moveInstance(v2, v1);
    NecessaryFuncs::Print(v2, "v2");
    if(v1 == nullptr)
    {
        std::cout << "v1 == nullptr\n";
    }
    else
    {
        std::cout << "v1 != nullptr\n";
        delete v1;
    }


    delete v1;
    delete v2;
    delete v3;
    delete v_1;
    delete log;
}


void SetTest()
{
    std::cout<< "\n\n\n///////////////Set test///////////// \n";

    ILogger* log = ILogger::createLogger();
    IVector::setLogger(log);
    ISet::setLogger(log);

    double tol = 0.1;
    double v1[] = {1, 0, 2};
    double v2[] = {5, 1, 2};
    double v3[] = {4, 1, -1};
    double v4[] = {0, 0, 6};
    double v5[] = {-1, -1, -1};

    RC err;

    IVector* vec1 = IVector::createVector(3, v1);
    IVector* vec2 = IVector::createVector(3, v2);
    IVector* vec3 = IVector::createVector(3, v3);
    IVector* vec4 = IVector::createVector(3, v4);
    IVector* vec5 = IVector::createVector(3, v5);
    IVector* temp_v = nullptr;

    ISet* set = ISet::createSet(), *set1 = ISet::createSet(), *set2 = ISet::createSet();

    set1->insert(vec1, IVector::NORM::FIRST, 1.e-4);
    set1->insert(vec2, IVector::NORM::FIRST, 1.e-4);
    set1->insert(vec3, IVector::NORM::FIRST, 1.e-4);

    set2->insert(vec1, IVector::NORM::FIRST, 1.e-4);
    set2->insert(vec2, IVector::NORM::FIRST, 1.e-4);
    set2->insert(vec3, IVector::NORM::FIRST, 1.e-4);

    std::cout<<"\nInsert test:\n";
    NecessaryFuncs::Print(set1, "set1");

    std::cout<<"\nGetSize test: \n";
    std::cout<< "   " <<set1->getSize()<<"\n";

        std::cout<<"\nEquals test set1 == set2? Ans: ";
    if(ISet::equals(set1, set2, IVector::NORM::SECOND, tol))
        std::cout<<"true\n";
    else
        std::cout<<"false\n";

    ISet* intersectionSet = ISet::makeIntersection(set1, set2, IVector::NORM::FIRST, tol);
    ISet* unionSet = ISet::makeUnion(set1, set2, IVector::NORM::FIRST, tol);
    ISet* subSet = ISet::sub(set1, set2, IVector::NORM::FIRST, tol);
    ISet* symSubSet = ISet::symSub(set1, set2, IVector::NORM::FIRST, tol);

    NecessaryFuncs::Print(intersectionSet, "Intersection");
    NecessaryFuncs::Print(unionSet, "Union");
    NecessaryFuncs::Print(subSet, "Sub");
    NecessaryFuncs::Print(symSubSet, "Symsub");

    std::cout<<"\nCopy test: \n";
    set1->getCopy(0, temp_v);
    NecessaryFuncs::Print(temp_v, "Copied vec");

    std::cout<<"\ngetCoords test: \n";
    set1->getCoords(1, temp_v);
    NecessaryFuncs::Print(temp_v, "result vec");

    std::cout<<"\nRemoveByIndex test: \n";
    set1->remove(1);
    NecessaryFuncs::Print(set1, "set1");

    std::cout<<"\nRemoveVector test: \n";
    set1->remove(vec3, IVector::NORM::FIRST, tol);
    NecessaryFuncs::Print(set1, "set1");

    set1->insert(vec4, IVector::NORM::FIRST, 1.e-4);
    set1->insert(vec5, IVector::NORM::FIRST, 1.e-4);
    NecessaryFuncs::Print(set1, "set1");

    std::cout<<"\nFindFirst test: \n";
    err = set1->findFirst(vec4, IVector::NORM::FIRST, tol);
    if(err != RC::SUCCESS)
    {
        std::cout<<"Error!!!\n";
    }
    else
    {
        std::cout<<"    Success.\n";
    }

    std::cout<<"\nFindFirstAndCopy test: \n";
    err = set1->findFirstAndCopy(vec4, IVector::NORM::FIRST, tol, temp_v);
    NecessaryFuncs::Print(temp_v, "result");

    std::cout<<"\nFindFirstAndCopyCoords test: \n";
    err = set1->findFirstAndCopy(vec4, IVector::NORM::FIRST, tol, temp_v);
    NecessaryFuncs::Print(temp_v, "result");


    delete set;
    delete vec1;
    delete vec2;
    delete vec3;
    delete vec4;
    delete vec5;
    delete log;
}

void SetIteratorTest()
{
    std::cout << "\n\n\n /////////////// Set Iterator test //////////////////////\n";
    double data1[6] = {2.5, 3.1, 6.7, 1.8, 9.5};
    double data2[6] = {0.5, 1.1, 2.7, 1.5, 19.5};
    double data3[6] = {-2.5, -3.1, -6.7, -1.8, -7.5};

    ILogger* log = ILogger::createLogger();
    ISet* set = ISet::createSet();
    IVector::setLogger(log);
    ISet::setLogger(log);

    IVector* vec1 = IVector::createVector(6, data1);
    IVector* vec2 = IVector::createVector(6, data2);
    IVector* vec3 = IVector::createVector(6, data3);

    set->insert(vec1, IVector::NORM::SECOND, 1.e-4);
    set->insert(vec2, IVector::NORM::SECOND, 1.e-4);
    set->insert(vec3, IVector::NORM::SECOND, 1.e-4);

    IVector* vector;
    ISet::IIterator* iterator1 = set->getIterator(0);
    iterator1->getVectorCopy(vector);
    std::cout<<"\ngetIterator test: \n";
    std::cout<<"    vector == vec1?\n   Ans:  " << IVector::equals(vec1, vector, IVector::NORM::CHEBYSHEV, 0.001) << "\n";
    delete vector;


    vector = nullptr;
    ISet::IIterator* iterator2 = iterator1->getNext(2);
    iterator2->getVectorCopy(vector);
    std::cout<<"\ngetVectorCopy && GetNext test: \n";
    std::cout<<"    vector == vec3?\n   Ans:  " << IVector::equals(vec3, vector, IVector::NORM::CHEBYSHEV, 0.001) << "\n";
    delete vector;




    delete iterator1;
    delete iterator2;
    delete vec1;
    delete vec2;
    delete vec3;
    delete set;
    delete log;

}

void MultiIndexTest()
{
    std::cout << "\n\n\n /////////////// MultiIndex test //////////////////////\n";
    ILogger* log = ILogger::createLogger();
    IMultiIndex::setLogger(log);
    size_t dim = 5;
    size_t data1[5] = {1, 2, 3, 4, 5};
    size_t data2[5] = {3, 4, 5, 6, 7};
    size_t val = 0;
    size_t coord = 3;
    size_t inc = 12;

    std::cout << "\ncreate test\n";
    IMultiIndex* index = IMultiIndex::createMultiIndex(dim, data1);
    NecessaryFuncs::Print(index, "index");

    std::cout << "\nclone test\n";
    IMultiIndex *index1 = index->clone();
    NecessaryFuncs::Print(index, "index1");
    delete index1;

    std::cout << "\nsetData test\n";
    index->setData(sizeof(data2) / sizeof(data2[0]), data2);
    NecessaryFuncs::Print(index, "index");

    std::cout << "\ngetAxisIndex test\n";
    index->getAxisIndex(coord, val);
    std::cout << "  val == " << val << "\n";

    std::cout << "\ngetGrid test\n";
    val = index->getDim();
    std::cout << "  Dim == " << val << "\n";

    std::cout <<"\nsetAxisIndex test\n";
    index->setAxisIndex(coord - 1, val);
    NecessaryFuncs::Print(index, "setAxisIndex");

    std::cout << "\nincAxisIndex test\n";
    index->incAxisIndex(coord, inc);
    NecessaryFuncs::Print(index, "incAxisIndex");

    delete index;
    delete log;
}

void CompactTest()
{
    std::cout<< "\n\n\n///////////////Compact test///////////// \n";

    ILogger* log = ILogger::createLogger();
    ICompact::setLogger(log);
    size_t _dim = 2;

    double leftData1[_dim] = {4,0};
    double rightData1[_dim] = {0,5};
    size_t grid1[_dim] = {3,3};

    double leftData2[_dim] = {1,4};
    double rightData2[_dim] = {2,6};
    size_t grid2[_dim] = {3,3};

    IVector* left1 = IVector::createVector(_dim, leftData1);
    IVector* right1 = IVector::createVector(_dim, rightData1);
    IVector* left2 = IVector::createVector(_dim, leftData2);
    IVector* right2 = IVector::createVector(_dim, rightData2);

    std::cout << "\nCreate test\n";
    IMultiIndex* nodeQuantity1 = IMultiIndex::createMultiIndex(_dim, grid1);
    ICompact* compact1 = ICompact::createCompact(left1, right1, nodeQuantity1);
    NecessaryFuncs::Print(compact1, "compact1");

    IMultiIndex* nodeQuantity2 = IMultiIndex::createMultiIndex(_dim, grid2);
    ICompact* compact2 = ICompact::createCompact(left2, right2, nodeQuantity2);
    NecessaryFuncs::Print(compact2, "compact2");

    std::cout << "\ngetDim test\n";
    IMultiIndex *tmp = compact1->getGrid();
    NecessaryFuncs::Print(tmp, "tmp");
    delete tmp;

    std::cout << "\nisInside test\n";
    double data[_dim] = {2,2};
    IVector* vec1 = IVector::createVector(_dim,data);
    std::cout << "\nDoes vec1 inside compact1?\n  Ans: " << compact1->isInside(vec1) << std::endl;
    delete vec1;

    std::cout << "\nIntersection test\n";
    size_t intersectionGrid[_dim] = {3,3};
    IMultiIndex* nodeQuantityIntersection = IMultiIndex::createMultiIndex(_dim, intersectionGrid);
    ICompact* Intersection = ICompact::createIntersection(compact1,compact2,nodeQuantityIntersection, 0.1);
    NecessaryFuncs::Print(Intersection, "Intersection");

    std::cout << "\nSpan test\n";
    size_t spanGrid[_dim] = {4,4};
    IMultiIndex* nodeQuantitySpan = IMultiIndex::createMultiIndex(_dim, spanGrid);
    ICompact* Span = ICompact::createCompactSpan(compact1,compact2, nodeQuantitySpan);
    NecessaryFuncs::Print(Span, "Span");

    delete Intersection;
    delete Span;

    std::cout << "\nCompact iterator tests:\n";
    size_t bypassData[_dim] = {0,1};
    IMultiIndex* bypassOrder = IMultiIndex::createMultiIndex(_dim, bypassData);

    std::cout << "\ngetBegin test\n";
    ICompact::IIterator* iter = compact2->getBegin(bypassOrder);
    IVector* tVec;
    iter->getVectorCopy(tVec);
    NecessaryFuncs::Print(tVec, "getBegin");
    delete tVec;
    tVec = nullptr;

    std::cout << "\ngetEnd test\n";
    iter = compact2->getEnd(bypassOrder);
    iter->getVectorCopy(tVec);
    NecessaryFuncs::Print(tVec, "getEnd");
    delete tVec;
    tVec = nullptr;

    std::cout << "\nNext test\n";
    iter = compact2->getBegin(bypassOrder);
    ICompact::IIterator* iter1 = iter->getNext();
    iter1->getVectorCopy(tVec);
    NecessaryFuncs::Print(tVec, "Next");
    delete tVec;
    tVec = nullptr;

    delete iter;
    delete iter1;


    delete left1;
    delete right1;
    delete nodeQuantity1;
    delete compact1;


    delete left2;
    delete right2;
    delete nodeQuantity2;
    delete compact2;

    delete nodeQuantitySpan;
    delete log;
}


