#include "gr_parser.hpp"

#include <sstream>

int gr_parse(std::istream &is, MultiGraph &graph, int &result) {
	std::string line_str;
	std::istringstream line;

	auto next_non_comment = [&]() {
		while (std::getline(is, line_str) && line_str.starts_with("c"))
			;
		if (!line_str.size()) return false;
		line.str(line_str);
		line.clear();
		return true;
	};

	if (!next_non_comment()) {
		std::cerr << "missing header line\n";
		return result = 20;
	}

	std::string token;
	line >> token;
	if (token != "p") {
		std::cerr << "first token in header line is not p\n";
		return result = 21;
	}
	line >> token;
	if (token != "tw") {
		std::cerr << "second token in header line is not tw\n";
		return result = 21;
	}

	int V, E;
	line >> V >> E;
	if (!line) {
		std::cerr << "header line didn't contain numbers V and E\n";
		return result = 20;
	}

	for (int v = 1; v <= V; ++v) graph.addVertex(v);

	int edge_cnt = 0;

	while (next_non_comment()) {
		++edge_cnt;
		int u, v;
		line >> u >> v;
		if (!line || !(1 <= u && u <= V) || !(1 <= v && v <= V)) {
			std::cerr << "incorrect format for edge number " << edge_cnt
					  << '\n';
			return result = 20;
		}
		graph.addEdgeEndPoints(u, v);
	}

	if (edge_cnt != E) {
		std::cerr << "incorrect number of edges (expected " << E << ", found "
				  << edge_cnt << ")\n";
		return result = 20;
	}

	return result = 0;
}
