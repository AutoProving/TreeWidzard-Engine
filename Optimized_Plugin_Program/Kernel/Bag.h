// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef BAG_TYPE_H
#define BAG_TYPE_H

#include <iostream>
#include <set>
#include <map>
#include <utility>
#include "Hasher.hpp"

class Bag {
  private:
	std::set<unsigned> elements;
	unsigned i = 0;
	unsigned j = 0;

  public:

	std::set<unsigned> get_elements() const;
	void set_elements(const std::set<unsigned>& elements);
	void set_edge(unsigned i, unsigned j);
	std::pair<unsigned, unsigned> get_edge() const;
	void print();
	void print() const;
	std::string bagInformation() const;
	bool operator<(const Bag& rhs) const;
	bool operator==(const Bag& rhs) const;
	void hash(Hasher &h) const;

	// Types of Bags
	bool vertex_introducible(unsigned i) const;
	bool edge_introducible(unsigned i, unsigned j) const;
	bool vertex_forgettable(unsigned i) const;
	bool joinable(Bag b);
	/*
			bool edge_forgettable(int i, int j) const;
	*/
	// Operations with Bags
	Bag intro_v(unsigned i);			 // add vertex
	Bag intro_e(unsigned i, unsigned j); // add edge
	Bag forget_v(unsigned i);			 // remove vertex
    Bag relabel(std::map<unsigned,unsigned> relabelingMap); // relabelingMap is a relabeling of the vertices in a bag. example: B = ({1,3,5},{1,5})
    // and relabelingmap = 1->1, 3->2, 5->3, then the relabled bag is B'=({1,2,3},{1,3})

};



#endif
