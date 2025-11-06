#include "matroid_problem.h"

bool MatroidProblem::tryAddElement(int element) {
  int successfulAdditions = 0;

  for (auto &matroid : matroids_) {
    if (!matroid->tryAddElement(element)) {
      // Rollback: remove element from all previously added matroids
      for (int i = 0; i < successfulAdditions; i++) {
        matroids_[i]->removeElement(element);
      }
      return false;
    }
    successfulAdditions++;
  }
  setMembership_[element] = true;
  return true;
}

void MatroidProblem::removeElement(int element) {
  if (!setMembership_[element]) {
    throw std::invalid_argument("Element not in the set");
  }
  for (auto &matroid : matroids_) {
    matroid->removeElement(element);
  }
  setMembership_[element] = false;
}

void MatroidProblem::reset() {
  for (int i = 0; i < groundSetSize_; i++)
    if (setMembership_[i]) {
      removeElement(i);
    }
}