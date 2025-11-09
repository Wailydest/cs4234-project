#include "graph_generator.h"
#include <algorithm>
#include <stdexcept>

GraphGenerator::GraphGenerator(unsigned int seed)
    : rng_(seed), dist_(0.0, 1.0) {}

std::vector<std::pair<int, int>>
GraphGenerator::generateErdosRenyiBipartite(int n, double p) {
  std::vector<std::pair<int, int>> edges;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (dist_(rng_) < p) {
        edges.emplace_back(i, j);
      }
    }
  }

  return edges;
}

std::vector<std::pair<int, int>>
GraphGenerator::generateCompleteBipartite(int n) {
  std::vector<std::pair<int, int>> edges;

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      edges.emplace_back(i, j);
    }
  }

  return edges;
}

std::vector<std::pair<int, int>> GraphGenerator::generateRandomGraph(int n,
                                                                     double p) {
  std::vector<std::pair<int, int>> edges;

  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (dist_(rng_) < p) {
        edges.emplace_back(i, j);
      }
    }
  }

  return edges;
}

std::vector<std::pair<int, int>>
GraphGenerator::generateRandomDirectedGraph(int n, double p,
                                            int minHamiltonianPathLength) {
  if (minHamiltonianPathLength > n || minHamiltonianPathLength < 0) {
    throw std::invalid_argument(
        "minHamiltonianPathLength must be between 0 and n");
  }
  std::vector<int> random_permutation(n);
  std::iota(random_permutation.begin(), random_permutation.end(), 0);
  std::shuffle(random_permutation.begin(), random_permutation.end(), rng_);
  std::vector<std::pair<int, int>> edges;
  for (int i = 0; i < minHamiltonianPathLength; ++i) {
    edges.emplace_back(random_permutation[i], random_permutation[i + 1]);
  }

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (i != j && dist_(rng_) < p) {
        edges.emplace_back(i, j);
      }
    }
  }

  std::sort(edges.begin(), edges.end());
  edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

  return edges;
}

std::vector<std::vector<int>> GraphGenerator::generate3DGraph(int n, double p) {
  std::vector<std::vector<int>> hyperedges;

  // Generate tripartite hypergraph: each hyperedge connects one vertex from
  // each partition
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      for (int k = 0; k < n; ++k) {
        if (dist_(rng_) < p) {
          hyperedges.push_back({i, j, k});
        }
      }
    }
  }

  return hyperedges;
}
