#include "matroid.h"
#include "matroid_intersection.h"
#include "matroid_implementations.h"
#include "graph_generator.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

// Parse command line arguments and run experiments
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [args...]" << std::endl;
        std::cerr << "Commands:" << std::endl;
        std::cerr << "  bipartite <n> <m> <p> <k> <epsilon> <seed>" << std::endl;
        std::cerr << "  3dmatching <n> <p> <epsilon> <seed>" << std::endl;
        std::cerr << "  hamiltonian <n> <p> <epsilon> <seed>" << std::endl;
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "bipartite" && argc >= 7) {
        int n = std::stoi(argv[2]);
        int m = std::stoi(argv[3]);
        double p = std::stod(argv[4]);
        int k = std::stoi(argv[5]);
        double epsilon = std::stod(argv[6]);
        unsigned int seed = (argc >= 8) ? std::stoul(argv[7]) : 42;
        
        // Generate random bipartite graph
        GraphGenerator gen(seed);
        auto edges = gen.generateErdosRenyiBipartite(n, m, p);
        
        // Create k partition matroids for k-matching
        std::vector<std::shared_ptr<Matroid>> matroids;
        
        if (k == 2) {
            // Two matroids: one for left partition, one for right partition
            auto leftMatroid = std::make_shared<BipartiteMatchingMatroid>(n, m, edges);
            auto rightMatroid = std::make_shared<BipartiteMatchingMatroid>(n, m, edges);
            matroids.push_back(leftMatroid);
            matroids.push_back(rightMatroid);
        } else if (k == 3) {
            // Three partition matroids for 3D matching
            // Simplified: use partition matroids
            std::vector<std::vector<int>> partitions1, partitions2, partitions3;
            for (size_t i = 0; i < edges.size(); ++i) {
                partitions1.push_back({(int)i});
                partitions2.push_back({(int)i});
                partitions3.push_back({(int)i});
            }
            
            matroids.push_back(std::make_shared<BipartiteMatchingMatroid>(n, m, edges));
            matroids.push_back(std::make_shared<UniformMatroid>(edges.size(), edges.size() / 2));
            matroids.push_back(std::make_shared<UniformMatroid>(edges.size(), edges.size() / 2));
        }
        
        // Run baseline
        BaselineAlgorithm baseline(matroids);
        auto baselineSolution = baseline.run();
        
        // Run local search
        LocalSearchAlgorithm localSearch(matroids, epsilon);
        auto localSearchSolution = localSearch.run();
        
        // Output results in JSON format for Python parsing
        std::cout << "{";
        std::cout << "\"edges\": " << edges.size() << ", ";
        std::cout << "\"baseline_size\": " << baselineSolution.size() << ", ";
        std::cout << "\"local_search_size\": " << localSearchSolution.size() << ", ";
        std::cout << "\"iterations\": " << localSearch.getIterations();
        std::cout << "}" << std::endl;
        
    } else if (command == "3dmatching" && argc >= 5) {
        int n = std::stoi(argv[2]);
        double p = std::stod(argv[3]);
        double epsilon = std::stod(argv[4]);
        unsigned int seed = (argc >= 6) ? std::stoul(argv[5]) : 42;
        
        // Generate 3D matching instance
        // Using three bipartite graphs
        GraphGenerator gen(seed);
        auto edges1 = gen.generateErdosRenyiBipartite(n, n, p);
        auto edges2 = gen.generateErdosRenyiBipartite(n, n, p);
        auto edges3 = gen.generateErdosRenyiBipartite(n, n, p);
        
        // Combine edges (simplified version)
        std::vector<std::pair<int, int>> allEdges;
        allEdges.insert(allEdges.end(), edges1.begin(), edges1.end());
        
        // Create three matroids
        std::vector<std::shared_ptr<Matroid>> matroids;
        matroids.push_back(std::make_shared<BipartiteMatchingMatroid>(n, n, edges1));
        matroids.push_back(std::make_shared<BipartiteMatchingMatroid>(n, n, edges2));
        matroids.push_back(std::make_shared<BipartiteMatchingMatroid>(n, n, edges3));
        
        // Run algorithms
        BaselineAlgorithm baseline(matroids);
        auto baselineSolution = baseline.run();
        
        LocalSearchAlgorithm localSearch(matroids, epsilon);
        auto localSearchSolution = localSearch.run();
        
        std::cout << "{";
        std::cout << "\"edges\": " << allEdges.size() << ", ";
        std::cout << "\"baseline_size\": " << baselineSolution.size() << ", ";
        std::cout << "\"local_search_size\": " << localSearchSolution.size() << ", ";
        std::cout << "\"iterations\": " << localSearch.getIterations();
        std::cout << "}" << std::endl;
        
    } else if (command == "hamiltonian" && argc >= 5) {
        int n = std::stoi(argv[2]);
        double p = std::stod(argv[3]);
        double epsilon = std::stod(argv[4]);
        unsigned int seed = (argc >= 6) ? std::stoul(argv[5]) : 42;
        
        // Generate random graph
        GraphGenerator gen(seed);
        auto edges = gen.generateRandomGraph(n, p);
        
        // Create two matroids: graphic matroid and path matroid
        std::vector<std::shared_ptr<Matroid>> matroids;
        matroids.push_back(std::make_shared<GraphicMatroid>(n, edges));
        matroids.push_back(std::make_shared<PathMatroid>(n, edges));
        
        // Run algorithms
        BaselineAlgorithm baseline(matroids);
        auto baselineSolution = baseline.run();
        
        LocalSearchAlgorithm localSearch(matroids, epsilon);
        auto localSearchSolution = localSearch.run();
        
        std::cout << "{";
        std::cout << "\"edges\": " << edges.size() << ", ";
        std::cout << "\"baseline_size\": " << baselineSolution.size() << ", ";
        std::cout << "\"local_search_size\": " << localSearchSolution.size() << ", ";
        std::cout << "\"iterations\": " << localSearch.getIterations();
        std::cout << "}" << std::endl;
        
    } else {
        std::cerr << "Invalid command or arguments" << std::endl;
        return 1;
    }
    
    return 0;
}
