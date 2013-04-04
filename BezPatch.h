#ifndef BEZPATCH_H
#define BEZPATCH_H

#include "Point.h"

//****************************************************
// BezPatch Header Definition
//****************************************************
class BezPatch {
  public:
    // Variables
    Point** controlPts;

    // Constructors
    BezPatch();		// Initialize all Control Points to (0,0,0)

    // Functions
    void print();
};
#endif
