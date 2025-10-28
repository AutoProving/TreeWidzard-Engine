#include "State.h"

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

Bag State::get_bag() const { return State::bag; }

void State::set_bag(const Bag &bag) { State::bag = bag; }

void State::setWitnessSetVector(
	const std::vector<std::shared_ptr<WitnessSet>> &witnessSetVector) {
	State::witnessSetVector = witnessSetVector;
}

void State::addWitnessSet(std::shared_ptr<WitnessSet> ws) {
	witnessSetVector.push_back(ws);
}

bool State::operator==(const State &rhs) const {
	if (this->get_bag() == rhs.get_bag()) {
		if (numberOfComponents() != rhs.numberOfComponents()) {
			std::cout << "ERROR: In State::operator== sizes are different! "
					  << std::endl;
			exit(20);
		} else {
			for (size_t j = 0; j < numberOfComponents(); j++) {
				if (!(*(rhs.getWitnessSet(j)) == *(getWitnessSet(j)))) {
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
	this->get_bag().hash(h);
	for (unsigned j = 0; j < numberOfComponents(); j++) {
		h << -3u;
		getWitnessSet(j)->hash(h);
	}
}

bool State::operator!=(const State &rhs) const { return !(rhs == *this); }

bool State::operator<(const State &rhs) const {
	if (this->get_bag() < rhs.get_bag()) {
		return true;
	} else if (numberOfComponents() != rhs.numberOfComponents()) {
		std::cout << "ERROR: In State::operator< sizes are different! "
				  << std::endl;
		exit(20);
	} else if (this->get_bag() == rhs.get_bag()) {
		for (unsigned j = 0; j < numberOfComponents(); j++) {
			std::shared_ptr<WitnessSet> temp1 = getWitnessSet(j);
			std::shared_ptr<WitnessSet> temp2 = rhs.getWitnessSet(j);
			if (*temp1 < *temp2) {
				return true;
			} else if (*temp2 < *temp1) {
				return false;
			}
		}
		return false;
	} else {
		return false;
	}
}

bool State::operator>(const State &rhs) const { return rhs < *this; }

bool State::operator<=(const State &rhs) const { return !(rhs < *this); }

bool State::operator>=(const State &rhs) const { return !(*this < rhs); }
/// Implementation of this function should change regard to WitnessSet and
/// Witness
size_t State::operator()(const State &b) const {
	std::string s;
	std::set<unsigned> bagSet = b.get_bag().get_elements();
	for (auto element : bagSet) {
		s = s + (char)element;
	}
	s = s + 'a';
	if (b.get_bag().get_edge().first != 0 and
		b.get_bag().get_edge().second != 0) {
		s = s + (char)b.get_bag().get_edge().first +
			(char)b.get_bag().get_edge().second + 'a';
	}
	std::size_t str_hash = std::hash<std::string>{}(s);
	return str_hash;
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

std::shared_ptr<WitnessSet> State::getWitnessSet(int i) const {
	return witnessSetVector[i];
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
	state->setWitnessSetVector(witnessSetVec);
	Bag b = bag.relabel(relabelingMap);
	state->set_bag(b);
	return state;
}
