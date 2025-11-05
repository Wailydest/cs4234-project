#include "matroid_implementations.h"
#include <algorithm>
#include <queue>
#include <unordered_set>

// PartitionMatroid implementation
PartitionMatroid::PartitionMatroid(int groundSetSize, const std::vector<std::vector<int>>& partitions)
    : groundSetSize_(groundSetSize) {
    for (size_t i = 0; i < partitions.size(); ++i) {
        for (int elem : partitions[i]) {
            elementToPartition_[elem] = i;
        }
    }
}

bool PartitionMatroid::isIndependent(const std::set<int>& S) const {
    std::unordered_set<int> usedPartitions;
    for (int elem : S) {
        auto it = elementToPartition_.find(elem);
        if (it == elementToPartition_.end()) {
            return false; // element not in ground set
        }
        int partition = it->second;
        if (usedPartitions.count(partition)) {
            return false; // two elements from same partition
        }
        usedPartitions.insert(partition);
    }
    return true;
}

std::vector<int> PartitionMatroid::getGroundSet() const {
    std::vector<int> result;
    for (const auto& pair : elementToPartition_) {
        result.push_back(pair.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

// UniformMatroid implementation
UniformMatroid::UniformMatroid(int groundSetSize, int k)
    : groundSetSize_(groundSetSize), k_(k) {}

bool UniformMatroid::isIndependent(const std::set<int>& S) const {
    return (int)S.size() <= k_;
}

std::vector<int> UniformMatroid::getGroundSet() const {
    std::vector<int> result;
    for (int i = 0; i < groundSetSize_; ++i) {
        result.push_back(i);
    }
    return result;
}

// BipartiteMatchingMatroid implementation
BipartiteMatchingMatroid::BipartiteMatchingMatroid(
    int n, int m, const std::vector<std::pair<int, int>>& edges)
    : n_(n), m_(m), edges_(edges) {}

bool BipartiteMatchingMatroid::isMatching(const std::set<int>& edgeIndices) const {
    std::unordered_set<int> usedLeft, usedRight;
    for (int idx : edgeIndices) {
        if (idx < 0 || idx >= (int)edges_.size()) {
            return false;
        }
        int left = edges_[idx].first;
        int right = edges_[idx].second;
        
        if (usedLeft.count(left) || usedRight.count(right)) {
            return false; // vertex already matched
        }
        usedLeft.insert(left);
        usedRight.insert(right);
    }
    return true;
}

bool BipartiteMatchingMatroid::isIndependent(const std::set<int>& S) const {
    return isMatching(S);
}

std::vector<int> BipartiteMatchingMatroid::getGroundSet() const {
    std::vector<int> result;
    for (size_t i = 0; i < edges_.size(); ++i) {
        result.push_back(i);
    }
    return result;
}

// Union-Find for GraphicMatroid
class UnionFind {
public:
    UnionFind(int n) : parent_(n), rank_(n, 0) {
        for (int i = 0; i < n; ++i) {
            parent_[i] = i;
        }
    }
    
    int find(int x) {
        if (parent_[x] != x) {
            parent_[x] = find(parent_[x]);
        }
        return parent_[x];
    }
    
    bool unite(int x, int y) {
        int px = find(x);
        int py = find(y);
        if (px == py) return false;
        
        if (rank_[px] < rank_[py]) {
            parent_[px] = py;
        } else if (rank_[px] > rank_[py]) {
            parent_[py] = px;
        } else {
            parent_[py] = px;
            rank_[px]++;
        }
        return true;
    }
    
private:
    std::vector<int> parent_;
    std::vector<int> rank_;
};

// GraphicMatroid implementation
GraphicMatroid::GraphicMatroid(int n, const std::vector<std::pair<int, int>>& edges)
    : n_(n), edges_(edges) {}

bool GraphicMatroid::isForest(const std::set<int>& edgeIndices) const {
    UnionFind uf(n_);
    for (int idx : edgeIndices) {
        if (idx < 0 || idx >= (int)edges_.size()) {
            return false;
        }
        int u = edges_[idx].first;
        int v = edges_[idx].second;
        
        if (!uf.unite(u, v)) {
            return false; // creates a cycle
        }
    }
    return true;
}

bool GraphicMatroid::isIndependent(const std::set<int>& S) const {
    return isForest(S);
}

std::vector<int> GraphicMatroid::getGroundSet() const {
    std::vector<int> result;
    for (size_t i = 0; i < edges_.size(); ++i) {
        result.push_back(i);
    }
    return result;
}

// PathMatroid implementation
PathMatroid::PathMatroid(int n, const std::vector<std::pair<int, int>>& edges)
    : n_(n), edges_(edges) {}

bool PathMatroid::isPath(const std::set<int>& edgeIndices) const {
    if (edgeIndices.empty()) {
        return true;
    }
    
    // Build adjacency list
    std::vector<std::vector<int>> adj(n_);
    for (int idx : edgeIndices) {
        if (idx < 0 || idx >= (int)edges_.size()) {
            return false;
        }
        int u = edges_[idx].first;
        int v = edges_[idx].second;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    // Check degree: at most 2 vertices can have degree > 1
    int endpointCount = 0;
    int startVertex = -1;
    for (int i = 0; i < n_; ++i) {
        if (adj[i].size() > 2) {
            return false; // not a path
        }
        if (adj[i].size() == 1) {
            endpointCount++;
            startVertex = i;
        } else if (adj[i].size() > 0 && startVertex == -1) {
            startVertex = i;
        }
    }
    
    if (endpointCount > 2) {
        return false;
    }
    
    // Check connectivity: all edges form a single connected component
    if (startVertex == -1) {
        return true; // no edges
    }
    
    std::vector<bool> visited(n_, false);
    std::queue<int> q;
    q.push(startVertex);
    visited[startVertex] = true;
    int visitedCount = 0;
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        visitedCount++;
        
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    
    // Count vertices with non-zero degree
    int nonZeroDegree = 0;
    for (int i = 0; i < n_; ++i) {
        if (adj[i].size() > 0) {
            nonZeroDegree++;
        }
    }
    
    return visitedCount == nonZeroDegree;
}

bool PathMatroid::isIndependent(const std::set<int>& S) const {
    return isPath(S);
}

std::vector<int> PathMatroid::getGroundSet() const {
    std::vector<int> result;
    for (size_t i = 0; i < edges_.size(); ++i) {
        result.push_back(i);
    }
    return result;
}
