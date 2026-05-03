// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "Bag.h"

#include <iterator>

size_t Bag::size() const { return elements.size(); }

void Bag::set_edge(unsigned i, unsigned j) {
	if (i < j) {
		edge_i = i;
		edge_j = j;
	} else {
		edge_i = j;
		edge_j = i;
	}
}

std::pair<unsigned, unsigned> Bag::get_edge() const {
	return std::make_pair(edge_i, edge_j);
}

const std::set<unsigned> &Bag::get_elements() const { return elements; }

std::set<unsigned> Bag::get_elements_copy() const { return elements; }

void Bag::set_elements(const std::set<unsigned> &new_elements) {
	elements = new_elements;
}

// void Bag::print() {
//	std::cout << "[{";
//	for (auto element : elements) {
//		std::cout << element;
//         if(element!=*(--elements.end()))
//             cout<<",";
//     }
//	std::cout << "} , {" << i << ", " << j << "}]";
// }

// void Bag::print() const {
//     std::cout << "[{";
//     for (auto element : elements) {
//         std::cout << element;
//         if(element!=*(--elements.end()))
//             cout<<",";
//     }
//     std::cout << "} , {" << i << ", " << j << "}]";
// }
// string Bag::bagInformation() {
//     string info;
//     info = "[{";
//     for (auto element : elements) {
//         info = info + to_string(element);
//         if(element!=*(--elements.end()))
//             info = info + ",";
//     }
//     info = info + "} , {" + to_string(i) +", " +to_string(j) + "}]";
//     return info;
// }

void Bag::print() {
	std::cout << "{";
	for (auto it = elements.begin(); it != elements.end(); ++it) {
		std::cout << *it;
		if (std::next(it) != elements.end()) std::cout << ",";
	}
	std::cout << "}";
}

void Bag::print() const {
	std::cout << "{";
	for (auto it = elements.begin(); it != elements.end(); ++it) {
		std::cout << *it;
		if (std::next(it) != elements.end()) std::cout << ",";
	}
	std::cout << "}";
}
std::string Bag::bagInformation() const {
	std::string info("{");
	for (auto it = elements.begin(); it != elements.end(); ++it) {
		info = info + std::to_string(*it);
		if (std::next(it) != elements.end()) info = info + ",";
	}
	info = info + "}";
	return info;
}

bool Bag::operator<(const Bag &rhs) const {
	return elements < rhs.elements;
}

bool Bag::operator==(const Bag &rhs) const {
	return elements == rhs.elements;
}

void Bag::hash(Hasher &h) const {
	for (unsigned elem : elements) h << elem;
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

bool Bag::joinable(const Bag &b) const {
	return elements == b.elements;
}

Bag Bag::intro_v(unsigned v) const {
	Bag result = *this;
	result.elements.insert(v);
	return result;
}

Bag Bag::intro_e(unsigned ei, unsigned ej) const {
	Bag result = *this;
	if (ei < ej) {
		result.set_edge(ei, ej);
	} else {
		result.set_edge(ej, ei);
	}
	return result;
}

Bag Bag::forget_v(unsigned v) const {
	Bag result = *this;
	result.elements.erase(v);
	if (result.edge_i == v or result.edge_j == v) {
		result.edge_i = 0;
		result.edge_j = 0;
	}
	return result;
}

Bag Bag::relabel(std::map<unsigned int, unsigned int> relabelingMap) const {
	Bag b;
	std::set<unsigned> bagElements;
	for (auto v : this->elements) {
		if (relabelingMap.count(v)) {
			bagElements.insert(relabelingMap[v]);
		} else {
			std::cout << "Error: Bag::relabel " << v
					  << " is not in the relabeling map" << std::endl;
			exit(20);
		}
	}
	b.set_elements(bagElements);
	if (this->edge_i != 0 && this->edge_j != 0) {
		if (relabelingMap.count(this->edge_i) && relabelingMap.count(this->edge_j)) {
			b.set_edge(relabelingMap[this->edge_i], relabelingMap[this->edge_j]);
		} else {
			std::cout << "Error: Bag::relabel edge vertices not in relabeling map"
					  << std::endl;
			exit(20);
		}
	}
	return b;
}
