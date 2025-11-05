#ifndef MATROID_INTERSECTION_H
#define MATROID_INTERSECTION_H

#include "matroid.h"
#include <vector>
#include <set>
#include <memory>

// Baseline algorithm: 1/k approximation
class BaselineAlgorithm {
public:
    BaselineAlgorithm(const std::vector<std::shared_ptr<Matroid>>& matroids);
    
    // Run the baseline algorithm
    std::set<int> run();
    
private:
    std::vector<std::shared_ptr<Matroid>> matroids_;
};

// Local search algorithm: 2/(k+epsilon) approximation
class LocalSearchAlgorithm {
public:
    LocalSearchAlgorithm(const std::vector<std::shared_ptr<Matroid>>& matroids, 
                        double epsilon, 
                        int maxIterations = 100);
    
    // Run the local search algorithm
    std::set<int> run();
    
    // Get number of iterations performed
    int getIterations() const { return iterations_; }
    
    // Set maximum iterations
    void setMaxIterations(int maxIter) { maxIterations_ = maxIter; }
    
private:
    std::vector<std::shared_ptr<Matroid>> matroids_;
    double epsilon_;
    int iterations_;
    int maxIterations_;
    
    // Check if all matroids are independent on a set
    bool allIndependent(const std::set<int>& S) const;
    
    // Try to improve solution by local search
    bool improve(std::set<int>& current);
};

#endif // MATROID_INTERSECTION_H
