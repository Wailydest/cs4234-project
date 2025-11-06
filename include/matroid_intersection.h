#ifndef MATROID_INTERSECTION_H
#define MATROID_INTERSECTION_H

#include "matroid_implementations.h"
#include <memory>
#include <set>
#include <vector>

class ApproximationSolution {
public:
  ApproximationSolution(double approximationRatio, std::vector<int> solution);
  double getApproximationRatio() const { return approximationRatio_; }
  const std::vector<int> &getSolution() const { return solution_; }

private:
  double approximationRatio_;
  std::vector<int> solution_;
};

// Baseline greedy algorithm: 1/k approximation
class BaselineAlgorithm {
public:
  BaselineAlgorithm(const std::shared_ptr<MatroidProblem> &matroidProblem);

  // Run the baseline algorithm
  ApproximationSolution run();

private:
  std::shared_ptr<MatroidProblem> matroidProblem_;
};

class Kuhn2dMatchingAlgorithm {
public:
  Kuhn2dMatchingAlgorithm(
      const std::shared_ptr<MatchingProblem> &matchingProblem);

  // Run the Hopcroft-Karp algorithm
  ApproximationSolution run();

private:
  std::shared_ptr<MatchingProblem> matchingProblem_;
};

// Local search algorithm: 2/(k+epsilon) approximation
class LocalSearchAlgorithm {
public:
  LocalSearchAlgorithm(const std::shared_ptr<MatroidProblem> &matroidProblem,
                       int timeLimitSeconds);

  // Run the local search algorithm
  std::vector<ApproximationSolution> run();

private:
  std::shared_ptr<MatroidProblem> matroidProblem_;
  int timeLimitSeconds_;
};

#endif // MATROID_INTERSECTION_H
