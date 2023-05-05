#ifndef MULTIGRAPH_H
#define MULTIGRAPH_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <filesystem>

class MultiGraph {
  private:
	std::set<unsigned> vertices;
	std::set<unsigned> edges;
	std::multimap<unsigned, unsigned> incidenceMap; // incidenceMap(e,v) indicates that v belongs to e
  public:
	MultiGraph();
	// copy constructor
	MultiGraph(const MultiGraph &multiGraph);
	// Order of the graph
	unsigned verticesSize();
	// Size of the graph
	unsigned edgesSize();
	// set functions

	void setVertices(const std::set<unsigned> &vertices);

	void setEdges(const std::set<unsigned> &edges);

	void setIncidenceMap(const std::multimap<unsigned, unsigned> &incidenceMap);

	// get functions
	const std::set<unsigned> &getVertices() const;

	const std::set<unsigned> &getEdges() const;

	const std::multimap<unsigned, unsigned> &getIncidenceMap() const;

	unsigned nthVertex(unsigned n); // get nth vertex's label in the vertex set

	// auxiliary functions

	void addVertex(unsigned vertex);

	void deleteVertex(unsigned vertex);

	unsigned addEdgeEndPoints(unsigned i, unsigned j); // Adds i and j as an edge and returns a label of the new edge

	std::set<unsigned> edgesBetweenVertices(unsigned i, unsigned j); // Returns label of the edges between i and j

	std::set<unsigned> edgesVertex(unsigned i); // Returns labels of the incident edges to i

	void deleteEdgeEndpoints(unsigned i, unsigned j);

	void addEdgeLabel(unsigned edge);

	void deleteEdgeLabel(unsigned edge);

	void addIncidence(unsigned edge, unsigned vertex);

	void printGraph(); // prints the vertices and edges in lexicographical order

	void readFromFile(std::ifstream &file);

	void readGraph();

	void printToFile(std::string fileName);

	void printToFilePACEFormat(std::string fileName);

    void printToFileDirectedBipartiteGraphNAUTY(std::string fileName);

    bool isMultiGraph(); // Tests if for each edge there is exactly two distinct
						 // elements in the map.

	bool convertToGML(std::string fileName);

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
