#ifndef MATROID_IMPLEMENTATIONS_H
#define MATROID_IMPLEMENTATIONS_H

#include "matroid.h"
#include <vector>
#include <set>
#include <map>

// Partition Matroid: Elements partitioned into groups, at most one from each group
class PartitionMatroid : public Matroid {
public:
    PartitionMatroid(int groundSetSize, const std::vector<std::vector<int>>& partitions);
    
    bool isIndependent(const std::set<int>& S) const override;
    int getGroundSetSize() const override { return groundSetSize_; }
    std::vector<int> getGroundSet() const override;
    
private:
    int groundSetSize_;
    std::map<int, int> elementToPartition_; // element -> partition index
};

// Uniform Matroid: Any set of size at most k is independent
class UniformMatroid : public Matroid {
public:
    UniformMatroid(int groundSetSize, int k);
    
    bool isIndependent(const std::set<int>& S) const override;
    int getGroundSetSize() const override { return groundSetSize_; }
    std::vector<int> getGroundSet() const override;
    
private:
    int groundSetSize_;
    int k_;
};

// Bipartite Matching Matroid: Independent sets correspond to matchings
class BipartiteMatchingMatroid : public Matroid {
public:
    BipartiteMatchingMatroid(int n, int m, const std::vector<std::pair<int, int>>& edges);
    
    bool isIndependent(const std::set<int>& S) const override;
    int getGroundSetSize() const override { return edges_.size(); }
    std::vector<int> getGroundSet() const override;
    
private:
    int n_, m_; // left and right partition sizes
    std::vector<std::pair<int, int>> edges_;
    
    // Check if edge indices form a matching
    bool isMatching(const std::set<int>& edgeIndices) const;
};

// Graphic Matroid for Hamiltonian Path: Independent sets are forests
class GraphicMatroid : public Matroid {
public:
    GraphicMatroid(int n, const std::vector<std::pair<int, int>>& edges);
    
    bool isIndependent(const std::set<int>& S) const override;
    int getGroundSetSize() const override { return edges_.size(); }
    std::vector<int> getGroundSet() const override;
    
private:
    int n_; // number of vertices
    std::vector<std::pair<int, int>> edges_;
    
    // Check if edge set forms a forest (no cycles)
    bool isForest(const std::set<int>& edgeIndices) const;
};

// Path Matroid: Elements form a path
class PathMatroid : public Matroid {
public:
    PathMatroid(int n, const std::vector<std::pair<int, int>>& edges);
    
    bool isIndependent(const std::set<int>& S) const override;
    int getGroundSetSize() const override { return edges_.size(); }
    std::vector<int> getGroundSet() const override;
    
private:
    int n_; // number of vertices
    std::vector<std::pair<int, int>> edges_;
    
    // Check if edge set forms a path
    bool isPath(const std::set<int>& edgeIndices) const;
};

#endif // MATROID_IMPLEMENTATIONS_H
