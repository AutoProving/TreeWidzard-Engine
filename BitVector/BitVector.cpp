#include "BitVector.h"
#include <iostream>
#include <cassert>
#include <set>

template <class Type>
LargeBitVector<Type>::LargeBitVector(unsigned int wordSize,  unsigned int maximumSize)
{
    unsigned int wsBits = this->countNumberOfBits(wordSize);
    unsigned int mxBits = this->countNumberOfBits(maximumSize);

    this->array.resize((wsBits << 1) + (mxBits << 2) + 6/* + 1500*/);

    unsigned int pointer = 0;
    pointer = writeOnHeader(pointer, wordSize, wsBits);
    pointer = writeOnHeader(pointer, maximumSize, mxBits);
    pointer = writeOnHeader(pointer, 0, mxBits);

    // while(this->array.size() > this->getFirstBit())
    //     this->array.pop_back();
}

template <class Type>
void LargeBitVector<Type>::doubleMaximumSize()
{
    int p = this->jumpSpace(1);

    this->array.push_back(false);
    this->array.push_back(false);

    for(int i = (int)this->array.size() - 1; i > p + 1; i--)
        this->array[i] = this->array[i - 2];

    this->array[p] = false;
    this->array[p + 1] = true;

    p = this->jumpSpace(3);
    this->array.push_back(false);
    this->array.push_back(false);

    for(int i = (int)this->array.size() - 1; i >= p; i--)
        this->array[i] = this->array[i - 2];

    this->array[p - 2] = false;
    this->array[p - 1] = true;
}

template <class Type>
unsigned int LargeBitVector<Type>::countNumberOfBits(unsigned int value)
{
    if(value == 0)
        return 1;
    unsigned int cnt = 0;
    while(value > 0)
    {
        value >>= 1;
        cnt++;
    }
    return cnt;
}

template <class Type>
unsigned int LargeBitVector<Type>::writeOnHeader(unsigned int pointer, unsigned int value, unsigned int b)
{
    for(int i = 0; i < b; i++)
    {
        if(value & (1 << i))
        {
            this->array[pointer] = 1;
            this->array[pointer + 1] = 0;
        }
        else
        {
            this->array[pointer] = 0;
            this->array[pointer + 1] = 1;
        }
        pointer += 2;
    }
    this->array[pointer] = 0;
    this->array[pointer + 1] = 0;
    pointer += 2;
    return pointer;
}

template <class Type>
unsigned int LargeBitVector<Type>::readFromHeader(unsigned int pointer)
{
    unsigned int value = 0, k = 0;
    while(this->array[pointer] or this->array[pointer + 1])
    {
        if(this->array[pointer] == 1 and this->array[pointer + 1] == 0)
            value += (1 << k);
        pointer += 2; 
        k++;
    }
    return value;
}

template <class Type>
void LargeBitVector<Type>::print()
{
    for(size_t i = 0; i < this->array.size(); i++)
        std::cout << this->array[i];
    std::cout << std::endl;
}

template <class Type>
unsigned int LargeBitVector<Type>::jumpSpace(unsigned int n)
{
    unsigned int pointer = 0;
    unsigned int cnt = 0;
    while(cnt < n)
    {
        if(this->array[pointer] == 0 and this->array[pointer + 1] == 0)
            cnt++;
        pointer += 2;
    }
    return pointer;
}

template <class Type>
unsigned int LargeBitVector<Type>::getWordSize()
{
    return this->readFromHeader(this->jumpSpace(0));
}

template <class Type>
unsigned int LargeBitVector<Type>::getMaxSize()
{
    return this->readFromHeader(this->jumpSpace(1));
}

template <class Type>
unsigned int LargeBitVector<Type>::size()
{
    return this->readFromHeader(this->jumpSpace(2));
}

template <class Type>
unsigned int LargeBitVector<Type>::getFirstBit()
{
    return this->jumpSpace(3);
}


template <class Type>
void LargeBitVector<Type>::incrementSize()
{
    unsigned int newSize = this->size() + 1;
    if(newSize == this->getMaxSize())
        this->doubleMaximumSize();
    this->writeOnHeader(this->jumpSpace(2), newSize, this->countNumberOfBits(this->getMaxSize()));
}

template <class Type>
void LargeBitVector<Type>::decrementSize()
{
    unsigned int newSize = this->size() - 1;
    this->writeOnHeader(this->jumpSpace(2), newSize, this->countNumberOfBits(this->getMaxSize()));
}

template <class Type>
std::vector<bool> LargeBitVector<Type>::toBits(int value)
{
    unsigned int sz = this->getWordSize();
    std::vector<bool> v(sz, false);

    for(size_t i = 0; i < sz; i++)
    {
        if(value & (1 << i))
            v[i] = 1;
        else
            v[i] = 0;
    }
    return v;
} 

template <class Type>
int LargeBitVector<Type>::fromBits(std::vector<bool> v, int dummy)
{
    unsigned int sz = this->getWordSize();
    int value = 0;
    for(size_t i = 0; i < sz; i++)
        if(v[i] == 1)
            value += (1 << i);
    return value;
}

template <class Type>
std::vector<bool> LargeBitVector<Type>::toBits(std::pair<int, int> value)
{
    unsigned int sz = this->getWordSize() >> 1;
    std::vector<bool> v(sz << 1, false);

    for(size_t i = 0; i < sz; i++)
    {
        if(value.first & (1 << i))
            v[i] = 1;
        else
            v[i] = 0;
    }
    for(size_t i = 0; i < sz; i++)
    {
        if(value.second & (1 << i))
            v[i + sz] = 1;
        else
            v[i + sz] = 0;
    }
    return v;
} 

template <class Type>
std::pair<int, int> LargeBitVector<Type>::fromBits(std::vector<bool> v, std::pair<int, int> dummy)
{
    std::pair<int, int> value = std::make_pair(0, 0);
    int sz = this->getWordSize() >> 1;
    for(size_t i = 0; i < sz; i++)
        if(v[i] == 1)
            value.first += (1 << i);
    for(size_t i = 0; i < sz; i++)
        if(v[i + sz] == 1)
            value.second += (1 << i);
    return value;
}

template <class Type>
void LargeBitVector<Type>::push_back(Type element)
{
    std::vector<bool> v = toBits(element);
    for(size_t i = 0; i < v.size(); i++)
        this->array.push_back(v[i]);
    this->incrementSize();
}

template <class Type>
Type LargeBitVector<Type>::at(int index)
{
    int b = this->getFirstBit() + index * this->getWordSize();

    std::vector<bool> v(this->getWordSize());
    int sz = b + this->getWordSize();
    for(size_t i = b, j = 0; i < sz; i++, j++)
    {
        v[j] = this->array[i];
    } 

    Type dummy;
    return this->fromBits(v, dummy);
}


template <class Type>
void LargeBitVector<Type>::set(Type element, int index)
{
    int b = this->getFirstBit() + index * this->getWordSize();
    std::vector<bool> v = toBits(element);
    int sz = b + this->getWordSize();
    for(size_t i = b, j = 0; i < sz; i++, j++)
        this->array[i] = v[j];
}

template <class Type>
void LargeBitVector<Type>::swap(unsigned int i, unsigned int j)
{
    Type temp = this->at(i);
    this->set(this->at(j), i);
    this->set(temp, j);
}

template <class Type>
void LargeBitVector<Type>::quickSort(int l, int r)
{
    assert(l <= r);

    int j = l - 1;

    // MAYBE REMOVE RAND

    int offset = rand() % (r - l + 1);

    this->swap(l + offset, r);

    for(int i = l; i < r; i++)
        if(this->at(i) <= this->at(r))
            this->swap(++j, i);

    this->swap(j + 1, r);

    if(l < j)
        this->quickSort(l, j);
    if(j + 2 < r)
        this->quickSort(j + 2, r);
}

template <class Type>
void LargeBitVector<Type>::sort(bool ascendingOrder)
{
    int sz = this->size();
    if(sz <= 1) return;
    this->quickSort(0, sz - 1);
    if(ascendingOrder == false)
    {
        int sz_aux = sz >> 1;
        for(int i = 0; i < sz_aux; i++)
            this->swap(i, sz - i - 1);
    }
}

template <class Type>
Type LargeBitVector<Type>::back()
{
    int sz = this->size();
    return this->at(sz - 1);
}

template <class Type>
void LargeBitVector<Type>::remove(unsigned int index)
{
    int b = this->getFirstBit() + index * this->getWordSize();
    int e = this->getFirstBit() + (this->size() - 1) * this->getWordSize();
    for(int i = b; i < e; i++)
        this->array[i] = this->array[i + this->getWordSize()];
    this->decrementSize();
    for(int i = 0; i < this->getWordSize(); i++)
        this->array.pop_back();
}

template <class Type>
void LargeBitVector<Type>::pop_back()
{
    this->remove(this->size() - 1);
}

template <class Type>
LargeBitVector<Type> LargeBitVector<Type>::unionSets(LargeBitVector<Type> aux)
{
    this->sort(true);
    aux.sort(true);

    unsigned int ws = std::max(this->getWordSize(), aux.getWordSize());

    LargeBitVector<Type> uniSets(ws, this->size() + aux.size());

    int l = 0, r = 0;
    int sz_l = this->size();
    int sz_r = aux.size();

    while(l < sz_l or r < sz_r)
    {
        if(l == sz_l)
        {
            if(uniSets.back() != aux.at(r))
                uniSets.push_back(aux.at(r));
            r++;
        }
        else if(r == sz_r)
        {
            if(uniSets.back() != this->at(l))
                uniSets.push_back(this->at(l));
            l++;
        }
        else if(this->at(l) < aux.at(r))
        {
            if(uniSets.back() != this->at(l))
                uniSets.push_back(this->at(l));
            l++;
        }
        else
        {
            if(uniSets.back() != aux.at(r))
                uniSets.push_back(aux.at(r));
            r++;
        }
    }
    return uniSets;
}

template <class Type>
bool LargeBitVector<Type>::binarySearch(Type value)
{
    int lef = 0, rig = (int)this->size() - 1;
    while(lef <= rig)
    {
        int middle = (lef + rig) >> 1;
        if(this->at(middle) ==  value)
            return true;
        else if(this->at(middle) < value)
            lef = middle + 1;
        else
            rig = middle - 1;
    }
    return false;
}

template <class Type>
bool LargeBitVector<Type>::find(Type value)
{
    for(int i = 0; i < this->size(); i++)
        if(this->at(i) == value)
            return true;
    return false;
}

template <class Type>
LargeBitVector<Type> LargeBitVector<Type>::intersetionSets(LargeBitVector<Type> aux)
{
    this->sort(true);
    aux.sort(true);

    unsigned int ws = std::max(this->getWordSize(), aux.getWordSize());
    LargeBitVector<Type> interSets(ws, this->size() + aux.size());

    int sz = aux.size();
    for(int i = 0; i < sz; i++)
    {
        if(this->binarySearch(aux.at(i)) == false)
            continue;
        if(interSets.size() == 0 or interSets.back() != aux.at(i))
            interSets.push_back(aux.at(i));
    }
    return interSets;
}

template <class Type>
void LargeBitVector<Type>::insertSorted(Type value)
{
    int k = this->size();
    for(int i = 0; i < this->size(); i++)
        if(value <= this->at(i))
        {
            k = i;
            break;
        }
    for(int i = 0; i < this->getWordSize(); i++)
        this->array.push_back(false);

    this->incrementSize();

    int b = this->getFirstBit() + k * this->getWordSize();
    int e = this->getFirstBit() + (this->size() - 1) * this->getWordSize();

    for(int i = e - 1; i >= b; i--)
        this->array[i + this->getWordSize()] = this->array[i];

    std::vector<bool> v = this->toBits(value);

    int sz = b + this->getWordSize();
    for(int i = b, j = 0; i < sz; i++, j++)
        this->array[i] = v[j];
}

template <class Type>
bool LargeBitVector<Type>::operator==(LargeBitVector<Type> &aux)
{
    if(this->size() != aux.size())
         return false;
    this->sort(true);
    aux.sort(true);
    for(int i = 0; i < this->size(); i++)
        if(this->at(i) != aux.at(i))
            return false;
    return true;
}

template <class Type>
bool LargeBitVector<Type>::operator<(LargeBitVector<Type> &aux)
{
    int sz = std::min(this->size(), aux.size());
    this->sort(true);
    aux.sort(true);

    for(int i = 0; i < sz; i++)
        if(this->at(i) < aux.at(i))
            return true;
        else if(this->at(i) > aux.at(i))
            return false;
    return this->size() < aux.size();
}

template <class Type>
void LargeBitVector<Type>::normalize() 
{
    LargeBitVector<Type> aux(this->getWordSize(), 1);
    this->sort(true);
    for(int i = 0; i < this->size(); i++)
        if(aux.size() == 0 or aux.back() != this->at(i))
            aux.push_back(this->at(i));
    this->array = aux.array;
}