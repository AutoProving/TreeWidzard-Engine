#include "State.h"

#include <utility>

// Static memory pool instance
State::StateMemoryPool State::pool_;

State* State::StateMemoryPool::allocate() {
	std::lock_guard<std::mutex> lock(pool_mutex);
	
	if (!available.empty()) {
		auto state = std::move(available.back());
		available.pop_back();
		active_allocations++;
		return state.release();
	}
	
	// Create new state if pool is empty
	active_allocations++;
	return new State();
}

void State::StateMemoryPool::deallocate(State* ptr) {
	std::lock_guard<std::mutex> lock(pool_mutex);
	
	if (ptr) {
		// Reset state to clean condition
		ptr->witnessSetVector.clear();
		ptr->bag = Bag();
		ptr->hashCacheValid = false;
		ptr->hashCache = 0;
		
		available.emplace_back(ptr);
		active_allocations--;
	}
}

void State::StateMemoryPool::clear() {
	std::lock_guard<std::mutex> lock(pool_mutex);
	available.clear();
	active_allocations = 0;
}

void State::initializePool(size_t initialSize) {
	std::lock_guard<std::mutex> lock(pool_.pool_mutex);
	
	// Pre-allocate states for the pool
	for (size_t i = 0; i < initialSize; ++i) {
		pool_.available.emplace_back(std::make_unique<State>());
	}
}

void State::clearPool() {
	pool_.clear();
}

size_t State::getPoolStats() {
	return pool_.getActiveAllocations();
}

std::shared_ptr<State> State::createPooled() {
	// Use pool allocator with custom deleter that returns to pool
	return std::shared_ptr<State>(
		pool_.allocate(),
		[](State* ptr) { pool_.deallocate(ptr); }
	);
}

const Bag &State::get_bag() const { return State::bag; }

Bag State::get_bag_copy() const { return State::bag; }

void State::set_bag(const Bag &new_bag) {
	bag = new_bag;
	hashCache = computeHashValue();
	hashCacheValid = true;
}

void State::setWitnessSetVector(
	const std::vector<std::shared_ptr<WitnessSet>> &newWitnessSetVector) {
	witnessSetVector = newWitnessSetVector;
	invalidateHashCache();
}

void State::setWitnessSetVector(
	std::vector<std::shared_ptr<WitnessSet>> &&newWitnessSetVector) {
	witnessSetVector = std::move(newWitnessSetVector);
	invalidateHashCache();
}

void State::addWitnessSet(std::shared_ptr<WitnessSet> ws) {
	witnessSetVector.push_back(std::move(ws));
	invalidateHashCache();
}

void State::invalidateHashCache() {
	hashCacheValid = false;
	hashCache = 0;
}

uint64_t State::computeHashValue() const {
	Hasher h(0);
	bag.hash(h);
	for (const auto &witnessSet : witnessSetVector) {
		h << -3u;
		witnessSet->hash(h);
	}
	return h.get();
}

bool State::operator==(const State &rhs) const {
	const Bag &lhs_bag = bag;
	const Bag &rhs_bag = rhs.bag;
	if (lhs_bag == rhs_bag) {
		const auto lhs_components = witnessSetVector.size();
		const auto rhs_components = rhs.witnessSetVector.size();
		if (lhs_components != rhs_components) {
			std::cout << "ERROR: In State::operator== sizes are different! "
					  << std::endl;
			exit(20);
		} else {
			for (size_t j = 0; j < lhs_components; j++) {
				if (!(*rhs.witnessSetVector[j] == *witnessSetVector[j])) {
					return false;
				}
			}
			return true;
		}
	} else {
		return false;
	}
}

void State::hash(Hasher &h) const {
	h << hashValue();
}

uint64_t State::hashValue() const {
	if (!hashCacheValid) {
		hashCache = computeHashValue();
		hashCacheValid = true;
	}
	return hashCache;
}

bool State::operator!=(const State &rhs) const { return !(rhs == *this); }

bool State::operator<(const State &rhs) const {
	const Bag &lhs_bag = bag;
	const Bag &rhs_bag = rhs.bag;
	if (lhs_bag < rhs_bag) {
		return true;
	}
	if (rhs_bag < lhs_bag) {
		return false;
	}
	// Bags are equal — compare witness sets lexicographically
	if (witnessSetVector.size() != rhs.witnessSetVector.size()) {
		std::cout << "ERROR: In State::operator< sizes are different! "
				  << std::endl;
		exit(20);
	}
	for (size_t j = 0; j < witnessSetVector.size(); j++) {
		const std::shared_ptr<WitnessSet> &temp1 = witnessSetVector[j];
		const std::shared_ptr<WitnessSet> &temp2 = rhs.witnessSetVector[j];
		if (*temp1 < *temp2) {
			return true;
		} else if (*temp2 < *temp1) {
			return false;
		}
	}
	return false;
}

bool State::operator>(const State &rhs) const { return rhs < *this; }

bool State::operator<=(const State &rhs) const { return !(rhs < *this); }

bool State::operator>=(const State &rhs) const { return !(*this < rhs); }
/// Hash functor for State objects used in unordered containers.
size_t State::operator()(const State &b) const {
	return b.hashValue();
}

void State::print() const {
	bag.print();
	int i = 1;
	for (auto element : witnessSetVector) {
		std::cout << "\nCore " << i << " WitnessSet \n";
		element->print();
		i++;
	}
	std::cout << "\n------------\n";
}

std::string State::stateInformation() const {
	std::string info;
	info = bag.bagInformation();
	int i = 1;
	for (auto element : witnessSetVector) {
		info = info + "\nCore " + std::to_string(i) + " WitnessSet \n";
		info = info + element->witnessSetInformation();
		i++;
	}
	info = info + "\n------------\n";
	return info;
}

const std::shared_ptr<WitnessSet> &State::getWitnessSet(int i) const {
	return witnessSetVector[static_cast<size_t>(i)];
}

std::shared_ptr<WitnessSet> State::getWitnessSetCopy(int i) const {
	return witnessSetVector[static_cast<size_t>(i)];
}

size_t State::numberOfComponents() const { return witnessSetVector.size(); }

std::shared_ptr<const State> State::relabel(
	std::map<unsigned int, unsigned int> relabelingMap) const {
	std::shared_ptr<State> state = State::createPooled();
	std::vector<std::shared_ptr<WitnessSet>> witnessSetVec;
	witnessSetVec.resize(witnessSetVector.size());
	for (size_t i = 0; i < witnessSetVector.size(); i++) {
		witnessSetVec[i] = witnessSetVector[i]->relabel(relabelingMap);
	}
	state->setWitnessSetVector(std::move(witnessSetVec));
	Bag b = bag.relabel(relabelingMap);
	state->set_bag(b);
	return state;
}
