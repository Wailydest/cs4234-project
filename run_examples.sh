#!/bin/bash
# Example script to demonstrate matroid intersection algorithms

echo "Building the project..."
mkdir -p build
cd build
cmake ..
make
cd ..

echo ""
echo "================================================"
echo "Running Example Experiments"
echo "================================================"
echo ""

echo "1. Bipartite Matching (k=2, ε=0.5):"
./build/matroid_intersection bipartite 15 15 0.3 2 0.5 42

echo ""
echo "2. 3D Matching (k=3, ε=0.5):"
./build/matroid_intersection 3dmatching 10 0.3 0.5 42

echo ""
echo "3. Hamiltonian Path (k=2, ε=0.5):"
./build/matroid_intersection hamiltonian 12 0.4 0.5 42

echo ""
echo "================================================"
echo "Running Full Test Suite"
echo "================================================"
echo ""
python3 tests/test_matroid.py
