#pragma once
#include <istream>
#include "../../../Multigraph/MultiGraph.h"

int gr_parse(std::istream &is, MultiGraph &graph, int &result);
