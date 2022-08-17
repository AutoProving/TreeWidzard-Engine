// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "State.h"

Bag State::get_bag() const { return State::bag; }

void State::set_bag(const Bag& bag) { State::bag = bag; }

void State::setWitnessSetVector(const vector<shared_ptr<WitnessSet>> &witnessSetVector) {
    State::witnessSetVector = witnessSetVector;
}

void State::addWitnessSet(shared_ptr<WitnessSet> ws) {
	witnessSetVector.push_back(ws);
}

bool State::operator==(const State &rhs) const {
    if (this->get_bag() == rhs.get_bag()) {
        if (numberOfComponents() != rhs.numberOfComponents()) {
            cout << "ERROR: In State::operator== sizes are different! " << endl;
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

bool State::operator!=(const State &rhs) const {
    return !(rhs == *this);
}

bool State::operator<(const State &rhs) const {
    if (this->get_bag() < rhs.get_bag()) {
        return true;
    } else if (numberOfComponents() != rhs.numberOfComponents()) {
        cout << "ERROR: In State::operator< sizes are different! " << endl;
        exit(20);
    } else if (this->get_bag() == rhs.get_bag()) {
        for (unsigned j = 0; j < numberOfComponents(); j++) {
            shared_ptr<WitnessSet> temp1 = getWitnessSet(j);
            shared_ptr<WitnessSet> temp2 = rhs.getWitnessSet(j);
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

bool State::operator >(const State &rhs) const {
    return rhs < *this;
}

bool State::operator<=(const State &rhs) const {
    return !(rhs < *this);
}

bool State::operator>=(const State &rhs) const {
    return !(*this < rhs);
}
/// Implementation of this function should change regard to WitnessSet and
/// Witness
size_t State::operator()(const State& b) const {
	string s;
	set<unsigned> bagSet = b.get_bag().get_elements();
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

void State::print() {
	bag.print();
	int i = 1;
	for (auto element : witnessSetVector) {
		cout << "\nCore " << i << " WitnessSet \n";
		element->print();
		i++;
	}
	cout << "\n------------\n";
}


string State::stateInformation() {
    string info;
    info = bag.bagInformation();
    int i = 1;
    for (auto element : witnessSetVector) {
        info  = info + "\nCore " + to_string(i) +" WitnessSet \n";
        info  = info + element->witnessSetInformation();
        i++;
    }
    info  = info + "\n------------\n";
    return info;
}

shared_ptr<WitnessSet> State::getWitnessSet(int i) const {
	return witnessSetVector[i];
}


int State::numberOfComponents() const { return witnessSetVector.size(); }

shared_ptr<State> State::relabel(map<unsigned int, unsigned int> relabelingMap) {
    shared_ptr<State> state(new State);
    vector<shared_ptr<WitnessSet> > witnessSetVec;
    witnessSetVec.resize(witnessSetVector.size());
    for(size_t i = 0; i < witnessSetVector.size(); i++){
        witnessSetVec[i] = witnessSetVector[i]->relabel(relabelingMap);
    }
    state->setWitnessSetVector(witnessSetVec);
    Bag b = bag.relabel(relabelingMap);
    state->set_bag(b);
    return state;
}






