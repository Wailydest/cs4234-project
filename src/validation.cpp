#include "validation.h"
#include <algorithm>
#include <utility>
#include <vector>

void check_solution_set(int groundSetSize, const std::vector<int> &solution) {
  std::vector<int> sorted_solution = solution;
  std::sort(sorted_solution.begin(), sorted_solution.end());
  for (int i = 0; i < sorted_solution.size(); i++) {
    if (i && sorted_solution[i] == sorted_solution[i - 1]) {
      throw std::invalid_argument(
          "Solution error: Duplicate element in solution");
    }
    if (sorted_solution[i] < 0 || sorted_solution[i] >= groundSetSize) {
      throw std::invalid_argument("Solution error: Element out of bounds");
    }
  }
}

void validate_bipartite_matching(int n,
                                 const std::vector<std::pair<int, int>> &edges,
                                 const std::vector<int> &solution) {
  for (const auto &edge : edges) {
    if (edge.first < 0 || edge.first >= n || edge.second < 0 ||
        edge.second >= n) {
      throw std::invalid_argument(
          "Input failed validation: Edge out of bounds");
    }
  }
  check_solution_set(edges.size(), solution);
  std::vector<std::vector<bool>> used_vertices(2, std::vector<bool>(n, false));
  for (int i = 0; i < solution.size(); i++) {
    if (used_vertices[0][edges[solution[i]].first]) {
      throw std::invalid_argument("Solution error: Edge already used");
    }
    if (used_vertices[1][edges[solution[i]].second]) {
      throw std::invalid_argument("Solution error: Edge already used");
    }
    used_vertices[0][edges[solution[i]].first] = true;
    used_vertices[1][edges[solution[i]].second] = true;
  }
}

void validate_3d_matching(int n, const std::vector<std::vector<int>> &edges,
                          const std::vector<int> &solution) {
  // Validate edges are within bounds and have correct structure
  for (const auto &edge : edges) {
    if (edge.size() != 3) {
      throw std::invalid_argument(
          "Input failed validation: Edge must have exactly 3 vertices");
    }
    for (int i = 0; i < 3; i++) {
      if (edge[i] < 0 || edge[i] >= n) {
        throw std::invalid_argument(
            "Input failed validation: Edge vertex out of bounds");
      }
    }
  }
  check_solution_set(edges.size(), solution);
  // Track used vertices in each of the 3 partitions
  std::vector<std::vector<bool>> used_vertices(3, std::vector<bool>(n, false));
  for (int i = 0; i < solution.size(); i++) {
    const auto &edge = edges[solution[i]];
    for (int p = 0; p < 3; p++) {
      if (used_vertices[p][edge[p]]) {
        throw std::invalid_argument("Solution error: Vertex already used");
      }
      used_vertices[p][edge[p]] = true;
    }
  }
}

void validate_hamiltonian_path(int n,
                               const std::vector<std::pair<int, int>> &edges,
                               const std::vector<int> &solution) {
  // Validate edges are within bounds
  for (const auto &edge : edges) {
    if (edge.first < 0 || edge.first >= n || edge.second < 0 ||
        edge.second >= n) {
      throw std::invalid_argument(
          "Input failed validation: Edge out of bounds");
    }
  }
  check_solution_set(edges.size(), solution);

  // Empty solution is valid
  if (solution.empty()) {
    return;
  }

  // Track incoming and outgoing edges for each vertex
  // Use -1 to indicate no edge
  std::vector<int> incoming(n, -1); // vertex -> edge index
  std::vector<int> outgoing(n, -1); // vertex -> edge index

  // Build adjacency information and check constraints
  for (int edge_idx : solution) {
    const auto &edge = edges[edge_idx];
    int from_vertex = edge.first;
    int to_vertex = edge.second;

    // Check incoming edge constraint (each vertex can have at most one incoming
    // edge)
    if (incoming[to_vertex] != -1) {
      throw std::invalid_argument(
          "Solution error: Vertex has multiple incoming edges");
    }
    incoming[to_vertex] = edge_idx;

    // Check outgoing edge constraint (each vertex can have at most one outgoing
    // edge)
    if (outgoing[from_vertex] != -1) {
      throw std::invalid_argument(
          "Solution error: Vertex has multiple outgoing edges");
    }
    outgoing[from_vertex] = edge_idx;
  }

  // Check for cycles
  std::vector<int> visited(n, 0);
  int iter = 1;
  for (int v = 0; v < n; ++v) {
    if (!visited[v]) {
      int cur = v;
      while (outgoing[cur] != -1) {
        cur = edges[outgoing[cur]].second;
        if (visited[cur]) {
          if (visited[cur] == iter) {
            throw std::invalid_argument("Solution error: Cycle detected");
          }
          break;
        }
        visited[cur] = iter;
      }
      ++iter;
    }
  }
}