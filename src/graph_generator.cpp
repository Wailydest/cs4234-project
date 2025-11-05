#include "graph_generator.h"
#include <algorithm>

GraphGenerator::GraphGenerator(unsigned int seed)
    : rng_(seed), dist_(0.0, 1.0) {}

std::vector<std::pair<int, int>> GraphGenerator::generateErdosRenyiBipartite(int n, int m, double p) {
    std::vector<std::pair<int, int>> edges;
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (dist_(rng_) < p) {
                edges.emplace_back(i, j);
            }
        }
    }
    
    return edges;
}

std::vector<std::pair<int, int>> GraphGenerator::generateCompleteBipartite(int n, int m) {
    std::vector<std::pair<int, int>> edges;
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            edges.emplace_back(i, j);
        }
    }
    
    return edges;
}

std::vector<std::pair<int, int>> GraphGenerator::generateRandomGraph(int n, double p) {
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
