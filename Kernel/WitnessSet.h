#ifndef WITNESSSET_H
#define WITNESSSET_H

#include <algorithm>
#include <bit>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_set>
#include <vector>

#include "Hasher.hpp"
#include "Witness.h"

// TODO: Should be scoped / namespaced
using valueType = std::shared_ptr<Witness>;

class AbstractIterator {
public:
  virtual const valueType dereference() = 0;
  virtual void increment() = 0;
  // virtual void decrement() = 0;
  virtual bool isDifferent(AbstractIterator &rhs) = 0;
  // virtual bool isEqual(const AbstractIterator &rhs) = 0;
  // virtual bool isLess(const AbstractIterator &rhs) = 0;
  virtual std::unique_ptr<AbstractIterator> clone() = 0;
  virtual ~AbstractIterator() = default;
};

class BaseIterator {
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = const ::valueType;
  using difference_type = std::ptrdiff_t;
  using pointer = const ::valueType *;
  using reference = const ::valueType &;

  std::unique_ptr<AbstractIterator> it;
  BaseIterator(const BaseIterator &iterator) {
    if (iterator.it)
      it = iterator.it->clone();
    else
      it = nullptr;
  };
  BaseIterator(std::unique_ptr<AbstractIterator> iter) {
    this->it = std::move(iter);
  }
  BaseIterator() {};
  const valueType operator*() const { return it->dereference(); }
  BaseIterator &operator++() {
    it->increment();
    return *this;
  }

  /*BaseIterator operator++(){
          it->increment();
          return *this;
  }*/
  friend bool operator!=(const BaseIterator &lhs, const BaseIterator &rhs) {
    if (!lhs.it || !rhs.it)
      return bool(lhs.it) != bool(rhs.it);
    return lhs.it->isDifferent(*rhs.it);
  }

  friend bool operator==(const BaseIterator &lhs, const BaseIterator &rhs) {
    return !(lhs != rhs);
  }

  BaseIterator &operator=(const BaseIterator &rhs) {
    if (rhs.it) {
      it = rhs.it->clone();
    } else {
      it = nullptr;
    }
    return *this;
  }

  // bool operator<(BaseIterator &rhs){
  //	return it->isLess(rhs->it);
  // }
  //	return it->isEqual(rhs->it);
  // }
};

class WitnessSet { // data structure to store 'std::shared_ptr<Witness>'
public:
  virtual ~WitnessSet() = default;
  std::shared_ptr<WitnessSet> relabel(std::map<unsigned, unsigned> map) {
    std::shared_ptr<WitnessSet> witnessSet = this->createEmptyWitnessSet();
    for (auto witness : *this) {
      witnessSet->insert(witness->relabel(map));
    }
    return witnessSet;
  }
  /*
  virtual BaseIterator begin() const {
    std::cout << "Error: WitnessSet begin()." << std::endl;
    exit(20);
  };
  */
  virtual BaseIterator begin() const = 0;
  virtual BaseIterator end() const = 0;
  virtual void insert(std::shared_ptr<Witness> w) = 0;
  virtual void union_set_witness(std::shared_ptr<WitnessSet> witnessSet) = 0;
  virtual void print() = 0;
  virtual std::string witnessSetInformation() = 0;
  virtual void hash(Hasher &h) const = 0;
  friend bool operator==(WitnessSet &lhs, WitnessSet &rhs) {
    return lhs.isEqual(rhs);
  };
  virtual bool isEqual(WitnessSet &rhs) = 0;
  friend bool operator!=(WitnessSet &lhs, WitnessSet &rhs) {
    return !lhs.isEqual(rhs);
  };
  friend bool operator<(WitnessSet &lhs, WitnessSet &rhs) {
    return lhs.isLess(rhs);
  };

  virtual bool isLess(WitnessSet &rhs) = 0;
  virtual int size() = 0;
  virtual std::shared_ptr<WitnessSet> createEmptyWitnessSet() = 0;
  virtual void setEqual(const WitnessSet &other) = 0;
};

using WitnessSetPointer = std::shared_ptr<WitnessSet>;
template <class T> class WitnessSetTypeOne : public WitnessSet {
public:
  static std::map<std::shared_ptr<Witness>, int, Witness::IsLessSharedPtr>
      allWitnesses;
  static std::vector<std::shared_ptr<Witness>> witnessVector;
#ifdef TREEWIDZARD_SINGLE_THREADED
  struct NoOpMutex {
    void lock() {}
    void unlock() {}
    bool try_lock() { return true; }
  };
  using MutexType = NoOpMutex;
#else
  using MutexType = std::mutex;
#endif
  static MutexType witnessMutex;
  mutable MutexType setMutex;
  std::vector<uint8_t> mask;
  int cardinality = 0;
  mutable bool hashCacheValid = false;
  mutable uint64_t hashCache = 0;
  WitnessSetTypeOne() = default;
  WitnessSetTypeOne(const WitnessSetTypeOne &other) {
    std::lock_guard<MutexType> lock(other.setMutex);
    mask = other.mask;
    cardinality = other.cardinality;
  }
  WitnessSetTypeOne &operator=(const WitnessSetTypeOne &other) {
    if (this == &other) {
      return *this;
    }
    std::scoped_lock lock(setMutex, other.setMutex);
    mask = other.mask;
    cardinality = other.cardinality;
    invalidateHashCache();
    return *this;
  }
  WitnessSetTypeOne(WitnessSetTypeOne &&other) noexcept {
    std::lock_guard<MutexType> lock(other.setMutex);
    mask = std::move(other.mask);
    cardinality = other.cardinality;
    other.cardinality = 0;
    other.invalidateHashCache();
  }
  WitnessSetTypeOne &operator=(WitnessSetTypeOne &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    std::scoped_lock lock(setMutex, other.setMutex);
    mask = std::move(other.mask);
    cardinality = other.cardinality;
    invalidateHashCache();
    other.cardinality = 0;
    other.invalidateHashCache();
    return *this;
  }
  class WitnessSetTypeOneIterator : public AbstractIterator {
  private:
    const WitnessSetTypeOne<T> *self;
    int idx;

  public:
    WitnessSetTypeOneIterator(const WitnessSetTypeOne<T> *self_, int idx_)
        : self(self_), idx(idx_) {}
    virtual const valueType dereference() override {
      return (witnessVector[idx]);
    };
    virtual void increment() override {
      const int limit = static_cast<int>(8 * self->mask.size());
      while (++idx < limit) {
        if (self->mask[idx / 8] >> (idx % 8) & 1)
          break;
      }
    };
    virtual void increment(int) {
      const int limit = static_cast<int>(8 * self->mask.size());
      while (++idx < limit) {
        if (self->mask[idx / 8] >> (idx % 8) & 1)
          break;
      }
    };

    virtual bool isDifferent(AbstractIterator &rhs) override {
      if (WitnessSetTypeOneIterator *e =
              dynamic_cast<WitnessSetTypeOneIterator *>(&rhs)) {
        return idx != e->idx;
      }
      // TODO: Handle casting error more robustly, e.g., throw an exception or
      // return false For now, returning false as a default for incompatible
      // types
      return false;
    };
    virtual std::unique_ptr<AbstractIterator> clone() override {
      return std::make_unique<WitnessSetTypeOneIterator>(*this);
    }
  };
  BaseIterator begin() const override {
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeOneIterator(this, -1)));
    ++baseIterator;
    return baseIterator;
  }
  BaseIterator end() const override {
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeOneIterator(this, static_cast<int>(8 * mask.size()))));
    return baseIterator;
  }

  // Direct access: returns indices of set bits for direct iteration
  std::vector<int> direct_indices() const {
    std::lock_guard<MutexType> lock(setMutex);
    std::vector<int> indices;
    indices.reserve(cardinality);
    const int limit = static_cast<int>(8 * mask.size());
    for (int idx = 0; idx < limit; ++idx) {
      if (mask[idx / 8] >> (idx % 8) & 1)
        indices.push_back(idx);
    }
    return indices;
  }
  static const std::vector<std::shared_ptr<Witness>>& direct_witness_vector() {
    return witnessVector;
  }

  virtual void insert(std::shared_ptr<Witness> w) override;
  virtual void
  union_set_witness(std::shared_ptr<WitnessSet> witnessSet) override;
  virtual void print() override;
  std::string witnessSetInformation() override;
  void hash(Hasher &h) const override;
  virtual bool isLess(WitnessSet &rhs) override;
  virtual bool isEqual(WitnessSet &rhs) override;
  virtual int size() override;
  virtual std::shared_ptr<WitnessSet> createEmptyWitnessSet() override {
    return std::make_shared<WitnessSetTypeOne<T>>();
  }
  int witnessVectorSize() { return witnessVector.size(); }
  void setEqual(const WitnessSet &other) override;

private:
  void invalidateHashCache() {
    hashCacheValid = false;
    hashCache = 0;
  }
};

template <class T> class WitnessSetTypeTwo : public WitnessSet {
private:
  struct compare {
    bool operator()(const std::shared_ptr<Witness> &lhs,
                    const std::shared_ptr<Witness> &rhs) const {
      return *lhs < *rhs;
    }
  };
  struct hash {
    auto operator()(const std::shared_ptr<Witness> &ptr) const
        -> std::uint64_t {
      auto h = Hasher(0);
      ptr->hash(h);
      return h.get();
    }
  };

  using InnerContainer = std::vector<std::shared_ptr<Witness>>;
  InnerContainer container;
  mutable bool hashCacheValid = false;
  mutable uint64_t hashCache = 0;

  void flush_pending() const {
    if (pending.empty()) return;
    // Sort pending by witness order
    std::sort(pending.begin(), pending.end(), compare{});
    // Remove duplicates within pending
    auto last = std::unique(pending.begin(), pending.end(),
        [](const auto &a, const auto &b) { return !compare{}(a, b) && !compare{}(b, a); });
    pending.erase(last, pending.end());
    // Merge pending into container
    InnerContainer merged;
    merged.reserve(container.size() + pending.size());
    std::merge(container.begin(), container.end(),
               pending.begin(), pending.end(),
               std::back_inserter(merged), compare{});
    // Remove duplicates from merge result
    auto mlast = std::unique(merged.begin(), merged.end(),
        [](const auto &a, const auto &b) { return !compare{}(a, b) && !compare{}(b, a); });
    merged.erase(mlast, merged.end());
    const_cast<InnerContainer&>(container) = std::move(merged);
    pending.clear();
  }

  mutable InnerContainer pending;  // Unsorted buffer for batch insertion

  class WitnessSetTypeTwoIterator : public AbstractIterator {
  private:
    typename InnerContainer::const_iterator it;

  public:
    WitnessSetTypeTwoIterator(typename InnerContainer::const_iterator it_)
        : it(it_) {}
    const valueType dereference() override { return *it; };
    void increment() override { ++it; };
    bool isDifferent(AbstractIterator &rhs) override {
      if (WitnessSetTypeTwoIterator *e =
              dynamic_cast<WitnessSetTypeTwoIterator *>(&rhs)) {
        return it != e->it;
      } else {
        std::cerr << "WitnessSetTypeTwoIterator::isDifferent: Casting "
                     "error\n";
        exit(20);
      }
    };
    std::unique_ptr<AbstractIterator> clone() override {
      return std::make_unique<WitnessSetTypeTwoIterator>(*this);
    };
  };

public:
  BaseIterator begin() const override {
    flush_pending();
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeTwoIterator(container.begin())));
    return baseIterator;
  }
  BaseIterator end() const override {
    flush_pending();
    BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
        new WitnessSetTypeTwoIterator(container.end())));
    return baseIterator;
  }

  // Direct access methods bypassing virtual BaseIterator
  const InnerContainer& direct_container() const {
    flush_pending();
    return container;
  }

  void insert(std::shared_ptr<Witness> w) override;
  void union_set_witness(std::shared_ptr<WitnessSet> witnessSet) override;
  void print() override;
  std::string witnessSetInformation() override;
  bool isLess(WitnessSet &rhs) override;
  bool isEqual(WitnessSet &rhs) override;
  int size() override;
  void hash(Hasher &h) const override;
  virtual std::shared_ptr<WitnessSet> createEmptyWitnessSet() override {
    return std::make_shared<WitnessSetTypeTwo<T>>();
  }
  void setEqual(const WitnessSet &other) override;

private:
  static bool witnessesEqual(const std::shared_ptr<Witness> &lhs,
                             const std::shared_ptr<Witness> &rhs) {
    return !compare{}(lhs, rhs) && !compare{}(rhs, lhs);
  }

  void invalidateHashCache() {
    hashCacheValid = false;
    hashCache = 0;
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
#ifdef TREEWIDZARD_SINGLE_THREADED
template <class T>
typename WitnessSetTypeOne<T>::NoOpMutex WitnessSetTypeOne<T>::witnessMutex;
#else
template <class T>
std::mutex WitnessSetTypeOne<T>::witnessMutex;
#endif

template <class T>
void WitnessSetTypeOne<T>::insert(std::shared_ptr<Witness> ws) {
  int idx = -1;
  {
    std::lock_guard<MutexType> lock(witnessMutex);
    auto it = allWitnesses.find(ws);
    if (it != allWitnesses.end()) {
      idx = it->second;
    } else {
      // new witness
      idx = allWitnesses.size();
      allWitnesses[ws] = idx;
      witnessVector.push_back(ws);
    }
  }

  {
    std::lock_guard<MutexType> lock(setMutex);
    if (idx >= static_cast<int>(8 * mask.size())) {
      const std::size_t needed = static_cast<std::size_t>(idx / 8) + 1;
      mask.resize(std::max(needed, mask.size() * 2), 0);
    }

    const std::size_t byte_index = static_cast<std::size_t>(idx / 8);
    const uint8_t bit = static_cast<uint8_t>(1u << (idx % 8));
    if ((mask[byte_index] & bit) == 0) {
      mask[byte_index] |= bit;
      ++cardinality;
      invalidateHashCache();
    }
  }
}
template <class T>
void WitnessSetTypeOne<T>::union_set_witness(
    std::shared_ptr<WitnessSet> witnessSet) {
  if (WitnessSetTypeOne<T> *e =
          dynamic_cast<WitnessSetTypeOne<T> *>(&*witnessSet)) {
    if (e == this) {
      return;
    }
    std::scoped_lock lock(setMutex, e->setMutex);
    bool changed = false;
    for (std::size_t i = 0; i < e->mask.size(); ++i) {
      if (i == mask.size())
        mask.push_back(0);

      const uint8_t old_byte = mask[i];
      const uint8_t merged_byte = static_cast<uint8_t>(old_byte | e->mask[i]);
      if (merged_byte != old_byte) {
        cardinality += static_cast<int>(
            std::popcount(static_cast<unsigned>(merged_byte)) -
            std::popcount(static_cast<unsigned>(old_byte)));
        mask[i] = merged_byte;
        changed = true;
      }
    }
    if (changed) {
      invalidateHashCache();
    }
  }
}
template <class T> void WitnessSetTypeOne<T>::print() {
  for (auto element : *this)
    element->print();
}

template <class T> std::string WitnessSetTypeOne<T>::witnessSetInformation() {
  std::string info;
  for (auto element : *this)
    info = info + element->witnessInformation();
  return info;
}

template <class T> void WitnessSetTypeOne<T>::hash(Hasher &h) const {
  std::lock_guard<MutexType> lock(setMutex);
  if (!hashCacheValid) {
    Hasher inner(0);
    inner << static_cast<unsigned>(mask.size());
    for (const auto byte : mask) {
      inner << static_cast<unsigned>(byte);
    }
    hashCache = inner.get();
    hashCacheValid = true;
  }
  h << hashCache;
}

template <class T> bool WitnessSetTypeOne<T>::isLess(WitnessSet &rhs) {
  if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&rhs)) {
    if (e == this) {
      return false;
    }
    std::scoped_lock lock(setMutex, e->setMutex);
    if (cardinality < e->cardinality) {
      return true;
    } else if (e->cardinality < cardinality) {
      return false;
    } else {
      auto it = e->begin();
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
  }
  std::cerr << "Error: In isLess, WitnessSet type cast error." << std::endl;
  exit(20);
}

template <class T> bool WitnessSetTypeOne<T>::isEqual(WitnessSet &rhs) {
  if (WitnessSetTypeOne<T> *e = dynamic_cast<WitnessSetTypeOne<T> *>(&rhs)) {
    if (e == this) {
      return true;
    }
    std::scoped_lock lock(setMutex, e->setMutex);
    return mask == e->mask;
  }
  std::cerr << "Error: In isEqual, WitnessSet type cast error." << std::endl;
  exit(20);
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

template <class T> int WitnessSetTypeOne<T>::size() {
  std::lock_guard<MutexType> lock(setMutex);
  return cardinality;
}

template <class T>
void WitnessSetTypeOne<T>::setEqual(const WitnessSet &other) {
  if (const WitnessSetTypeOne<T> *e =
          dynamic_cast<const WitnessSetTypeOne<T> *>(&other)) {
    *this = *e;
  } else {
    std::cerr << "Error: WitnessSetTypeOne::setEqual called with wrong type\n";
    exit(20);
  }
}

/////////////WitnessSet TYPE Two////////////////
template <class T>
void WitnessSetTypeTwo<T>::insert(std::shared_ptr<Witness> ws) {
  // Add to pending buffer instead of doing O(n) shift immediately
  pending.push_back(std::move(ws));
  invalidateHashCache();
}
template <class T>
void WitnessSetTypeTwo<T>::union_set_witness(
    std::shared_ptr<WitnessSet> witnessSet) {
  if (&*witnessSet == this) {
    return;
  }

  if (auto *typed_rhs = dynamic_cast<WitnessSetTypeTwo<T> *>(&*witnessSet)) {
    flush_pending();
    typed_rhs->flush_pending();
    InnerContainer merged;
    merged.reserve(container.size() + typed_rhs->container.size());

    auto lhs_it = container.begin();
    auto rhs_it = typed_rhs->container.begin();
    while (lhs_it != container.end() || rhs_it != typed_rhs->container.end()) {
      if (rhs_it == typed_rhs->container.end() ||
          (lhs_it != container.end() && compare{}(*lhs_it, *rhs_it))) {
        merged.push_back(*lhs_it);
        ++lhs_it;
      } else if (lhs_it == container.end() || compare{}(*rhs_it, *lhs_it)) {
        merged.push_back(*rhs_it);
        ++rhs_it;
      } else {
        merged.push_back(*lhs_it);
        ++lhs_it;
        ++rhs_it;
      }
    }

    container = std::move(merged);
    invalidateHashCache();
    return;
  }

  for (const auto &element : *witnessSet) {
    insert(element);
  }
}
template <class T> void WitnessSetTypeTwo<T>::print() {
  for (auto element : *this)
    element->print();
}
template <class T> std::string WitnessSetTypeTwo<T>::witnessSetInformation() {
  std::string info;
  for (auto element : *this)
    info = info + element->witnessInformation();
  return info;
}
template <class T> void WitnessSetTypeTwo<T>::hash(Hasher &h) const {
  flush_pending();
  if (!hashCacheValid) {
    Hasher inner(0);
    for (const auto &element : container) {
      element->hash(inner);
      inner << -2u;
    }
    hashCache = inner.get();
    hashCacheValid = true;
  }
  h << hashCache;
}

template <class T> bool WitnessSetTypeTwo<T>::isLess(WitnessSet &rhs) {
  flush_pending();
  if (WitnessSetTypeTwo<T> *e = dynamic_cast<WitnessSetTypeTwo<T> *>(&rhs)) {
    e->flush_pending();
    if (container.size() < e->container.size()) {
      return true;
    } else if (e->container.size() < container.size()) {
      return false;
    } else {
      auto it = e->container.begin();
      for (const auto &element : container) {
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

template <class T> bool WitnessSetTypeTwo<T>::isEqual(WitnessSet &rhs) {
  flush_pending();
  if (WitnessSetTypeTwo *e = dynamic_cast<WitnessSetTypeTwo *>(&rhs)) {
    e->flush_pending();
    if (container.size() != e->container.size()) {
      return false;
    }

    for (std::size_t i = 0; i < container.size(); ++i) {
      if (!witnessesEqual(container[i], e->container[i])) {
        return false;
      }
    }
    return true;
  } else {
    std::cout << "Error: In isEqual, WitnessSet type cast error." << std::endl;
    exit(20);
  }
}

template <class T> int WitnessSetTypeTwo<T>::size() {
  flush_pending();
  return static_cast<int>(container.size());
}

template <class T>
void WitnessSetTypeTwo<T>::setEqual(const WitnessSet &other) {
  if (const WitnessSetTypeTwo<T> *e =
          dynamic_cast<const WitnessSetTypeTwo<T> *>(&other)) {
    *this = *e;
  } else {
    std::cerr << "Error: WitnessSetTypeTwo::setEqual called with wrong type\n";
    exit(20);
  }
}

#endif
