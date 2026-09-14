#pragma once

#include <string>
#include <vector>

namespace aksiomat::discrete_math {

using AdjacencyList = std::vector<std::vector<int>>;

struct GraphAnalysisResult {
	std::vector<int> degrees;
	bool connected;
	bool eulerian;
	bool bipartite;
	std::vector<std::string> steps;
};

struct ShortestPathResult {
	bool reachable;
	int distance;
	std::vector<int> path;
	std::vector<std::string> steps;
};

class GraphTheory {
public:
	static GraphAnalysisResult analyze(const AdjacencyList& adjacency, bool directed);
	static ShortestPathResult shortestPath(const AdjacencyList& adjacency, int source, int target);
};

} // namespace aksiomat::discrete_math
