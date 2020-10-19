// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "MultiGraph.h"

MultiGraph::MultiGraph() {}

MultiGraph::MultiGraph(const MultiGraph &multiGraph) {
	this->vertices = multiGraph.getVertices();
	this->edges = multiGraph.getEdges();
	this->incidenceMap = multiGraph.getIncidenceMap();
}

void MultiGraph::setVertices(const set<unsigned> &vertices) {
	MultiGraph::vertices = vertices;
}

void MultiGraph::setEdges(const set<unsigned> &edges) {
	MultiGraph::edges = edges;
}

void MultiGraph::setIncidenceMap(
	const multimap<unsigned, unsigned> &incidenceMap) {
	MultiGraph::incidenceMap = incidenceMap;
}

const set<unsigned> &MultiGraph::getVertices() const { return vertices; }

const set<unsigned> &MultiGraph::getEdges() const { return edges; }

const multimap<unsigned, unsigned> &MultiGraph::getIncidenceMap() const {
	return incidenceMap;
}

void MultiGraph::addVertex(unsigned vertex) { this->vertices.insert(vertex); }

void MultiGraph::addEdgeLabel(unsigned edge) { this->edges.insert(edge); }

void MultiGraph::addIncidence(unsigned edge, unsigned vertex) {
	this->incidenceMap.insert(make_pair(edge, vertex));
}

void MultiGraph::deleteVertex(unsigned vertex) {
	cout << "deleteVertex have not been implemented" << endl;
	exit(20);
}

unsigned MultiGraph::addEdgeEndPoints(unsigned i, unsigned j) {
	unsigned last = 0;
	if (edges.begin() != edges.end()) {
		last = *edges.rbegin();
	}
	vertices.insert(i);
	vertices.insert(j);
	edges.insert(last + 1);
	incidenceMap.insert(make_pair(last + 1, i));
	incidenceMap.insert(make_pair(last + 1, j));
	return last + 1;
}

set<unsigned> MultiGraph::edgesVertex(unsigned i) {
	set<unsigned> edges;
	for (auto it = this->incidenceMap.begin(); it != this->incidenceMap.end();
		 it++) {
		if (it->second == i) {
			edges.insert(it->first);
		}
	}
	return edges;
}

set<unsigned> MultiGraph::edgesBetweenVertices(unsigned i, unsigned j) {
	set<unsigned> edgesI = edgesVertex(i);
	set<unsigned> edgesJ = edgesVertex(j);
	set<unsigned> sharedEdges;
	set_intersection(edgesI.begin(), edgesI.end(), edgesJ.begin(), edgesJ.end(),
					 inserter(sharedEdges, sharedEdges.begin()));
	return sharedEdges;
}

void MultiGraph::printGraph() {
	cout << "vertices:" << endl;
	for (set<unsigned>::iterator it = this->vertices.begin();
		 it != this->vertices.end(); ++it) {
		cout << *it << endl;
	}
	cout << "Edges" << endl;
	for (set<unsigned>::iterator itr = this->edges.begin();
		 itr != this->edges.end(); ++itr) {
		cout << *itr << "\t";
		auto equalIt = incidenceMap.equal_range(*itr);
		for (auto it = equalIt.first; it != equalIt.second; ++it) {
			cout << it->second << "\t";
		}
		cout << endl;
	}
}

void MultiGraph::readFromFile(ifstream &file) {
	string str;
	bool isVertex = false;
	bool isEdge = false;
	unsigned edgeC = 0;
	unsigned edgeNo = 1;
	while (file >> str) {
		if (str == "vertices") {
			isVertex = true;
		} else if (str == "edges") {
			isVertex = false;
			isEdge = true;
		} else if (isVertex) {
			this->addVertex(stoi(str));
		} else if (isEdge) {
			if (edgeC == 0) {
				if (incidenceMap.count(stoi(str)) != 0) {
					cout << "ERROR: in MultiGraph::readFromFile there are two "
							"edges with same name"
						 << endl;
					exit(20);
				}
				this->addEdgeLabel(stoi(str));
				edgeC = 1;
				edgeNo = stoi(str);
			} else if (edgeC == 1) {
				addIncidence(edgeNo, stoi(str));
				edgeC = 2;
			} else if (edgeC == 2) {
				addIncidence(edgeNo, stoi(str));
				edgeC = 0;
			} else {
				cout << "ERROR: in MultiGraph::readFromFile format did not "
						"matched"
					 << endl;
				exit(20);
			}
		} else {
			cout << "ERROR: in MultiGraph::readFromFile format did not matched"
				 << endl;
			exit(20);
		}
	}
	if (!isMultigraph()) {
		cout << "The graph is not valid multigraph" << endl;
		exit(20);
	}
}

void MultiGraph::printToFile(ofstream &file) {
	file.open("multigraphOutput.txt");
	file << "vertices:" << endl;
	for (set<unsigned>::iterator it = this->vertices.begin();
		 it != this->vertices.end(); ++it) {
		file << *it << endl;
	}
	file << "Edges" << endl;
	for (set<unsigned>::iterator itr = this->edges.begin();
		 itr != this->edges.end(); ++itr) {
		file << *itr << "\t";
		auto equalIt = incidenceMap.equal_range(*itr);
		for (auto it = equalIt.first; it != equalIt.second; ++it) {
			file << it->second << "\t";
		}
		file << endl;
	}
}

bool MultiGraph::isMultigraph() {
	for (set<unsigned>::iterator itr = this->edges.begin();
		 itr != this->edges.end(); ++itr) {
		if (this->incidenceMap.count(*itr) != 2) {
			return false;
		}
	}
	return true;
}

bool MultiGraph::convertToGML() {
	ofstream gmlFile("convertGml.gml");
	gmlFile << "graph [\n";
	for (auto it = vertices.begin(); it != vertices.end(); it++) {
		gmlFile << "node \n [\n id " + to_string(*it) + "\n ]\n";
	}
	for (auto itr = edges.begin(); itr != edges.end(); itr++) {
		gmlFile << "edge \n [\n";
		auto values = incidenceMap.equal_range(*itr);
		if (std::distance(values.first, values.second) == 2) {
			auto tempIt = values.first;
			gmlFile << "source " + to_string(tempIt->second) + "\n";
			tempIt++;
			gmlFile << "target " + to_string(tempIt->second) + "\n ]\n";
		} else {
			cout << "ERROR: MultiGraph::convertToGML multigraph is not valid"
				 << endl;
			return false;
		}
	}
	gmlFile << "]\n";
	gmlFile.close();
	return true;
}
