#include "MultiGraph.h"

MultiGraph::MultiGraph() {}

MultiGraph::MultiGraph(const MultiGraph &multiGraph) {
	this->vertices = multiGraph.getVertices();
	this->edges = multiGraph.getEdges();
	this->incidenceMap = multiGraph.getIncidenceMap();
}

void MultiGraph::setVertices(const std::set<unsigned> &vertices) {
	MultiGraph::vertices = vertices;
}

void MultiGraph::setEdges(const std::set<unsigned> &edges) {
	MultiGraph::edges = edges;
}

void MultiGraph::setIncidenceMap(
	const std::multimap<unsigned, unsigned> &incidenceMap) {
	MultiGraph::incidenceMap = incidenceMap;
}

const std::set<unsigned> &MultiGraph::getVertices() const { return vertices; }

const std::set<unsigned> &MultiGraph::getEdges() const { return edges; }

const std::multimap<unsigned, unsigned> &MultiGraph::getIncidenceMap() const {
	return incidenceMap;
}

void MultiGraph::addVertex(unsigned vertex) { this->vertices.insert(vertex); }

void MultiGraph::addEdgeLabel(unsigned edge) { this->edges.insert(edge); }

void MultiGraph::addIncidence(unsigned edge, unsigned vertex) {
	this->incidenceMap.insert(std::make_pair(edge, vertex));
}

void MultiGraph::deleteVertex(unsigned vertex) {
	std::cout << "deleteVertex have not been implemented" << std::endl;
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
	incidenceMap.insert(std::make_pair(last + 1, i));
	incidenceMap.insert(std::make_pair(last + 1, j));
	return last + 1;
}

std::set<unsigned> MultiGraph::edgesVertex(unsigned i) {
	std::set<unsigned> edges;
	for (auto it = this->incidenceMap.begin(); it != this->incidenceMap.end();
		 it++) {
		if (it->second == i) {
			edges.insert(it->first);
		}
	}
	return edges;
}

std::set<unsigned> MultiGraph::edgesBetweenVertices(unsigned i, unsigned j) {
	std::set<unsigned> edgesI = edgesVertex(i);
	std::set<unsigned> edgesJ = edgesVertex(j);
	std::set<unsigned> sharedEdges;
	set_intersection(edgesI.begin(), edgesI.end(), edgesJ.begin(), edgesJ.end(),
					 inserter(sharedEdges, sharedEdges.begin()));
	return sharedEdges;
}

void MultiGraph::printGraph() {
	std::cout << "vertices:" << std::endl;
	for (std::set<unsigned>::iterator it = this->vertices.begin();
		 it != this->vertices.end(); ++it) {
		std::cout << *it << std::endl;
	}
	std::cout << "Edges" << std::endl;
	for (std::set<unsigned>::iterator itr = this->edges.begin();
		 itr != this->edges.end(); ++itr) {
		std::cout << *itr << "\t";
		auto equalIt = incidenceMap.equal_range(*itr);
		for (auto it = equalIt.first; it != equalIt.second; ++it) {
			std::cout << it->second << "\t";
		}
		std::cout << std::endl;
	}
}

void MultiGraph::readFromFile(std::ifstream &file) {
	std::string str;
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
					std::cout << "ERROR: in MultiGraph::readFromFile there are two "
							"edges with same name"
						 << std::endl;
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
				std::cout << "ERROR: in MultiGraph::readFromFile format did not "
						"matched"
					 << std::endl;
				exit(20);
			}
		} else {
			std::cout << "ERROR: in MultiGraph::readFromFile format did not matched"
				 << std::endl;
			exit(20);
		}
	}
	if (!isMultiGraph()) {
		std::cout << "The graph is not valid multigraph" << std::endl;
		exit(20);
	}
}

void MultiGraph::printToFile(std::string fileName) {
   // fileName = "Counterexample_Graph_"+multigraph_fs::path(fileName).filename().string();
    std::ofstream multiGraphFile (fileName);
    if (multiGraphFile.is_open())
    {
        multiGraphFile << "vertices:" << std::endl;
        for (std::set<unsigned>::iterator it = this->vertices.begin();
             it != this->vertices.end(); ++it) {
            multiGraphFile << *it << std::endl;
        }
        multiGraphFile << "Edges" << std::endl;
        for (std::set<unsigned>::iterator itr = this->edges.begin();
             itr != this->edges.end(); ++itr) {
            multiGraphFile << *itr << "\t";
            auto equalIt = incidenceMap.equal_range(*itr);
            for (auto it = equalIt.first; it != equalIt.second; ++it) {
                multiGraphFile << it->second << "\t";
            }
            multiGraphFile << std::endl;
        }
        multiGraphFile.close();
    }else {
        std::cout << "Unable to open "<< fileName << std::endl;
        exit(20);
    }
}

void MultiGraph::printToFilePACEFormat(std::string fileName) {
    std::ofstream multiGraphFile (fileName);
    if (multiGraphFile.is_open())
    {
        multiGraphFile << "c This is the counter example for "<<fileName << std::endl;
        multiGraphFile<< "p tw " << this->vertices.size() << " " << edges.size() << std::endl;
        for (std::set<unsigned>::iterator itr = this->edges.begin(); itr != this->edges.end(); ++itr) {
            auto equalIt = incidenceMap.equal_range(*itr);
            bool spaceFlag = false; // This is used to put spaces between endpoints of an edge.
            for (auto it = equalIt.first; it != equalIt.second; ++it) {
                if(spaceFlag){
                    multiGraphFile << " " ;
                }
                multiGraphFile << it->second;
                spaceFlag = true;
            }

            multiGraphFile << std::endl;
        }
        multiGraphFile.close();
    }else {
        std::cout << "Unable to open "<< fileName << std::endl;
        exit(20);
    }

}

void MultiGraph::printToFileDirectedBipartiteGraphNAUTY(std::string fileName) {
    std::ofstream multiGraphFile (fileName);
    if (multiGraphFile.is_open())
    {
        multiGraphFile << "n="<<this->vertices.size()+this->edgesSize()<< " g" << std::endl;
        for(auto edge: this->incidenceMap){
            multiGraphFile<< edge.first << ":" << edge.second+edgesSize() << std::endl;
        }
        multiGraphFile.close();
    }else {
        std::cout << "Unable to open "<< fileName << std::endl;
        exit(20);
    }

}

bool MultiGraph::isMultiGraph() {
	for (std::set<unsigned>::iterator itr = this->edges.begin();
		 itr != this->edges.end(); ++itr) {
		if (this->incidenceMap.count(*itr) != 2) {
			return false;
		}
	}
	return true;
}

bool MultiGraph::convertToGML(std::string fileName) {
    std::ofstream gmlFile(fileName);
	gmlFile << "graph [\n";
	for (auto it = vertices.begin(); it != vertices.end(); it++) {
		gmlFile << "node \n [\n id " + std::to_string(*it) + "\n ]\n";
	}
	for (auto itr = edges.begin(); itr != edges.end(); itr++) {
		gmlFile << "edge \n [\n";
		auto values = incidenceMap.equal_range(*itr);
		if (std::distance(values.first, values.second) == 2) {
			auto tempIt = values.first;
			gmlFile << "source " + std::to_string(tempIt->second) + "\n";
			tempIt++;
			gmlFile << "target " + std::to_string(tempIt->second) + "\n ]\n";
		} else {
			std::cout << "ERROR: MultiGraph::convertToGML multigraph is not valid"
				 << std::endl;
			return false;
		}
	}
	gmlFile << "]\n";
	gmlFile.close();
	return true;
}

unsigned MultiGraph::verticesSize() {
    return vertices.size();
}

unsigned MultiGraph::edgesSize() {
    return edges.size();
}

unsigned MultiGraph::nthVertex(unsigned n) {
   auto it = vertices.begin();
   advance(it,n);
   return *it;
}
