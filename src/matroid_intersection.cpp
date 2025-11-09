#include "matroid_intersection.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

ApproximationSolution::ApproximationSolution(double approximationRatio,
                                             std::vector<int> solution)
    : approximationRatio_(approximationRatio), solution_(std::move(solution)) {}

BaselineAlgorithm::BaselineAlgorithm(
    const std::shared_ptr<MatroidProblem> &matroidProblem)
    : matroidProblem_(matroidProblem) {}

ApproximationSolution BaselineAlgorithm::run() {
  // 1/k approximation: greedily add elements that maintain independence
  std::vector<int> solution;

  for (int element = 0; element < matroidProblem_->getGroundSetSize();
       element++) {
    if (matroidProblem_->tryAddElement(element)) {
      solution.push_back(element);
    }
  }

  return ApproximationSolution(1.0 / matroidProblem_->getMatroidQuantity(),
                               solution);
}

Kuhn2dMatchingAlgorithm::Kuhn2dMatchingAlgorithm(
    const std::shared_ptr<MatchingProblem> &matchingProblem)
    : matchingProblem_(matchingProblem) {
  // ensure that the graph is bipartite
  if (matchingProblem_->getMatroidQuantity() != 2) {
    throw std::invalid_argument("Graph must be bipartite");
  }
}

ApproximationSolution Kuhn2dMatchingAlgorithm::run() {
  // Hopcroft-Karp algorithm: find a maximum matching in a bipartite graph
  std::vector<int> solution;
  int n = matchingProblem_->getVertexPerPartitionCount();

  // store graph as edge list for each vertex in the first partition; holding
  // <vertex, edge_index>
  std::vector<std::vector<std::pair<int, int>>> graph(n);
  const auto &edges = matchingProblem_->getEdges();
  for (size_t edge_i = 0; edge_i < edges.size(); edge_i++) {
    graph[edges[edge_i][0]].push_back({edges[edge_i][1], edge_i});
  }

  // run Hopcroft-Karp algorithm
  std::vector<int> match_vertex(n, -1); // right partition
  std::vector<int> match_edge(
      n, -1); // for matching recovery purposes, right partition
  std::vector<bool> isVisited(n, false); // left partition
  std::vector<bool> isMatched(n, false); // left partition

  // recursive dfs function

  auto dfs = [&](auto self, int v) -> bool {
    if (isVisited[v])
      return false;
    isVisited[v] = true;
    for (auto [u, edge_i] : graph[v]) {
      if (match_vertex[u] == -1 || self(self, match_vertex[u])) {
        match_vertex[u] = v;
        match_edge[u] = edge_i;
        isMatched[v] = true;
        return true;
      }
    }
    return false;
  };

  bool any;
  do {
    any = false;
    std::fill(isVisited.begin(), isVisited.end(), false);
    for (int v = 0; v < n; v++) {
      if (!isVisited[v] && !isMatched[v] && dfs(dfs, v))
        any = true;
    }
  } while (any);
  for (int v = 0; v < n; v++) {
    if (match_vertex[v] != -1) {
      solution.push_back(match_edge[v]);
    }
  }
  return ApproximationSolution(1.0, solution);
}

LocalSearchAlgorithm::LocalSearchAlgorithm(
    const std::shared_ptr<MatroidProblem> &matroidProblem, int timeLimitSeconds)
    : matroidProblem_(matroidProblem), timeLimitSeconds_(timeLimitSeconds) {}

double computeApproximationRatio(int s, int k) {
  if (s == 0)
    return 1.0 / k;
  if (k == 2)
    return (double)(s + 1) / (s + 2);
  // for k == 3
  if (k != 3)
    throw std::invalid_argument(
        "Approximation ratio not supported for k != 2 or k != 3");
  return 2.0 / (3 + 2 * pow(s, -0.3562)); // the formula to be confirmed
}

std::vector<int> convertMaskToSolution(const std::vector<bool> &solutionMask) {
  std::vector<int> solution;
  for (int i = 0; i < (int) solutionMask.size(); i++) {
    if (solutionMask[i]) {
      solution.push_back(i);
    }
  }
  return solution;
}

std::vector<ApproximationSolution> LocalSearchAlgorithm::run() {
  std::vector<ApproximationSolution> solutions;
  int edgesCount = matroidProblem_->getGroundSetSize();
  std::vector<bool> solutionMask(edgesCount, false);
  std::vector<bool> justRemoved(edgesCount, false);
  auto startTime = std::chrono::steady_clock::now();
  bool timeLimitExceeded = false;

  auto checkTimeLimit = [&]() -> bool {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                              currentTime - startTime)
                              .count();
    if (elapsedSeconds >= timeLimitSeconds_) {
      timeLimitExceeded = true;
      return true;
    }
    return false;
  };

  auto addElements = [&](auto self, int idx, int addQuantity) -> bool {
    if (checkTimeLimit()) {
      return false;
    }
    if (addQuantity == 0) {
      return true;
    }
    if (idx == edgesCount) {
      return false;
    }
    if (justRemoved[idx] || solutionMask[idx])
      return self(self, idx + 1, addQuantity);
    if (matroidProblem_->tryAddElement(idx)) {
      solutionMask[idx] = true;
      if (self(self, idx + 1, addQuantity - 1))
        return true;
      matroidProblem_->removeElement(idx);
      solutionMask[idx] = false;
    }
    return self(self, idx + 1, addQuantity);
  };
  auto removeAndAddElements = [&](auto self, int idx, int removeQuantity,
                                  int addQuantity) -> bool {
    if (checkTimeLimit()) {
      return false;
    }
    if (removeQuantity == 0)
      return addElements(addElements, 0, addQuantity);
    if (idx == edgesCount) {
      return false;
    }
    if (solutionMask[idx]) {
      matroidProblem_->removeElement(idx);
      solutionMask[idx] = false;
      justRemoved[idx] = true;
      if (self(self, idx + 1, removeQuantity - 1, addQuantity))
        return true;
      justRemoved[idx] = false;
      if (!matroidProblem_->tryAddElement(idx))
        throw std::runtime_error("Failed to add element");
      solutionMask[idx] = true;
    }
    return self(self, idx + 1, removeQuantity, addQuantity);
  };

  int solutionSize = 0;
  for (int s = 0;; ++s) {
    // Check time limit before starting a new step
    if (checkTimeLimit()) {
      std::cerr << "Time limit of " << timeLimitSeconds_
                << " seconds reached at step " << s << std::endl;
      break;
    }
    // trying to remove s and add s+1 elements
    bool success;
    do {
      if (timeLimitExceeded) {
        break;
      }
      success = false;
      std::fill(justRemoved.begin(), justRemoved.end(), false);
      for (int i = 0; i <= s; ++i) {
        if (timeLimitExceeded) {
          break;
        }
        if (removeAndAddElements(removeAndAddElements, 0, i, i + 1)) {
          ++solutionSize;
          success = true;
          break;
        }
      }
      if (s == solutionSize) {
        // achieved the maximum possible solution size
        break;
      }
    } while (success && !timeLimitExceeded);
    if (timeLimitExceeded) {
      std::cerr << "Time limit of " << timeLimitSeconds_
                << " seconds reached at step " << s << std::endl;
      std::cerr << "Solution size: " << solutionSize << std::endl;
      double ratio;
      if (s == 0)
        ratio = 0.0;
      else
        ratio = computeApproximationRatio(
            s - 1, matroidProblem_->getMatroidQuantity());
      std::cerr << "Approximation ratio: " << ratio << std::endl;
      solutions.push_back(
          ApproximationSolution(ratio, convertMaskToSolution(solutionMask)));
      break;
    } else {
      // we can provide a guaranteed approximation ratio at this step for s
      double ratio =
          computeApproximationRatio(s, matroidProblem_->getMatroidQuantity());
      if (s == solutionSize) {
        ratio = 1.0;
      }
      solutions.push_back(
          ApproximationSolution(ratio, convertMaskToSolution(solutionMask)));
      if (s == solutionSize)
        break;
    }
    std::cerr << "At step " << s << " we found a solution of size "
              << solutionSize << std::endl;
  }
  return solutions;
}