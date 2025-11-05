#include "matroid_intersection.h"
#include <algorithm>
#include <random>
#include <iostream>

BaselineAlgorithm::BaselineAlgorithm(const std::vector<std::shared_ptr<Matroid>>& matroids)
    : matroids_(matroids) {}

std::set<int> BaselineAlgorithm::run() {
    // 1/k approximation: greedily add elements that maintain independence
    std::set<int> solution;
    
    if (matroids_.empty()) {
        return solution;
    }
    
    std::vector<int> groundSet = matroids_[0]->getGroundSet();
    
    for (int element : groundSet) {
        std::set<int> candidate = solution;
        candidate.insert(element);
        
        // Check if candidate is independent in all matroids
        bool independent = true;
        for (const auto& matroid : matroids_) {
            if (!matroid->isIndependent(candidate)) {
                independent = false;
                break;
            }
        }
        
        if (independent) {
            solution = candidate;
        }
    }
    
    return solution;
}

LocalSearchAlgorithm::LocalSearchAlgorithm(
    const std::vector<std::shared_ptr<Matroid>>& matroids,
    double epsilon,
    int maxIterations)
    : matroids_(matroids), epsilon_(epsilon), iterations_(0), maxIterations_(maxIterations) {}

bool LocalSearchAlgorithm::allIndependent(const std::set<int>& S) const {
    for (const auto& matroid : matroids_) {
        if (!matroid->isIndependent(S)) {
            return false;
        }
    }
    return true;
}

bool LocalSearchAlgorithm::improve(std::set<int>& current) {
    if (matroids_.empty()) {
        return false;
    }
    
    std::vector<int> groundSet = matroids_[0]->getGroundSet();
    
    // Try simple additions first (0-exchange)
    for (int elem : groundSet) {
        if (current.find(elem) == current.end()) {
            std::set<int> candidate = current;
            candidate.insert(elem);
            
            if (allIndependent(candidate) && candidate.size() > current.size()) {
                current = candidate;
                return true;
            }
        }
    }
    
    // Try 1-exchange: remove 1, add 1 or 2
    std::vector<int> currentVec(current.begin(), current.end());
    for (int removeIdx = 0; removeIdx < (int)currentVec.size(); ++removeIdx) {
        std::set<int> reduced = current;
        reduced.erase(currentVec[removeIdx]);
        
        // Try adding 1 element
        for (int elem : groundSet) {
            if (reduced.find(elem) == reduced.end()) {
                std::set<int> candidate = reduced;
                candidate.insert(elem);
                
                if (allIndependent(candidate) && candidate.size() > current.size()) {
                    current = candidate;
                    return true;
                }
            }
        }
        
        // Try adding 2 elements
        std::vector<int> candidates;
        for (int elem : groundSet) {
            if (reduced.find(elem) == reduced.end()) {
                candidates.push_back(elem);
            }
        }
        
        for (size_t i = 0; i < candidates.size(); ++i) {
            for (size_t j = i + 1; j < candidates.size(); ++j) {
                std::set<int> candidate = reduced;
                candidate.insert(candidates[i]);
                candidate.insert(candidates[j]);
                
                if (allIndependent(candidate) && candidate.size() > current.size()) {
                    current = candidate;
                    return true;
                }
            }
        }
    }
    
    // Try 2-exchange: remove 2, add 2 or 3
    if (currentVec.size() >= 2) {
        for (size_t i = 0; i < currentVec.size(); ++i) {
            for (size_t j = i + 1; j < currentVec.size(); ++j) {
                std::set<int> reduced = current;
                reduced.erase(currentVec[i]);
                reduced.erase(currentVec[j]);
                
                std::vector<int> candidates;
                for (int elem : groundSet) {
                    if (reduced.find(elem) == reduced.end()) {
                        candidates.push_back(elem);
                    }
                }
                
                // Try adding 2 or 3 elements
                for (size_t a = 0; a < candidates.size(); ++a) {
                    for (size_t b = a + 1; b < candidates.size(); ++b) {
                        std::set<int> candidate = reduced;
                        candidate.insert(candidates[a]);
                        candidate.insert(candidates[b]);
                        
                        if (allIndependent(candidate) && candidate.size() > current.size()) {
                            current = candidate;
                            return true;
                        }
                        
                        // Try adding third element (limited search for efficiency)
                        const size_t maxCandidates = 10; // Limit search space for performance
                        for (size_t c = b + 1; c < std::min(candidates.size(), b + maxCandidates); ++c) {
                            std::set<int> candidate3 = candidate;
                            candidate3.insert(candidates[c]);
                            
                            if (allIndependent(candidate3) && candidate3.size() > current.size()) {
                                current = candidate3;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

std::set<int> LocalSearchAlgorithm::run() {
    // Start with baseline solution
    BaselineAlgorithm baseline(matroids_);
    std::set<int> solution = baseline.run();
    
    iterations_ = 0;
    
    // Local search improvement
    while (iterations_ < maxIterations_) {
        iterations_++;
        
        if (!improve(solution)) {
            break; // No improvement found
        }
    }
    
    return solution;
}
