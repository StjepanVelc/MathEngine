#include "aksiomat/discrete_math/GraphTheory.hpp"

#include <gtest/gtest.h>

using namespace aksiomat::discrete_math;

TEST(GraphTheory, ComputesDegreesForUndirectedGraph) {
	AdjacencyList adjacency{{1, 2}, {0, 2}, {0, 1}};
	GraphAnalysisResult result = GraphTheory::analyze(adjacency, false);
	EXPECT_EQ(result.degrees[0], 2);
	EXPECT_EQ(result.degrees[1], 2);
	EXPECT_EQ(result.degrees[2], 2);
}

TEST(GraphTheory, DetectsConnectedGraph) {
	AdjacencyList adjacency{{1}, {0, 2}, {1}};
	GraphAnalysisResult result = GraphTheory::analyze(adjacency, false);
	EXPECT_TRUE(result.connected);
}

TEST(GraphTheory, DetectsDisconnectedGraph) {
	AdjacencyList adjacency{{1}, {0}, {3}, {2}};
	GraphAnalysisResult result = GraphTheory::analyze(adjacency, false);
	EXPECT_FALSE(result.connected);
}

TEST(GraphTheory, DetectsEulerianCircleOnTriangle) {
	AdjacencyList adjacency{{1, 2}, {0, 2}, {0, 1}};
	GraphAnalysisResult result = GraphTheory::analyze(adjacency, false);
	EXPECT_TRUE(result.eulerian);
}

TEST(GraphTheory, DetectsBipartiteGraph) {
	AdjacencyList adjacency{{1}, {0, 2}, {1}};
	GraphAnalysisResult result = GraphTheory::analyze(adjacency, false);
	EXPECT_TRUE(result.bipartite);
}

TEST(GraphTheory, DetectsNonBipartiteTriangle) {
	AdjacencyList adjacency{{1, 2}, {0, 2}, {0, 1}};
	GraphAnalysisResult result = GraphTheory::analyze(adjacency, false);
	EXPECT_FALSE(result.bipartite);
}

TEST(GraphTheory, FindsShortestPath) {
	AdjacencyList adjacency{{1}, {0, 2}, {1, 3}, {2}};
	ShortestPathResult result = GraphTheory::shortestPath(adjacency, 0, 3);
	ASSERT_TRUE(result.reachable);
	EXPECT_EQ(result.distance, 3);
	std::vector<int> expectedPath{0, 1, 2, 3};
	EXPECT_EQ(result.path, expectedPath);
}

TEST(GraphTheory, ReportsUnreachableTarget) {
	AdjacencyList adjacency{{1}, {0}, {3}, {2}};
	ShortestPathResult result = GraphTheory::shortestPath(adjacency, 0, 3);
	EXPECT_FALSE(result.reachable);
}

TEST(GraphTheory, ThrowsOnEmptyGraph) {
	EXPECT_THROW(GraphTheory::analyze({}, false), std::invalid_argument);
}
