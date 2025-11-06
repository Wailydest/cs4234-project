#ifndef MATROID_IMPLEMENTATIONS_H
#define MATROID_IMPLEMENTATIONS_H

#include "matroid_problem.h"
#include <map>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

// Matching problem: find a matching in a hypergraph
class MatchingProblem : public MatroidProblem {
public:
  // edge_list[i] is the list of vertices in edge i, and groundSetSize is the
  // number of vertices
  MatchingProblem(int graphRank, int vertexPerPartitionCount,
                  const std::vector<std::vector<int>> &edge_list);

  // Get the edges of the matching problem
  const std::vector<std::vector<int>> &getEdges() const { return edges_; }

  int getVertexPerPartitionCount() const { return vertexPerPartitionCount_; }

private:
  std::vector<std::vector<int>> edges_;
  int vertexPerPartitionCount_;

  // Correponds to each part of the multipartite graph
  class PartitionMatroidSet : public MatroidSet {
  public:
    PartitionMatroidSet(int groundSetSize, int vertexPerPartitionCount,
                        const std::vector<int> &edge_to_vertex);

    bool tryAddElement(int element) override;
    void removeElement(int element) override;

  private:
    int groundSetSize_;           // number of edges in the ground set
    int vertexPerPartitionCount_; // number of vertices in each partition
    std::vector<int>
        edge_to_vertex_; // for each edge, the index of the vertex it belongs to
    std::vector<bool> is_vertex_used_; // true if the vertex is used
    std::vector<bool> is_edge_used_;   // true if the edge is used
  };
};

class HamiltonianPathProblem : public MatroidProblem {
public:
  HamiltonianPathProblem(int groundSetSize, int vertexCount,
                         const std::vector<std::pair<int, int>> &edges);

  const std::vector<std::pair<int, int>> &getEdges() const { return edges_; }

  // using single class for both incoming and outgoing edges because they are
  // symmetric
  class SingleIncomingEdgeMatroidSet : public MatroidSet {
  public:
    SingleIncomingEdgeMatroidSet(int groundSetSize, int vertexCount,
                                 const std::vector<std::pair<int, int>> &edges,
                                 bool is_incoming);
    bool tryAddElement(int element) override;
    void removeElement(int element) override;

  private:
    int vertexCount_;
    int groundSetSize_;
    std::vector<int> edge_to_;         // [E]
    std::vector<bool> is_vertex_used_; // [V]
  };
  class GraphicMatroidSet : public MatroidSet {
  public:
    GraphicMatroidSet(int groundSetSize, int vertexCount,
                      const std::vector<std::pair<int, int>> &edges);
    bool tryAddElement(int element) override;
    void removeElement(int element) override;

  private:
    int vertexCount_;
    int groundSetSize_;
    std::vector<int> next_;                  // [V]
    std::vector<std::pair<int, int>> edges_; // [E]
  };

private:
  std::vector<std::pair<int, int>> edges_;
};

#endif // MATROID_IMPLEMENTATIONS_H