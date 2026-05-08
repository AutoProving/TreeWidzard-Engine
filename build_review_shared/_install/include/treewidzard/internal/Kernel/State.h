#ifndef STATE_H
#define STATE_H
#include <cassert>
#include <vector>
#include <memory>
#include <deque>
#include <mutex>
#include "Bag.h"
#include "WitnessSet.h"

class State : private std::enable_shared_from_this<State> {
  private:
	Bag bag;
	std::vector<std::shared_ptr<WitnessSet>> witnessSetVector;
	
	// Simple memory pool for State objects
	struct StateMemoryPool {
		std::deque<std::unique_ptr<State>> available;
		std::mutex pool_mutex;
		size_t active_allocations = 0;
		
		State* allocate();
		void deallocate(State* ptr);
		void clear();
		size_t getActiveAllocations() const { return active_allocations; }
	};
	
	static StateMemoryPool pool_;
  public:
	// Pool management methods
	static void initializePool(size_t initialSize = 10000);
	static void clearPool();
	static size_t getPoolStats();
	
	// Custom allocation using memory pool
	static std::shared_ptr<State> createPooled();
	
	class ptr {
	  private:
		std::shared_ptr<const State> pointer;
	  public:
		ptr() { 
			auto mutable_state = State::createPooled();
			pointer = mutable_state;
		}
		ptr(std::shared_ptr<const State> pointer_) : pointer(pointer_) {}
		ptr(std::shared_ptr<State> pointer_) : pointer(pointer_) {}
		
		// Allow access to mutable state during construction
		std::shared_ptr<State> getMutableState() const {
			return std::const_pointer_cast<State>(pointer);
		}
		
		const State &operator*() const { return *pointer; }
		const State *operator->() const { return &*pointer; }
		bool operator<(const ptr &rhs) const { return **this < *rhs; }
		bool operator>(const ptr &rhs) const { return rhs < *this; }
		bool operator<=(const ptr &rhs) const { return !(rhs < *this); }
		bool operator>=(const ptr &rhs) const { return !(*this < rhs); }
		bool operator==(const ptr &rhs) const { return **this == *rhs; }
		bool operator!=(const ptr &rhs) const { return !(rhs == *this); }
		void print() const { pointer->print(); }
		std::string stateInformation() const {
			return pointer->stateInformation();
		}

		struct Hash {
			const uint64_t seed_ = 0;
			uint64_t operator()(const State::ptr &ptr) const {
				Hasher h(seed_);
				ptr->hash(h);
				return h.get();
			}
		};
		
		struct Equal {
			bool operator()(const State::ptr &l, const State::ptr &r) const {
				return *l == *r;
			}
		};
	};

	ptr get_ptr() const { return ptr(this->shared_from_this()); }

	Bag get_bag() const;

	void set_bag(const Bag &bag);

	void setWitnessSetVector(
		const std::vector<std::shared_ptr<WitnessSet>> &witnessSetVector);

	void addWitnessSet(std::shared_ptr<WitnessSet>);

	bool operator==(const State &rhs) const;

	bool operator!=(const State &rhs) const;

	bool operator<(const State &rhs) const;

	bool operator>(const State &rhs) const;

	bool operator<=(const State &rhs) const;

	bool operator>=(const State &rhs) const;

	size_t operator()(const State &b) const;

	void print() const;

	std::string stateInformation() const;

	std::shared_ptr<WitnessSet> getWitnessSet(int) const;

	size_t numberOfComponents() const;

	std::shared_ptr<const State> relabel(
		std::map<unsigned, unsigned> relabelingMap)
		const; // relabelingMap is a relabeling of the vertices in a bag. The
			   // relabel function propagates this relabeling to a State

	void hash(Hasher &h) const;
};

#endif
