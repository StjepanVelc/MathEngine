#include "aksiomat/discrete_math/GraphTheory.hpp"

#include <algorithm>
#include <queue>
#include <sstream>
#include <stdexcept>

namespace aksiomat::discrete_math {

namespace {

void validateAdjacency(const AdjacencyList& adjacency) {
	if (adjacency.empty()) {
		throw std::invalid_argument("Graf mora imati barem jedan vrh.");
	}
	const int n = static_cast<int>(adjacency.size());
	for (const auto& neighbors : adjacency) {
		for (int neighbor : neighbors) {
			if (neighbor < 0 || neighbor >= n) {
				throw std::invalid_argument("Susjed izvan raspona vrhova grafa.");
			}
		}
	}
}

} // namespace

GraphAnalysisResult GraphTheory::analyze(const AdjacencyList& adjacency, bool directed) {
	validateAdjacency(adjacency);
	const int n = static_cast<int>(adjacency.size());

	std::vector<int> degrees(n, 0);
	for (int v = 0; v < n; ++v) {
		degrees[v] += static_cast<int>(adjacency[v].size());
	}
	if (!directed) {
		// Za neusmjeren graf svaki brid se broji na oba kraja pa je stupanj vec ispravno zbrojen po popisu susjeda.
	}

	// Povezanost pomocu BFS-a od vrha 0 (koristi se simetricno gledanje bridova).
	std::vector<std::vector<int>> undirectedView(n);
	for (int v = 0; v < n; ++v) {
		for (int neighbor : adjacency[v]) {
			undirectedView[v].push_back(neighbor);
			undirectedView[neighbor].push_back(v);
		}
	}
	std::vector<bool> visited(n, false);
	std::queue<int> queue;
	queue.push(0);
	visited[0] = true;
	int visitedCount = 1;
	while (!queue.empty()) {
		int current = queue.front();
		queue.pop();
		for (int neighbor : undirectedView[current]) {
			if (!visited[neighbor]) {
				visited[neighbor] = true;
				++visitedCount;
				queue.push(neighbor);
			}
		}
	}
	const bool connected = visitedCount == n;

	// Eulerov ciklus postoji ako je graf povezan i svi stupnjevi su parni (za neusmjereni graf).
	bool eulerian = connected;
	if (!directed) {
		for (int degree : degrees) {
			if (degree % 2 != 0) { eulerian = false; break; }
		}
	} else {
		eulerian = false;
	}

	// Dvodjelnost pomocu 2-bojanja BFS-om.
	std::vector<int> color(n, -1);
	bool bipartite = true;
	for (int start = 0; start < n && bipartite; ++start) {
		if (color[start] != -1) continue;
		color[start] = 0;
		std::queue<int> colorQueue;
		colorQueue.push(start);
		while (!colorQueue.empty() && bipartite) {
			int current = colorQueue.front();
			colorQueue.pop();
			for (int neighbor : undirectedView[current]) {
				if (color[neighbor] == -1) {
					color[neighbor] = 1 - color[current];
					colorQueue.push(neighbor);
				} else if (color[neighbor] == color[current]) {
					bipartite = false;
					break;
				}
			}
		}
	}

	std::ostringstream degreeStream;
	for (int v = 0; v < n; ++v) {
		if (v) degreeStream << ", ";
		degreeStream << "deg(" << v << ")=" << degrees[v];
	}

	std::vector<std::string> steps{
		"Stupnjevi vrhova: " + degreeStream.str() + ".",
		std::string("Graf je povezan: ") + (connected ? "da" : "ne") + " (provjereno BFS obilaskom od vrha 0).",
		std::string("Eulerov ciklus postoji: ") + (eulerian ? "da" : "ne") + " (graf povezan i svi stupnjevi parni).",
		std::string("Graf je dvodjelan: ") + (bipartite ? "da" : "ne") + " (moguce 2-bojanje BFS obilaskom)."
	};

	return GraphAnalysisResult{degrees, connected, eulerian, bipartite, std::move(steps)};
}

ShortestPathResult GraphTheory::shortestPath(const AdjacencyList& adjacency, int source, int target) {
	validateAdjacency(adjacency);
	const int n = static_cast<int>(adjacency.size());
	if (source < 0 || source >= n || target < 0 || target >= n) {
		throw std::invalid_argument("Izvorni ili ciljni vrh je izvan raspona grafa.");
	}

	std::vector<int> distance(n, -1);
	std::vector<int> predecessor(n, -1);
	std::queue<int> queue;
	distance[source] = 0;
	queue.push(source);
	while (!queue.empty()) {
		int current = queue.front();
		queue.pop();
		for (int neighbor : adjacency[current]) {
			if (distance[neighbor] == -1) {
				distance[neighbor] = distance[current] + 1;
				predecessor[neighbor] = current;
				queue.push(neighbor);
			}
		}
	}

	const bool reachable = distance[target] != -1;
	std::vector<int> path;
	if (reachable) {
		for (int at = target; at != -1; at = predecessor[at]) {
			path.push_back(at);
		}
		std::reverse(path.begin(), path.end());
	}

	std::vector<std::string> steps{
		"Najkraci put trazi se BFS obilaskom jer su svi bridovi jedinicne tezine.",
		reachable
			? ("Pronaden je put duljine " + std::to_string(distance[target]) + " od vrha " + std::to_string(source) + " do vrha " + std::to_string(target) + ".")
			: ("Vrh " + std::to_string(target) + " nije dostiziv iz vrha " + std::to_string(source) + ".")
	};

	return ShortestPathResult{reachable, reachable ? distance[target] : -1, path, std::move(steps)};
}

} // namespace aksiomat::discrete_math
