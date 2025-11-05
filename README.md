# CS4234 Project: Matroid Intersection Algorithms

Implementation and experimental evaluation of matroid intersection algorithms for CS4234 (Algorithms for Optimization).

## Overview

This project implements and compares two matroid intersection algorithms:

1. **Baseline Algorithm**: Greedy 1/k approximation
2. **Local Search Algorithm**: Lee-Sviridenko-Vondrák's 2/(k+ε) approximation using local search

## Problem Description

Given k matroids M₁, M₂, ..., Mₖ over the same ground set E, find a common independent set of maximum cardinality. This is known as the k-matroid intersection problem.

### Algorithms Implemented

#### 1. Baseline (1/k Approximation)
- Simple greedy approach
- Guarantees 1/k approximation ratio
- Fast and serves as baseline for comparison

#### 2. Local Search (2/(k+ε) Approximation)
- Based on Lee, Sviridenko, and Vondrák's algorithm
- Achieves 2/(k+ε) approximation for unweighted case
- Uses local search with configurable ε parameter
- Better approximation ratio with trade-off in runtime

## Project Structure

```
cs4234-project/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── requirements.txt        # Python dependencies
├── include/               # C++ header files
│   ├── matroid.h                      # Matroid interface
│   ├── matroid_intersection.h         # Algorithm declarations
│   ├── matroid_implementations.h      # Specific matroid types
│   └── graph_generator.h              # Graph generation utilities
├── src/                   # C++ implementation
│   ├── main.cpp                       # Main executable
│   ├── matroid_intersection.cpp       # Algorithm implementations
│   ├── matroid_implementations.cpp    # Matroid type implementations
│   └── graph_generator.cpp            # Graph generation
├── tests/                 # Python test framework
│   └── test_matroid.py               # Experimental evaluation
└── data/                  # Experimental data (generated)
```

## Building the Project

### Prerequisites

- C++ compiler with C++17 support (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.10 or higher
- Python 3.6 or higher

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make

# Optional: Install (installs to /usr/local/bin by default)
sudo make install
```

## Running Experiments

### Quick Start

Run all experiments with default parameters:

```bash
# From project root directory
python3 tests/test_matroid.py
```

### Manual Testing

The C++ executable can be run directly for individual experiments:

```bash
# Bipartite matching (k=2)
./build/matroid_intersection bipartite <n> <m> <p> <k> <epsilon> <seed>

# 3D matching (k=3)
./build/matroid_intersection 3dmatching <n> <p> <epsilon> <seed>

# Hamiltonian path (k=2)
./build/matroid_intersection hamiltonian <n> <p> <epsilon> <seed>
```

#### Parameters:
- `n`, `m`: Graph dimensions (number of vertices)
- `p`: Edge probability for Erdős-Rényi graph generation
- `k`: Number of matroids (2 or 3)
- `epsilon`: Approximation parameter (smaller = better approximation, more iterations)
- `seed`: Random seed for reproducibility

#### Example:

```bash
# Bipartite matching on 20x20 graph with p=0.3, k=2, ε=0.5
./build/matroid_intersection bipartite 20 20 0.3 2 0.5 42
```

Output format (JSON):
```json
{
  "edges": 120,
  "baseline_size": 15,
  "local_search_size": 18,
  "iterations": 23
}
```

## Experiments

The test framework runs three types of experiments:

### 1. Bipartite Matching (k=2)
- Tests on random Erdős-Rényi bipartite graphs
- Evaluates k=2 case (intersection of two matching matroids)
- Sensitivity analysis for different ε values

### 2. 3D Matching (k=3)
- Tests on 3-dimensional matching instances
- Evaluates k=3 case (intersection of three matroids)
- More challenging problem with k=3

### 3. Hamiltonian Path (k=2)
- Tests on random graphs for Hamiltonian path problem
- Uses intersection of graphic matroid and path matroid
- Demonstrates algorithm on different problem type

### Sensitivity Analysis

For each experiment, the framework evaluates multiple ε values:
- ε ∈ {0.1, 0.5, 1.0, 2.0}
- Smaller ε → better approximation ratio, more iterations
- Larger ε → faster convergence, slightly worse approximation

Each configuration is run multiple times with different random seeds to compute:
- Mean and standard deviation of solution sizes
- Improvement ratio (local search vs. baseline)
- Average number of iterations

## Matroid Types Implemented

### 1. Partition Matroid
Independent sets contain at most one element from each partition.

### 2. Uniform Matroid
Any set of size at most k is independent.

### 3. Bipartite Matching Matroid
Independent sets correspond to matchings in a bipartite graph.

### 4. Graphic Matroid
Independent sets are forests (acyclic edge sets).

### 5. Path Matroid
Independent sets form paths in a graph.

## Theoretical Background

### Approximation Ratios

For k-matroid intersection:
- **Baseline**: 1/k approximation (worst-case guarantee)
- **Local Search**: 2/(k+ε) approximation (Lee et al.)
- **Optimal**: OPT (polynomial time for k=2, NP-hard for k≥3)

### Performance Analysis

Expected performance:
- Local search improves upon baseline by approximately (2k)/(k+ε) factor
- For k=2, ε=1: improvement factor ≈ 1.33x
- For k=3, ε=1: improvement factor ≈ 1.5x

## Results Interpretation

The test framework reports:
- **Baseline size**: Solution size from 1/k approximation
- **Local search size**: Solution size from 2/(k+ε) approximation
- **Improvement ratio**: (Local search size) / (Baseline size)
- **Iterations**: Number of local search iterations until convergence

Expected observations:
1. Local search consistently finds larger solutions than baseline
2. Smaller ε leads to better solutions but more iterations
3. Improvement is more pronounced for larger k
4. Results vary with graph density (parameter p)

## References

- Lee, J., Sviridenko, M., & Vondrák, J. (2010). "Matroid Matching: The Power of Local Search"
- Oxley, J. (2011). "Matroid Theory" (2nd ed.)
- Schrijver, A. (2003). "Combinatorial Optimization: Polyhedra and Efficiency"

## License

This is a school project for educational purposes.

## Authors

CS4234 Course Project