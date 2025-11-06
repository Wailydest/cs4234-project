#include "graph_generator.h"
#include "matroid_implementations.h"
#include "matroid_intersection.h"
#include "matroid_problem.h"
#include "validation.h"
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

// Helper function to convert graph edges to JSON
template <typename EdgeType>
nlohmann::json graphToJson(const std::vector<EdgeType> &edges) {
  nlohmann::json graphJson = nlohmann::json::array();
  for (const auto &edge : edges) {
    if constexpr (std::is_same_v<EdgeType, std::vector<int>>) {
      // Hyperedge (vector of ints)
      graphJson.push_back(edge);
    } else if constexpr (std::is_same_v<EdgeType, std::pair<int, int>>) {
      // Regular edge (pair)
      graphJson.push_back({edge.first, edge.second});
    }
  }
  return graphJson;
}

// Helper function to add a solution to JSON output
void addSolutionToJson(nlohmann::json &output, const std::string &algorithm,
                       const ApproximationSolution &solution) {
  nlohmann::json solutionJson;
  solutionJson["algorithm"] = algorithm;
  solutionJson["approxRatio"] = solution.getApproximationRatio();
  solutionJson["solution"] = solution.getSolution();
  output["solutions"].push_back(solutionJson);
}

// Helper function to add multiple solutions to JSON output
void addSolutionsToJson(nlohmann::json &output, const std::string &algorithm,
                        const std::vector<ApproximationSolution> &solutions) {
  for (const auto &solution : solutions) {
    addSolutionToJson(output, algorithm, solution);
  }
}

// Helper function to run baseline and local search algorithms
struct AlgorithmResults {
  ApproximationSolution baseline;
  std::vector<ApproximationSolution> localSearch;
};

// Helper function to build final JSON output
nlohmann::json
buildOutputJson(const std::string &problemName, const nlohmann::json &graphJson,
                const AlgorithmResults &results,
                const ApproximationSolution *kuhnResult = nullptr) {
  nlohmann::json output;
  output["problem_name"] = problemName;
  output["graph"] = graphJson;
  output["solutions"] = nlohmann::json::array();

  addSolutionToJson(output, "baseline", results.baseline);

  if (kuhnResult) {
    addSolutionToJson(output, "kuhn", *kuhnResult);
  }

  addSolutionsToJson(output, "localsearch", results.localSearch);

  return output;
}

// Parse command line arguments and run experiments
int main(int argc, char *argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: " << argv[0] << " <command> [args...]" << std::endl;
      std::cerr << "Commands:" << std::endl;
      std::cerr << "  bipartite <n> <p> [seed] [timeLimit]" << std::endl;
      std::cerr << "  3dmatching <n> <p> [seed] [timeLimit]" << std::endl;
      std::cerr << "  hamiltonian <n> <p> [minHamiltonianPathLength] [seed] "
                   "[timeLimit]"
                << std::endl;
      return 1;
    }

    std::string command = argv[1];

    if (command == "bipartite" && argc >= 4) {
      int n = std::stoi(argv[2]);
      double p = std::stod(argv[3]);
      unsigned int seed = (argc >= 5) ? std::stoul(argv[4]) : 42;
      int timeLimit = (argc >= 6) ? std::stoi(argv[5]) : 10;

      // Generate random bipartite graph
      GraphGenerator gen(seed);
      auto edgePairs = gen.generateErdosRenyiBipartite(n, p);
      std::cerr << "Generated " << edgePairs.size() << " edges" << std::endl;

      // Convert pairs to vectors for MatchingProblem (2-uniform hypergraph)
      std::vector<std::vector<int>> edges;
      edges.reserve(edgePairs.size());
      for (const auto &pair : edgePairs) {
        edges.push_back({pair.first, pair.second});
      }

      // Create MatchingProblem for 2-uniform hypergraph (bipartite matching)
      auto matchingProblem = std::make_shared<MatchingProblem>(2, n, edges);

      // Run baseline algorithm
      BaselineAlgorithm baseline(matchingProblem);
      auto baselineResult = baseline.run();

      // Reset and run Kuhn 2D matching algorithm
      matchingProblem->reset();
      Kuhn2dMatchingAlgorithm kuhn(matchingProblem);
      auto kuhnResult = kuhn.run();

      // Reset and run local search algorithm
      matchingProblem->reset();
      LocalSearchAlgorithm localSearch(matchingProblem, timeLimit);
      auto localSearchSolutions = localSearch.run();

      // Validate all solutions before outputting
      validate_bipartite_matching(n, edgePairs, baselineResult.getSolution());
      validate_bipartite_matching(n, edgePairs, kuhnResult.getSolution());
      for (const auto &solution : localSearchSolutions) {
        validate_bipartite_matching(n, edgePairs, solution.getSolution());
      }

      // Build JSON output using helper functions
      auto graphJson = graphToJson(edges);
      AlgorithmResults results{baselineResult, localSearchSolutions};
      auto output =
          buildOutputJson("BIPARTITE", graphJson, results, &kuhnResult);

      std::cout << output.dump() << std::endl;

    } else if (command == "3dmatching" && argc >= 4) {
      int n = std::stoi(argv[2]);
      double p = std::stod(argv[3]);
      unsigned int seed = (argc >= 5) ? std::stoul(argv[4]) : 42;
      int timeLimit = (argc >= 6) ? std::stoi(argv[5]) : 10;

      // Generate 3D matching instance using tripartite hypergraph
      GraphGenerator gen(seed);
      auto hyperedges = gen.generate3DGraph(n, p);
      std::cerr << "Generated " << hyperedges.size() << " hyperedges"
                << std::endl;

      // Create MatchingProblem for 3-uniform hypergraph (3D matching)
      auto matchingProblem =
          std::make_shared<MatchingProblem>(3, n, hyperedges);

      // Run baseline algorithm
      BaselineAlgorithm baseline(matchingProblem);
      auto baselineResult = baseline.run();

      // Reset problem and run local search
      matchingProblem->reset();
      LocalSearchAlgorithm localSearch(matchingProblem, timeLimit);
      auto localSearchResults = localSearch.run();

      AlgorithmResults results{baselineResult, localSearchResults};

      // Validate all solutions before outputting
      validate_3d_matching(n, hyperedges, baselineResult.getSolution());
      for (const auto &solution : localSearchResults) {
        validate_3d_matching(n, hyperedges, solution.getSolution());
      }

      // Build JSON output using helper functions
      auto graphJson = graphToJson(hyperedges);
      auto output = buildOutputJson("3DMATCHING", graphJson, results);

      std::cout << output.dump() << std::endl;

    } else if (command == "hamiltonian" && argc >= 4) {
      int n = std::stoi(argv[2]);
      double p = std::stod(argv[3]);

      // Parse optional minHamiltonianPathLength, seed, and timeLimit
      // Format: hamiltonian <n> <p> [minHamiltonianPathLength] [seed]
      // [timeLimit]
      int minHamiltonianPathLength = 0;
      unsigned int seed = 42;
      int timeLimit = 10;

      if (argc >= 5) {
        if (argc >= 6) {
          if (argc >= 7) {
            // All three optional parameters provided
            minHamiltonianPathLength = std::stoi(argv[4]);
            seed = std::stoul(argv[5]);
            timeLimit = std::stoi(argv[6]);
          } else {
            // minHamiltonianPathLength and seed provided
            minHamiltonianPathLength = std::stoi(argv[4]);
            seed = std::stoul(argv[5]);
          }
        } else {
          // Only minHamiltonianPathLength provided (argc == 5)
          minHamiltonianPathLength = std::stoi(argv[4]);
        }
      }

      // Generate random directed graph for Hamiltonian path
      GraphGenerator gen(seed);
      auto edges =
          gen.generateRandomDirectedGraph(n, p, minHamiltonianPathLength);
      std::cerr << "Generated " << edges.size() << " edges" << std::endl;

      // Create HamiltonianPathProblem
      auto hamiltonianProblem = std::make_shared<HamiltonianPathProblem>(
          static_cast<int>(edges.size()), n, edges);

      // Run baseline algorithm
      BaselineAlgorithm baseline(hamiltonianProblem);
      auto baselineResult = baseline.run();

      // Reset problem and run local search
      hamiltonianProblem->reset();
      LocalSearchAlgorithm localSearch(hamiltonianProblem, timeLimit);
      auto localSearchResults = localSearch.run();

      AlgorithmResults results{baselineResult, localSearchResults};

      // Validate all solutions before outputting
      validate_hamiltonian_path(n, edges, baselineResult.getSolution());
      for (const auto &solution : localSearchResults) {
        validate_hamiltonian_path(n, edges, solution.getSolution());
      }

      // Build JSON output using helper functions
      auto graphJson = graphToJson(edges);
      auto output = buildOutputJson("HAMILTONIAN", graphJson, results);

      std::cout << output.dump() << std::endl;

    } else {
      std::cerr << "Invalid command or arguments" << std::endl;
      return 1;
    }

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
