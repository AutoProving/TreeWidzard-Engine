// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "Bag.h"

void Bag::set_edge(unsigned i, unsigned j) {
	if (i < j) {
		Bag::i = i;
		Bag::j = j;
	} else {
		Bag::i = j;
		Bag::j = i;
	}
}

pair<unsigned, unsigned> Bag::get_edge() const { return make_pair(i, j); }

std::set<unsigned> Bag::get_elements() const { return elements; }

void Bag::set_elements(const std::set<unsigned> &elements) {
	this->elements = elements;
}

void Bag::print() {
	std::cout << "[{";
	for (auto element : elements) {
		std::cout << element;
        if(element!=*(--elements.end()))
            cout<<",";
    }
	std::cout << "} , {" << i << ", " << j << "}]";
}

string Bag::bagInformation() {
    string info;
    info = "[{";
    for (auto element : elements) {
        info = info + to_string(element);
        if(element!=*(--elements.end()))
            info = info + ",";
    }
    info = info + "} , {" + to_string(i) +", " +to_string(j) + "}]";
    return info;
}

bool Bag::operator<(const Bag &rhs) const {
	if (elements < rhs.get_elements()) {
		return true;
	} else if (rhs.get_elements() < elements) {
		return false;
	} else if (this->get_edge() < rhs.get_edge()) {
		return true;
	} else {
		return false;
	}
}

bool Bag::operator==(const Bag &rhs) const {
	if (elements == rhs.get_elements() and this->get_edge() == rhs.get_edge()) {
		return true;
	} else {
		return false;
	}
}

bool Bag::vertex_introducible(unsigned i) const {
	if (elements.find(i) == elements.end()) {
		return true;
	} else {
		return false;
	}
}

bool Bag::edge_introducible(unsigned i, unsigned j) const {
	if (elements.find(i) != elements.end() and
		elements.find(j) != elements.end()) {
		return true;
	} else {
		return false;
	}
}

bool Bag::vertex_forgettable(unsigned i) const {
	if (elements.find(i) != elements.end()) {
		return true;
	} else {
		return false;
	}
}

bool Bag::joinable(Bag b) {
	if (this->elements == b.get_elements()) {
		return true;
	} else {
		return false;
	}
}

Bag Bag::intro_v(unsigned i) {
	elements.insert(i);
	return *this;
}

Bag Bag::intro_e(unsigned i, unsigned j) {
	if (i < j) {
		set_edge(i, j);
	} else {
		set_edge(j, i);
	}
	return *this;
}

Bag Bag::forget_v(unsigned i) {
	elements.erase(i);
	if (this->i == i or this->j == i) {
		this->i = 0;
		this->j = 0;
	}
	return *this;
}

Bag Bag::relabel(map<unsigned int, unsigned int> relabelingMap) {
    Bag b;
    set<unsigned > bagElements;
    for(auto v:this->elements){
        if(relabelingMap.count(v)) {
            bagElements.insert(relabelingMap[v]);
        }else{
            cout<<"Error: Bag::relabel "<< v <<" is not in the relabeling map"<<endl;
            exit(20);
        }
    }
    b.set_elements(bagElements);
    b.set_edge(relabelingMap[this->i],relabelingMap[this->j]);
    return b;
}


