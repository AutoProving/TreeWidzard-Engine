// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef WITNESSSET_H
#define WITNESSSET_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "Hasher.hpp"
#include "Witness.h"

// TODO: Should be scoped / namespaced
using valueType = std::shared_ptr<Witness>;

class AbstractIterator {
 public:
  virtual const valueType dereference(){};
  virtual void increment(){};
  virtual void increment(int){};
  // virtual void decrement() = 0;
  virtual bool isDifferent(AbstractIterator &rhs){};
  // virtual bool isEqual(AbstractIterator &rhs);
  // virtual bool isLess(AbstractIterator &rhs);
};

class BaseIterator {
 public:
  std::unique_ptr<AbstractIterator> it;
  BaseIterator(const BaseIterator &iterator) { *it = *(iterator.it); };
  BaseIterator(std::unique_ptr<AbstractIterator> it) {
    this->it = std::move(it);
  }
  BaseIterator(){};
  const valueType operator*() const { return it->dereference(); }
  BaseIterator &operator++() {
    it->increment();
    return *this;
  }
  BaseIterator operator++(int i) {
    it->increment(i);
    return *this;
  }

  /*BaseIterator operator++(){
      it->increment();
      return *this;
  }*/
  bool operator!=(const BaseIterator &rhs) {
    return it->isDifferent(*(rhs.it));
  }

  // bool operator<(BaseIterator &rhs){
  //	return it->isLess(rhs->it);
  // }
  //	return it->isEqual(rhs->it);
  // }
};

class WitnessSet {  // data structure to store 'std::shared_ptr<Witness>'
 public:
  virtual ~WitnessSet(){};
  std::shared_ptr<WitnessSet> relabel(std::map<unsigned, unsigned> map) {
    std::shared_ptr<WitnessSet> witnessSet = this->createEmptyWitnessSet();
    for (auto witness : *this) {
      witnessSet->insert(witness->relabel(map));
    }
    return witnessSet;
  }
  virtual BaseIterator begin() const {
    std::cout << "Error: WitnessSet begin()." << std::endl;
    exit(20);
  };
  virtual BaseIterator end() const {
    std::cout << "Error: WitnessSet end()" << std::endl;
    exit(20);
  };
  virtual void insert(std::shared_ptr<Witness> w) {
    std::cout << " \n Error: WitnessSet insert." << std::endl;
    exit(20);
  };
  virtual void union_set_witness(std::shared_ptr<WitnessSet> witnessSet) {
    std::cout << "Error: Set union of witnessSet class." << std::endl;
    exit(20);
  };
  virtual void print() {
    std::cout << "Error: WitnessSet print." << std::endl;
    exit(20);
  };
  virtual std::string witnessSetInformation() {
    std::cout << "Error: WitnessSet witnessSetInformation" << std::endl;
    exit(20);
  }
  virtual void hash(Hasher &h) const {
    std::cout << "Error: WitnessSet hash." << std::endl;
    exit(20);
  }
  friend bool operator==(WitnessSet &lhs, WitnessSet &rhs) {
    return lhs.isEqual(rhs);
  };
  virtual bool isEqual(WitnessSet &rhs) {
    std::cout << "Error: WitnessSet isEqual." << std::endl;
    exit(20);
  };
  // virtual bool operator!=(WitnessSet &rhs);
  friend bool operator!=(WitnessSet &lhs, WitnessSet &rhs) {
    return !lhs.isEqual(rhs);
  };
  friend bool operator<(WitnessSet &lhs, WitnessSet &rhs) {
    return lhs.isLess(rhs);
  };

  virtual bool isLess(WitnessSet &rhs) {
    std::cout << "Error: WitnessSet isLess." << std::endl;
    exit(20);
  };
  virtual int size() {
    std::cout << "Error: WitnessSet size function." << std::endl;
    exit(20);
  };
  virtual std::shared_ptr<WitnessSet> createEmptyWitnessSet() {
    std::cout << "Error: WitnessSet createEmptyWitnessSet" << std::endl;
    exit(20);
  };
};

using WitnessSetPointer = std::shared_ptr<WitnessSet>;
template <class T>
class WitnessSetTypeOne : public WitnessSet {
 public:
  static std::map<std::shared_ptr<Witness>, int, Witness::IsLessSharedPtr>
      allWitnesses;
  static std::vector<std::shared_ptr<Witness>> witnessVector;
  std::vector<uint8_t> mask;
  class WitnessSetTypeOneIterator : public AbstractIterator {
   private:
    WitnessSetTypeOne<T> *self;
    int idx;

   public:
    WitnessSetTypeOneIterator(WitnessSetTypeOne<T> *self_, int idx_)
        : self(self_), idx(idx_) {}
    virtual const valueType dereference() { return (witnessVector[idx]); };
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

    virtual bool isDifferent(AbstractIterator &rhs) {
      if (WitnessSetTypeOneIterator *e =
              dynamic_cast<WitnessSetTypeOneIterator *>(&rhs)) {
        return idx != e->idx;
      }
    };
  };
  BaseIterator begin() const {
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeOneIterator(this, -1)));
    baseIterator++;
    return baseIterator;
  }
  BaseIterator end() const {
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeOneIterator(this, 8 * mask.size())));
    return baseIterator;
  }
  virtual void insert(std::shared_ptr<Witness> w);
  virtual void union_set_witness(std::shared_ptr<WitnessSet> witnessSet);
  virtual void print();
  std::string witnessSetInformation() override;
  void hash(Hasher &h) const override;
  virtual bool isLess(WitnessSet &rhs);
  virtual bool isEqual(WitnessSet &rhs);
  virtual int size();
  virtual std::shared_ptr<WitnessSet> createEmptyWitnessSet() override {
    return std::make_shared<WitnessSetTypeOne<T>>();
  }
  int witnessVectorSize() { return witnessVector.size(); }
};

template <class T>
class WitnessSetTypeTwo : public WitnessSet {
 private:
  struct compare {
    bool operator()(const std::shared_ptr<Witness> lhs,
                    const std::shared_ptr<Witness> rhs) const {
      return *lhs < *rhs;
    }
  };
  std::set<std::shared_ptr<Witness>, compare> container;
  class WitnessSetTypeTwoIterator : public AbstractIterator {
   private:
    std::set<std::shared_ptr<Witness>>::iterator it;

   public:
    WitnessSetTypeTwoIterator(std::set<std::shared_ptr<Witness>>::iterator it_)
        : it(it_) {}
    virtual const valueType dereference() { return *it; };
    virtual void increment() { it++; };
    virtual void increment(int i) { it++; };
    virtual bool isDifferent(AbstractIterator &rhs) {
      if (WitnessSetTypeTwoIterator *e =
              dynamic_cast<WitnessSetTypeTwoIterator *>(&rhs)) {
        return it != e->it;
      }
    };
  };

 public:
  BaseIterator begin() const {
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeTwoIterator(container.begin())));
    return baseIterator;
  }
  BaseIterator end() const {
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeTwoIterator(container.end())));
    return baseIterator;
  }
  virtual void insert(std::shared_ptr<Witness> w);
  virtual void union_set_witness(std::shared_ptr<WitnessSet> witnessSet);
  virtual void print();
  std::string witnessSetInformation() override;
  virtual bool isLess(WitnessSet &rhs);
  virtual bool isEqual(WitnessSet &rhs);
  virtual int size();
  void hash(Hasher &h) const override;
  virtual std::shared_ptr<WitnessSet> createEmptyWitnessSet() override {
    return std::make_shared<WitnessSetTypeTwo<T>>();
  }
};

/////////////////////////////////////////////////////////////////////////////
/// Definition of the WitnessSet class
//////////////////////////////////////////////////////////////////////////////
template <class T>
std::map<std::shared_ptr<Witness>, int, Witness::IsLessSharedPtr>
    WitnessSetTypeOne<T>::allWitnesses;
template <class T>
std::vector<std::shared_ptr<Witness>> WitnessSetTypeOne<T>::witnessVector;

template <class T>
void WitnessSetTypeOne<T>::insert(std::shared_ptr<Witness> ws) {
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
void WitnessSetTypeOne<T>::union_set_witness(
    std::shared_ptr<WitnessSet> witnessSet) {
  if (WitnessSetTypeOne<T> *e =
          dynamic_cast<WitnessSetTypeOne<T> *>(&*witnessSet)) {
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
std::string WitnessSetTypeOne<T>::witnessSetInformation() {
  std::string info;
  for (auto element : *this) info = info + element->witnessInformation();
  return info;
}

template <class T>
void WitnessSetTypeOne<T>::hash(Hasher &h) const {
  for (const auto &element : *this) {
    element->hash(h);
    h << -2u;
  }
}

template <class T>
bool WitnessSetTypeOne<T>::isLess(WitnessSet &rhs) {
  if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&rhs)) {
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
  if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&rhs)) {
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
  for (int i = 0; i < 8 * mask.size(); i++) ret += (mask[i / 8] >> (i % 8) & 1);
  return ret;
}

/////////////WitnessSet TYPE Two////////////////
template <class T>
void WitnessSetTypeTwo<T>::insert(std::shared_ptr<Witness> ws) {
  container.insert(ws);
}
template <class T>
void WitnessSetTypeTwo<T>::union_set_witness(
    std::shared_ptr<WitnessSet> witnessSet) {
  for (auto element : *witnessSet) container.insert(element);
}
template <class T>
void WitnessSetTypeTwo<T>::print() {
  for (auto element : *this) element->print();
}
template <class T>
std::string WitnessSetTypeTwo<T>::witnessSetInformation() {
  std::string info;
  for (auto element : *this) info = info + element->witnessInformation();
  return info;
}
template <class T>
void WitnessSetTypeTwo<T>::hash(Hasher &h) const {
  for (const auto &element : *this) {
    element->hash(h);
    h << -2u;
  }
}

template <class T>
bool WitnessSetTypeTwo<T>::isLess(WitnessSet &rhs) {
  if (WitnessSetTypeTwo<T> *e = dynamic_cast<WitnessSetTypeTwo<T> *>(&rhs)) {
    if (size() < rhs.size()) {
      return true;
    } else if (rhs.size() < size()) {
      return false;
    } else {
      auto it = rhs.begin();
      for (auto element : *this) {
        if (**it < *element) {
          return false;
        } else if (*element < **it) {
          return true;
        }
        ++it;
      }
      return false;
    }
  } else {
    std::cout << "Error: In isLess, WitnessSet type cast error." << std::endl;
    exit(20);
  }
}

template <class T>
bool WitnessSetTypeTwo<T>::isEqual(WitnessSet &rhs) {
  if (WitnessSetTypeTwo *e = dynamic_cast<WitnessSetTypeTwo *>(&rhs)) {
    if (size() != rhs.size()) {
      return false;
    } else {
      auto it = rhs.begin();
      for (auto element : *this) {
        if (!(*element == **it)) {
          return false;
        }
        ++it;
      }
    }
    return true;
  } else {
    std::cout << "Error: In isEqual, WitnessSet type cast error." << std::endl;
    exit(20);
  }
}

template <class T>
int WitnessSetTypeTwo<T>::size() {
  return container.size();
}
#endif
