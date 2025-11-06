# CS4234 Project: Matroid Intersection Algorithms

The code is a C++ implementation of matroid intersection applied on various problems.

## Algorithm description

* The baseline is a greedy algorithm that goes through each of the ground set ellements and attepts adding them to the solution if they maintain independence for all matroids.
  * Achieves 1/k approximation
* For 2d matroids, Hopcroft-Karp achieves $1.0$ approximation on bipartite matching problem.
* Local search algorithm is the algorithm described in the Lee et. al. paper
  * The basic idea: at each step we try to remove $s$ elements and add $s+1$ elements to the solution
  * The paper claims that if it's impossible to perform such operation for some subset size $\leq s$, then we achieved $\frac{2}{(k+2\varepsilon)}$ approximation.
  * The relationship between $s$ and $\varepsilon$ is $s(\varepsilon) = (2k + 1)^{\,t - 1}$, where $t = \left\lceil \log_{k-1} \frac{1}{\varepsilon} \right\rceil$.
    * It's hardly useful to use this formula in practice, as its main purpose is to prove that a ratio arbitrarily close to $\frac{2}{k}$ can be achieved in polynomial time in $1/\varepsilon$.
      * In fact it's very theoretical, and the real results are much better.
    * The algorithm uses a formula to calculate theoretical approximation ratio $\rho(s)=\frac{2}{3+2s^{-log_7 2}}$ from $s$ for $k=3$ case.
    * For $k=2$ case, the formula is $\rho(s)=\frac{s+1}{s+2}$ used. *To be verified.*
  * It makes sense then to increase $s$ until either we found the maximum possible solution size or the time limit is reached.

## Implementation details

* Each of matroid problems is implemented as a subclass of `MatroidProblem` class.
* Function `MatroidProblem::tryAddElement` is implemented to add an element to the solution if it maintains independence for all matroids.
  * Implemented assuming that the current set is independent for all matroids
* `main.cpp` takes the problem name as a command line argument and runs all the feasible algorithms on the problem.
* Graph structure:
  * For bipartite and 3D matching problem, each graph partition has the same number of vertices.
  * For Hamiltonian path problem, the graph is a directed graph
  * Parameter `p` is supplied, which indicates the probability of each edge being present among the edges that are in the complete graph of the respective type.
  * For the Hamiltonian path problem, the parameter `minHamiltonianPathLength` is supplied, which indicates the guaranteed length of the longest path present in the graph.
* **Caution**: `MatroidProblem::reset()` has to be called manually to reset the current set to empty. Needed when running multiple algorithms on the same problem instance.

## Execution

Check out the example notebook for execution examples.

## Ideas for future work and for the report

 The distribution of the solutions to the same problem
  * I.e., how stable the algorithm is
  * Also would answer the question of how useful it is to rerun the algorithm on the same graph multiple times
* Study the relationship between $s$ (maximum step size), $n$, $p$, and the solution size/approximation ratio
  * Parameter $p$ itself might not be as good as $n^2p$ for normal graphs and $n^3p$ for 3D graps.
  * Many cool slice plots could be thought of here for the report.