#include "td_parser.hpp"

#include <vector>

int td_parse(std::istream &is, TreeDecompositionPACE &td, int &result) {
  std::cerr << "Hello, from tr_parse!\n";

  std::string line_str;
  std::istringstream line;

  auto next_non_comment = [&]() {
    while (std::getline(is, line_str) && line_str.starts_with("c"))
      ;
    if (!line_str.size()) return false;
    line = std::istringstream(line_str);
    return true;
  };

  if (!next_non_comment()) {
    std::cerr << "missing header line\n";
    return result = 20;
  }

  std::string token;
  line >> token;
  if (token != "s") {
    std::cerr << "first token in header line is not s\n";
    return result = 21;
  }
  line >> token;
  if (token != "td") {
    std::cerr << "second token in header line is not td\n";
    return result = 21;
  }

  int target_bag_cnt, width_plus_one, V;
  line >> target_bag_cnt >> width_plus_one >> V;

  if (!line) {
    std::cerr << "header line doesn't contain bag count, width + 1, and V\n";
    return result = 20;
  }

  td.setWidthType("tree_width");
  td.setNum_vertices(target_bag_cnt);
  td.setWidth(width_plus_one - 1);
  td.setNum_graph_vertices(V);

  int bag_cnt = 0;
  int edge_cnt = 0;

  std::vector<int> bag;

  while (next_non_comment()) {
    if (line_str.starts_with("b")) {
      ++bag_cnt;

      if (edge_cnt) {
        std::cerr << "all bags must come before the edges in the tree "
                     "decomposition\n";
        return result = 20;
      }

      line >> token;
      if (token != "b") {
        std::cerr << "incorrect bag format\n";
        return result = 20;
      }

      int bag_id;
      line >> bag_id;

      if (!line || !(1 <= bag_id && bag_id <= target_bag_cnt)) {
        std::cerr << "incorrect or missing bag id for bag number " << bag_cnt
                  << '\n';
        return result = 20;
      }

      bag.clear();
      int nxt_vertex;
      while (line >> nxt_vertex) bag.push_back(nxt_vertex);

      if (!td.setABag({bag.begin(), bag.end()}, bag_id)) {
        std::cerr << "failed to set bag number " << bag_cnt
                  << " (id = " << bag_id << ")\n";
        return result = 20;
      }

    } else {
      ++edge_cnt;

      if (bag_cnt != target_bag_cnt) {
        std::cerr << "incorrect number of bags before edge list\n";
        return result = 20;
      }

      int i, j;
      line >> i >> j;

      if (!(1 <= i && i <= bag_cnt) || !(1 <= j && j <= bag_cnt)) {
        std::cerr << "incorrect bag ids for edge number " << edge_cnt << " ("
                  << i << ", " << j << ")\n";
        return result = 20;
      }

      if (!td.addEdge(i, j)) {
        std::cerr << "failed add edge number " << edge_cnt << " (" << i << ", "
                  << j << ")\n";
        return result = 20;
      }
    }
  }

  if (bag_cnt != target_bag_cnt) {
    std::cerr << "incorrect number of bags (expected" << target_bag_cnt
              << ", found " << bag_cnt << ")\n";
    return result = 20;
  }

  if (edge_cnt != bag_cnt - 1) {
    std::cerr << "incorrect number of edges (expected" << bag_cnt - 1
              << ", found " << edge_cnt << ")\n";
    return result = 20;
  }

  return result = 0;
}
