#ifndef VALIDATION_H
#define VALIDATION_H

#include <utility>
#include <vector>

void validate_bipartite_matching(int n,
                                 const std::vector<std::pair<int, int>> &edges,
                                 const std::vector<int> &solution);

void validate_3d_matching(int n, const std::vector<std::vector<int>> &edges,
                          const std::vector<int> &solution);

void validate_hamiltonian_path(int n,
                               const std::vector<std::pair<int, int>> &edges,
                               const std::vector<int> &solution);

#endif // VALIDATION_H