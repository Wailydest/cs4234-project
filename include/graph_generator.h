#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <vector>
#include <utility>
#include <random>

// Generate random Erdős-Rényi bipartite graph
class GraphGenerator {
public:
    GraphGenerator(unsigned int seed = std::random_device{}());
    
    // Generate random bipartite graph with n vertices on left, m on right, probability p
    std::vector<std::pair<int, int>> generateErdosRenyiBipartite(int n, int m, double p);
    
    // Generate random complete bipartite graph
    std::vector<std::pair<int, int>> generateCompleteBipartite(int n, int m);
    
    // Generate random graph for Hamiltonian path testing
    std::vector<std::pair<int, int>> generateRandomGraph(int n, double p);
    
private:
    std::mt19937 rng_;
    std::uniform_real_distribution<double> dist_;
};

#endif // GRAPH_GENERATOR_H
