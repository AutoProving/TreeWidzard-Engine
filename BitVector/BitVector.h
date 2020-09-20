#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include <vector>
#include <utility>

template <class Type>
class LargeBitVector
{
private:
    
    std::vector<bool> array;
    
    unsigned int writeOnHeader(unsigned int pointer, unsigned int value, unsigned int b);
    unsigned int readFromHeader(unsigned int pointer);
    unsigned int countNumberOfBits(unsigned int value);
    unsigned int getWordSize();
    unsigned int jumpSpace(unsigned int n);
    unsigned int getFirstBit();
    void incrementSize();
    void decrementSize();
    
    void doubleMaximumSize();

    void quickSort(int l, int r);
    
    std::vector<bool> toBits(int value);
    int fromBits(std::vector<bool> v, int dummy);
    
    std::vector<bool> toBits(std::pair<int, int> value);
    std::pair<int, int> fromBits(std::vector<bool> v, std::pair<int, int> dummy);

public:

    LargeBitVector(unsigned int wordSize, unsigned int maximumSize);
    unsigned int getMaxSize();
    unsigned int size();
    void insertSorted(Type value);
    void push_back(Type value);
    Type at(int index);
    void set(Type value, int index);
    void swap(unsigned int i, unsigned int j);
    void sort(bool ascendingOrder);
    Type back();
    bool binarySearch(Type value);
    bool find(Type value);
    void remove(unsigned int i);
    void pop_back();
    void print();
    LargeBitVector<Type> unionSets(LargeBitVector<Type> aux);
    LargeBitVector<Type> intersetionSets(LargeBitVector<Type> aux);
    bool operator==(LargeBitVector<Type> &aux); 
    bool operator<(LargeBitVector<Type> &aux);
    void normalize();
};

template class LargeBitVector<int>;
template class LargeBitVector<std::pair<int, int>>;

#endif // END of bitvector.h