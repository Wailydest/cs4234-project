#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <random>
#include <utility>
#include <vector>

// Generate random Erdős-Rényi bipartite graph
class GraphGenerator {
public:
  GraphGenerator(unsigned int seed = std::random_device{}());

  // Generate random bipartite graph with n vertices on left, m on right,
  // probability p
  std::vector<std::pair<int, int>> generateErdosRenyiBipartite(int n, double p);

  // Generate random complete bipartite graph
  std::vector<std::pair<int, int>> generateCompleteBipartite(int n);

  // Generate random graph for Hamiltonian path testing
  std::vector<std::pair<int, int>> generateRandomGraph(int n, double p);

  // Generate random directed graph on n vertices
  // Each directed edge (i, j) where i != j is added with probability p
  std::vector<std::pair<int, int>>
  generateRandomDirectedGraph(int n, double p, int minHamiltonianPathLength);

  // Generate 3D (tripartite) hypergraph: 3-uniform hypergraph on 3 partitions
  // n: number of vertices in each partition
  // p: probability of adding a hyperedge connecting one vertex from each
  // partition
  std::vector<std::vector<int>> generate3DGraph(int n, double p);

private:
  std::mt19937 rng_;
  std::uniform_real_distribution<double> dist_;
};

#endif // GRAPH_GENERATOR_H
