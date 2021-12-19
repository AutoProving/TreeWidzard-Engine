// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef MULTIGRAPH_H
#define MULTIGRAPH_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <experimental/filesystem>

using namespace std;
namespace multigraph_fs = std::experimental::filesystem;

class MultiGraph {
  private:
	set<unsigned> vertices;
	set<unsigned> edges;
	multimap<unsigned, unsigned> incidenceMap; // incidenceMap(e,v) indicates that v belongs to e
  public:
	MultiGraph();
	// copy constructor
	MultiGraph(const MultiGraph &multiGraph);
	// Order of the graph
	unsigned verticesSize();
	// Size of the graph
	unsigned edgesSize();
	// set functions

	void setVertices(const set<unsigned> &vertices);

	void setEdges(const set<unsigned> &edges);

	void setIncidenceMap(const multimap<unsigned, unsigned> &incidenceMap);

	// get functions
	const set<unsigned> &getVertices() const;

	const set<unsigned> &getEdges() const;

	const multimap<unsigned, unsigned> &getIncidenceMap() const;

	unsigned nthVertex(unsigned n); // get nth vertex's label in the vertex set

	// auxiliary functions

	void addVertex(unsigned vertex);

	void deleteVertex(unsigned vertex);

	unsigned addEdgeEndPoints(unsigned i, unsigned j); // Adds i and j as an edge and returns a label of the new edge

	set<unsigned> edgesBetweenVertices(unsigned i, unsigned j); // Returns label of the edges between i and j

	set<unsigned> edgesVertex(unsigned i); // Returns labels of the incident edges to i

	void deleteEdgeEndpoints(unsigned i, unsigned j);

	void addEdgeLabel(unsigned edge);

	void deleteEdgeLabel(unsigned edge);

	void addIncidence(unsigned edge, unsigned vertex);

	void printGraph(); // prints the vertices and edges in lexicographical order

	void readFromFile(ifstream &file);

	void readGraph();

	void printToFile(string fileName);

	void printToFilePACEFormat(string fileName);

    void printToFileDirectedBipartiteGraphNAUTY(string fileName);

    bool isMultiGraph(); // Tests if for each edge there is exactly two distinct
						 // elements in the map.

	bool convertToGML(string fileName);

	bool convertFromGML();


};

/*
// Example of input
vertices:
1
2
4
6
edges:
1 2 3  // the meaning is that edge 1 has endpoints 2 and 3
1 3 2  // our graphs are undirected. This has the same effect as 1 2 3
3 2 4
5 3 2
*/
#endif
