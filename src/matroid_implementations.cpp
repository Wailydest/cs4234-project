#include "matroid_implementations.h"
#include <algorithm>
#include <cassert>
#include <queue>
#include <unordered_set>
#include <stdexcept>

// MatchingProblem implementation
MatchingProblem::MatchingProblem(int graphRank, int vertexPerPartitionCount,
                                 const std::vector<std::vector<int>> &edge_list)
    : MatroidProblem(static_cast<int>(edge_list.size()), graphRank),
      edges_(edge_list), vertexPerPartitionCount_(vertexPerPartitionCount) {

  // ensure all edges have the same rank
  for (const auto &edge : edge_list) {
    if (static_cast<int>(edge.size()) != graphRank) {
      throw std::invalid_argument("All edges must have the same rank");
    }
  }

  // For each partition, create a partition matroid set
  // The partition matroid ensures at most one edge containing each vertex is
  // selected
  std::vector<std::vector<int>> edge_to_vertices(matroidQuantity_);
  for (int edge_i = 0; edge_i < groundSetSize_; ++edge_i) {
    for (int p = 0; p < matroidQuantity_; ++p) {
      edge_to_vertices[p].push_back(edges_[edge_i][p]);
    }
  }

  for (int p = 0; p < matroidQuantity_; ++p) {
    matroids_.push_back(std::make_unique<PartitionMatroidSet>(
        groundSetSize_, vertexPerPartitionCount_, edge_to_vertices[p]));
  }
}

// PartitionMatroidSet implementation
MatchingProblem::PartitionMatroidSet::PartitionMatroidSet(
    int groundSetSize, int vertexPerPartitionCount,
    const std::vector<int> &edge_to_vertex)
    : groundSetSize_(groundSetSize),
      vertexPerPartitionCount_(vertexPerPartitionCount),
      edge_to_vertex_(edge_to_vertex) {
  is_vertex_used_ = std::vector<bool>(vertexPerPartitionCount_, false);
  is_edge_used_ = std::vector<bool>(groundSetSize_, false);
}

bool MatchingProblem::PartitionMatroidSet::tryAddElement(int element) {
  assert(element >= 0 && element < groundSetSize_);
  int vertex = edge_to_vertex_[element];
  if (vertex < 0 || vertex >= vertexPerPartitionCount_) {
    throw std::invalid_argument("Vertex index out of bounds");
  }

  if (is_edge_used_[element]) {
    throw std::invalid_argument("Edge already used");
  }

  if (is_vertex_used_[vertex]) {
    return false;
  }
  is_vertex_used_[vertex] = true;
  is_edge_used_[element] = true;
  return true;
}

void MatchingProblem::PartitionMatroidSet::removeElement(int element) {
  assert(element >= 0 && element < groundSetSize_);
  int vertex = edge_to_vertex_[element];
  if (vertex < 0 || vertex >= vertexPerPartitionCount_) {
    throw std::invalid_argument("Vertex index out of bounds");
  }
  if (!is_edge_used_[element]) {
    throw std::invalid_argument("Edge not used");
  }

  is_vertex_used_[vertex] = false;
  is_edge_used_[element] = false;
}

// HamiltonianPathProblem implementation
HamiltonianPathProblem::HamiltonianPathProblem(
    int groundSetSize, int vertexCount,
    const std::vector<std::pair<int, int>> &edges)
    : MatroidProblem(groundSetSize, 3), edges_(edges) {
  matroids_.push_back(
      std::make_unique<HamiltonianPathProblem::SingleIncomingEdgeMatroidSet>(
          groundSetSize, vertexCount, edges, true));
  matroids_.push_back(
      std::make_unique<HamiltonianPathProblem::SingleIncomingEdgeMatroidSet>(
          groundSetSize, vertexCount, edges, false));
  matroids_.push_back(
      std::make_unique<HamiltonianPathProblem::GraphicMatroidSet>(
          groundSetSize, vertexCount, edges));
}

HamiltonianPathProblem::SingleIncomingEdgeMatroidSet::
    SingleIncomingEdgeMatroidSet(int groundSetSize, int vertexCount,
                                 const std::vector<std::pair<int, int>> &edges,
                                 bool is_incoming)
    : vertexCount_(vertexCount), groundSetSize_(groundSetSize) {
  edge_to_ = std::vector<int>(groundSetSize_);
  is_vertex_used_ = std::vector<bool>(vertexCount_, false);
  for (int i = 0; i < groundSetSize_; i++) {
    edge_to_[i] = is_incoming ? edges[i].second : edges[i].first;
  }
}

bool HamiltonianPathProblem::SingleIncomingEdgeMatroidSet::tryAddElement(
    int element) {
  assert(element >= 0 && element < groundSetSize_);
  if (is_vertex_used_[edge_to_[element]]) {
    return false;
  }
  is_vertex_used_[edge_to_[element]] = true;
  return true;
}

void HamiltonianPathProblem::SingleIncomingEdgeMatroidSet::removeElement(
    int element) {
  assert(element >= 0 && element < groundSetSize_);
  if (!is_vertex_used_[edge_to_[element]]) {
    throw std::invalid_argument("Edge can't be present in the set if the "
                                "corresponding vertex is not used");
  }
  is_vertex_used_[edge_to_[element]] = false;
}

HamiltonianPathProblem::GraphicMatroidSet::GraphicMatroidSet(
    int groundSetSize, int vertexCount,
    const std::vector<std::pair<int, int>> &edges)
    : vertexCount_(vertexCount), groundSetSize_(groundSetSize) {
  next_ = std::vector<int>(vertexCount_, -1);
  edges_ = edges;
}

bool HamiltonianPathProblem::GraphicMatroidSet::tryAddElement(int element) {
  assert(element >= 0 && element < groundSetSize_);
  int vertex = edges_[element].second;
  int count = 0;
  while (next_[vertex] != -1) {
    vertex = next_[vertex];
    ++count;
    if (count > vertexCount_) {
      throw std::runtime_error("Cycle detected");
    }
  }
  if (vertex == edges_[element].first) {
    // the edge would form a cycle
    return false;
  }
  next_[edges_[element].first] = edges_[element].second;
  return true;
}

void HamiltonianPathProblem::GraphicMatroidSet::removeElement(int element) {
  assert(element >= 0 && element < groundSetSize_);
  if (next_[edges_[element].first] != edges_[element].second) {
    throw std::invalid_argument("Edge not found");
  }
  next_[edges_[element].first] = -1;
}