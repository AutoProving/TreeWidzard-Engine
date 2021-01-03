// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef WITNESSSET_H
#define WITNESSSET_H

#include <map>
#include <memory>
#include <vector>
#include <set>
#include "Witness.h"

typedef shared_ptr<Witness> valueType;

class AbstractIterator{
public:
    virtual valueType dereference(){};
    virtual void increment() {};
    virtual void increment(int) {};
    //virtual void decrement() = 0;
    virtual bool isDifferent(AbstractIterator &rhs){};
    //virtual bool isEqual(AbstractIterator &rhs);
    //virtual bool isLess(AbstractIterator &rhs);
};

class BaseIterator {
public:
    std::unique_ptr<AbstractIterator> it;
    BaseIterator(const BaseIterator &iterator){
        *it = *(iterator.it);
    };
    BaseIterator(std::unique_ptr<AbstractIterator> it){
        this->it = std::move(it);
    }
    BaseIterator(){};
    valueType operator*(){
        return it->dereference();
    }
    BaseIterator& operator++(){
        it->increment();
        return *this;
    }
    BaseIterator operator++(int i){
        it->increment(i);
        return *this;
    }

    /*BaseIterator operator++(){
        it->increment();
        return *this;
    }*/
    bool operator != (const BaseIterator &rhs){
        return it->isDifferent(*(rhs.it));
    }

    //bool operator<(BaseIterator &rhs){
    //	return it->isLess(rhs->it);
    //}
    //	return it->isEqual(rhs->it);
    //}

};

class WitnessSet { // data structure to store 'shared_ptr<Witness>'
public:
    virtual ~WitnessSet(){};
    virtual shared_ptr<WitnessSet> relabel(map<unsigned,unsigned> map){cout<<"Error: Base WitnessSet relabel function"<<endl; exit(20);}
    virtual BaseIterator begin(){cout<<"Error: WitnessSet begin()."<<endl; exit(20);};
    virtual BaseIterator end(){cout<< "Error: WitnessSet end()"<<endl; exit(20);};
    virtual void insert(shared_ptr<Witness> w){cout<<"Error: WitnessSet insert."<<endl; exit(20);};
    virtual void union_set_witness(shared_ptr<WitnessSet> witnessSet){cout<<"Error: Set union of witnessSet class."<<endl; exit(20);};
    virtual void print(){cout<<"Error: WitnessSet print."<<endl; exit(20);};
    friend bool operator==(WitnessSet &lhs, WitnessSet &rhs){
        return lhs.isEqual(rhs);
    };
    virtual bool isEqual(WitnessSet &rhs){cout<<"Error: WitnessSet isEqual."<<endl; exit(20);};
    //virtual bool operator!=(WitnessSet &rhs);
    friend bool operator!=(WitnessSet &lhs, WitnessSet &rhs){
        return !lhs.isEqual(rhs);
    };
    friend bool operator<(WitnessSet &lhs,WitnessSet &rhs){
        return lhs.isLess(rhs);
    };
    virtual bool isLess(WitnessSet &rhs){cout<<"Error: WitnessSet isLess."<<endl; exit(20); };
    virtual int size(){cout<<"Error: WitnessSet size function."<<endl; exit(20);};
    virtual shared_ptr<WitnessSet> createEmptyWitnessSet(){cout<<"Error: WitnessSet createEmptyWitnessSet"<<endl; exit(20);};

};
typedef shared_ptr<WitnessSet> WitnessSetPointer;
template <class T>
class WitnessSetTypeOne : public WitnessSet{
public:
    static std::map<shared_ptr<Witness>, int, Witness::IsLessSharedPtr> allWitnesses;
    static std::vector<shared_ptr<Witness> > witnessVector;
    vector<uint8_t> mask;
    class WitnessSetTypeOneIterator : public AbstractIterator{
    private:
        WitnessSetTypeOne<T> *self;
        int idx;
    public:
        WitnessSetTypeOneIterator(WitnessSetTypeOne<T> *self_, int idx_) : self(self_), idx(idx_) {}
        virtual valueType dereference(){return (witnessVector[idx]);};
        virtual void increment() {
            while (++idx < 8 * self->mask.size()) {
                if (self->mask[idx / 8] >> (idx % 8) & 1) break;
            }
        };
        virtual void increment(int i) {
            while (++idx < 8 * self->mask.size()) {
                if (self->mask[idx / 8] >> (idx % 8) & 1) break;
            }
        };

        virtual bool isDifferent(AbstractIterator &rhs){
            if (WitnessSetTypeOneIterator *e = dynamic_cast<WitnessSetTypeOneIterator *>(&rhs)){
                return idx != e->idx;
            }
        };

    };
    BaseIterator begin() {
        BaseIterator baseIterator(unique_ptr<AbstractIterator>(new WitnessSetTypeOneIterator(this, -1)));
        baseIterator++;
        return baseIterator;
    }
    BaseIterator end() {
        BaseIterator baseIterator(unique_ptr<AbstractIterator>(new WitnessSetTypeOneIterator(this, 8 * mask.size())));
        return baseIterator;
    }
    virtual void insert(shared_ptr<Witness> w);
    virtual void union_set_witness(shared_ptr<WitnessSet> witnessSet);
    virtual void print();
    virtual bool isLess(WitnessSet &rhs);
    virtual bool isEqual(WitnessSet &rhs);
    virtual int size();
    virtual shared_ptr<WitnessSet> createEmptyWitnessSet(){
        shared_ptr<WitnessSetTypeOne<T>> witnessSet(new WitnessSetTypeOne<T>);
        return witnessSet;
    }
    virtual shared_ptr<WitnessSet> relabel(map<unsigned,unsigned> map){
        shared_ptr<WitnessSet> witnessSet = this->createEmptyWitnessSet();
        for(auto witness:*this){
            witnessSet->insert(witness->relabel(map));
        }
        return witnessSet;
    }

};

template <class T>
class WitnessSetTypeTwo: public WitnessSet{
private:
    struct compare {
        bool operator()(const shared_ptr<Witness> lhs,
                        const shared_ptr<Witness> rhs) const {
            return *lhs < *rhs;
        }
    };
    std::set<shared_ptr<Witness>, compare> container;
    class WitnessSetTypeTwoIterator : public AbstractIterator{
    private:
        std::set<shared_ptr<Witness>>::iterator it;
    public:
        WitnessSetTypeTwoIterator(std::set<shared_ptr<Witness>>::iterator it_) : it(it_) {}
        virtual valueType dereference(){return *it;};
        virtual void increment() {
            it++;
        };
        virtual void increment(int i) {
            it++;
        };
        virtual bool isDifferent(AbstractIterator &rhs){
            if (WitnessSetTypeTwoIterator *e = dynamic_cast<WitnessSetTypeTwoIterator *>(&rhs)){
                return it != e->it;
            }
        };

    };
    BaseIterator begin() {
        BaseIterator baseIterator(unique_ptr<AbstractIterator>(new WitnessSetTypeTwoIterator(container.begin())));
        return baseIterator;
    }
    BaseIterator end() {
        BaseIterator baseIterator(unique_ptr<AbstractIterator>(new WitnessSetTypeTwoIterator(container.end())));
        return baseIterator;
    }
    virtual void insert(shared_ptr<Witness> w);
    virtual void union_set_witness(shared_ptr<WitnessSet> witnessSet);
    virtual void print();
    virtual bool isLess(WitnessSet &rhs);
    virtual bool isEqual(WitnessSet &rhs);
    virtual int size();
    virtual shared_ptr<WitnessSet> createEmptyWitnessSet(){
        shared_ptr<WitnessSetTypeTwo<T>> witnessSet(new WitnessSetTypeTwo<T>);
        return witnessSet;
    }
};

/////////////////////////////////////////////////////////////////////////////
/// Definition of the WitnessSet class
//////////////////////////////////////////////////////////////////////////////
template <class T>
std::map<shared_ptr<Witness>, int, Witness::IsLessSharedPtr> WitnessSetTypeOne<T>::allWitnesses;
template <class T>
std::vector<shared_ptr<Witness> > WitnessSetTypeOne<T>::witnessVector;

template <class T>
void WitnessSetTypeOne<T>::insert(shared_ptr<Witness> ws) {
    int idx = -1;
    auto it = allWitnesses.find(ws);
    if (it != allWitnesses.end()) {
        idx = it->second;
    } else {
        // new witness
        idx = allWitnesses.size();
        allWitnesses[ws] = idx;
        witnessVector.push_back(ws);
    }

    while (idx >= 8 * mask.size()) mask.push_back(0);

    mask[idx / 8] |= (1 << (idx % 8));
}
template <class T>
void WitnessSetTypeOne<T>::union_set_witness(shared_ptr<WitnessSet> witnessSet) {
    if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&*witnessSet)){
        for (int i = 0; i < e->mask.size(); i++) {
            if (i == mask.size())
                mask.push_back(e->mask[i]);
            else
                mask[i] |= e->mask[i];
        }
    }

}
template <class T>
void WitnessSetTypeOne<T>::print() {
    for (auto element : *this) element->print();
}
template <class T>
bool WitnessSetTypeOne<T>::isLess(WitnessSet &rhs) {
    if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&rhs)){
        if (size() < e->size()) {
            return true;
        } else if (e->size() < size()) {
            return false;
        } else {
            auto it = e->begin();
            for (auto element : *this) {
                if (**it < *element) {
                    return false;
                } else if (*element < **it) {
                    return true;
                }
                it++;
            }
            return false;
        }
    }
}

template <class T>
bool WitnessSetTypeOne<T>::isEqual(WitnessSet &rhs) {
    if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&rhs)){
        return !(*this < *e or *e < *this);
    }
}

/*
bool WitnessSetTypeOne::operator==(WitnessSet &rhs) {
    if (WitnessSetTypeOne *e = dynamic_cast<WitnessSetTypeOne *>(&rhs)){
        return mask == e->mask;
    }
}


bool WitnessSetTypeOne::operator<(WitnessSet &rhs) {
    if (WitnessSetTypeOne *e = dynamic_cast<WitnessSetTypeOne *>(&rhs)){
        return mask < e->mask;
    }
}


bool WitnessSetTypeOne::operator!=(WitnessSet &rhs) {
    if (WitnessSetTypeOne *e = dynamic_cast<WitnessSetTypeOne *>(&rhs)){
        return !(*this == rhs);
    }

}*/

template <class T>
int WitnessSetTypeOne<T>::size() {
    int ret = 0;
    for (int i = 0; i < 8 * mask.size(); i++)
        ret += (mask[i / 8] >> (i % 8) & 1);
    return ret;
}


/////////////WitnessSet TYPE Two////////////////
template <class T>
void WitnessSetTypeTwo<T>::insert(shared_ptr<Witness> ws) {
    container.insert(ws);
}
template <class T>
void WitnessSetTypeTwo<T>::union_set_witness(shared_ptr<WitnessSet> witnessSet) {
    for (auto element : *witnessSet) container.insert(element);
}
template <class T>
void WitnessSetTypeTwo<T>::print() {
    for (auto element : *this) element->print();
}
template <class T>
bool WitnessSetTypeTwo<T>::isLess(WitnessSet &rhs) {
    if (WitnessSetTypeTwo<T> *e = dynamic_cast<WitnessSetTypeTwo<T> *>(&rhs)){
        if (size() < rhs.size()) {
            return true;
        } else if (rhs.size() < size()) {
            return false;
        } else {
            WitnessSetTypeTwo<T>::WitnessSetTypeTwoIterator it = rhs.begin();
            for (auto element : *this) {
                if (**it < *element) {
                    return false;
                } else if (*element < **it) {
                    return true;
                }
                it++;
            }
            return false;
        }
    }else{
        cout<<"Error: In isLess, WitnessSet type cast error."<<endl;
        exit(20);
    }
}

template <class T>
bool WitnessSetTypeTwo<T>::isEqual(WitnessSet &rhs) {
    if (WitnessSetTypeTwo *e = dynamic_cast<WitnessSetTypeTwo *>(&rhs)){
        if (size() != rhs.size()) {
            return false;
        } else {
            WitnessSetTypeTwo<T>::WitnessSetTypeTwoIterator it = rhs.begin();
            for (auto element : *this) {
                if (!(*element == **it)) {
                    return false;
                }
                it++;
            }
        }
        return true;
    }else{
        cout<<"Error: In isEqual, WitnessSet type cast error."<<endl;
        exit(20);
    }
}

template <class T>
int WitnessSetTypeTwo<T>::size() {
    return container.size();
}
#endif
