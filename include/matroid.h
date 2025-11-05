#ifndef MATROID_H
#define MATROID_H

#include <vector>
#include <set>
#include <unordered_set>

// Abstract base class for Matroid
class Matroid {
public:
    virtual ~Matroid() = default;
    
    // Check if a set is independent in the matroid
    virtual bool isIndependent(const std::set<int>& S) const = 0;
    
    // Get the ground set size
    virtual int getGroundSetSize() const = 0;
    
    // Get all elements in the ground set
    virtual std::vector<int> getGroundSet() const = 0;
};

#endif // MATROID_H
