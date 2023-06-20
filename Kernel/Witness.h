#ifndef WITNESS_H
#define WITNESS_H
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include "Hasher.hpp"

class Witness {
  protected:
	// Making the methods pure virtual,
	// meaning one can not construct a Witness
	// or a descendant of Witness without overriding
	// these methods
	virtual bool is_equal(const Witness &rhs) const = 0;
	virtual bool is_less(const Witness &rhs) const = 0;
	virtual Witness &set_equal(const Witness &witness) = 0;

  public:
	virtual std::shared_ptr<Witness> relabel(
		const std::map<unsigned, unsigned> &relabelingMap) const;
	virtual void print() const;
	virtual std::string witnessInformation() const;
	virtual ~Witness();
	friend bool operator==(const Witness &lhs, const Witness &rhs);
	friend bool operator<(const Witness &lhs, const Witness &rhs);
	bool operator!=(const Witness &rhs) const;
	virtual void hash(Hasher &h) const;
	struct IsLessSharedPtr {
		bool operator()(const std::shared_ptr<const Witness> &lhs,
						const std::shared_ptr<const Witness> &rhs) const {
			return *lhs < *rhs;
		}
	};
};

typedef std::shared_ptr<Witness> WitnessPointer;
typedef std::shared_ptr<Witness const> WitnessPointerConst;

#endif
