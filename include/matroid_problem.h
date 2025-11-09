#ifndef MATROID_H
#define MATROID_H

#include <set>
#include <memory>
#include <unordered_set>
#include <vector>

// Abstract base class for a matroid problem; incapsulates all the matroids for
// the problem
class MatroidProblem {
public:
  // Constructor that initializes the ground set size
  explicit MatroidProblem(int groundSetSize, int matroidQuatity)
      : groundSetSize_(groundSetSize), matroidQuantity_(matroidQuatity),
        setMembership_(groundSetSize, false) {}

  // Attempt to add an element to all underlying matroid sets
  // Returns true if the element was successfully added to all sets, false
  // otherwise If any addition fails, all previous additions are rolled back
  bool tryAddElement(int element);

  // Remove an element from all underlying matroid sets
  void removeElement(int element);

  // Reset the set membership to all false
  void reset();

  // A subset of the ground set; the inner class enhances efficiency
  class MatroidSet {
  public:
    virtual ~MatroidSet() = default;

    // Add element only if the set remains independent, otherwise return false
    virtual bool tryAddElement(int element) = 0;

    // Remove element
    virtual void removeElement(int element) = 0;
  };
  int getGroundSetSize() const { return groundSetSize_; }

  int getMatroidQuantity() const { return matroidQuantity_; }

protected:
  int groundSetSize_;   // the size of the shared ground set
  int matroidQuantity_; // the quantity of the matroids to intersect
  std::vector<std::unique_ptr<MatroidSet>>
      matroids_; // the matroids to intersect
  std::vector<bool>
      setMembership_; // true if the element is in the intersection
};

#endif // MATROID_H
