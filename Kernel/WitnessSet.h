#ifndef WITNESSSET_H
#define WITNESSSET_H

#include <map>
#include <memory>
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
	BaseIterator(std::unique_ptr<AbstractIterator> it) {
		this->it = std::move(it);
	}
	BaseIterator(){};
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
		if (!lhs.it || !rhs.it) return bool(lhs.it) != bool(rhs.it);
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
		virtual void increment(int) {
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
	BaseIterator begin() const override {
		BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
			new WitnessSetTypeOneIterator(this, -1)));
		++baseIterator;
		return baseIterator;
	}
	BaseIterator end() const override {
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
	void setEqual(const WitnessSet &other) override;
};

template <class T>
class WitnessSetTypeTwo : public WitnessSet {
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

	using InnerContainer = std::set<std::shared_ptr<Witness>, compare>;
	// using InnerContainer =
	//      std::unordered_set<std::shared_ptr<Witness>, hash>;
	InnerContainer container;

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
		BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
			new WitnessSetTypeTwoIterator(container.begin())));
		return baseIterator;
	}
	BaseIterator end() const override {
		BaseIterator baseIterator(std::unique_ptr<AbstractIterator>(
			new WitnessSetTypeTwoIterator(container.end())));
		return baseIterator;
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
	for (int i = 0; i < 8 * mask.size(); i++)
		ret += (mask[i / 8] >> (i % 8) & 1);
	return ret;
}

template <class T>
void WitnessSetTypeOne<T>::setEqual(const WitnessSet &other) {
	if (const WitnessSetTypeOne<T> *e =
			dynamic_cast<const WitnessSetTypeOne<T> *>(&other)) {
		*this = *e;
	} else {
		std::cerr
			<< "Error: WitnessSetTypeOne::setEqual called with wrong type\n";
		exit(20);
	}
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
		std::cout << "Error: In isLess, WitnessSet type cast error."
				  << std::endl;
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
		std::cout << "Error: In isEqual, WitnessSet type cast error."
				  << std::endl;
		exit(20);
	}
}

template <class T>
int WitnessSetTypeTwo<T>::size() {
	return container.size();
}

template <class T>
void WitnessSetTypeTwo<T>::setEqual(const WitnessSet &other) {
	if (const WitnessSetTypeTwo<T> *e =
			dynamic_cast<const WitnessSetTypeTwo<T> *>(&other)) {
		*this = *e;
	} else {
		std::cerr
			<< "Error: WitnessSetTypeTwo::setEqual called with wrong type\n";
		exit(20);
	}
}

#endif
