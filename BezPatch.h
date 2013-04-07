#ifndef BEZPATCH_H
#define BEZPATCH_H

#include <iostream>
#include "Point.h"

//****************************************************
// BezPatch Header Definition
//****************************************************
class BezPatch {
  public:
    // Variables
    Point* controlPts[4][4];

    // Constructors
    BezPatch();		// Initialize all Control Points to (0,0,0)

    // General BezPatch Functions
    void print();

    // Bez Curve Interpretation Functions
    std::pair<Point,Point> bezCurveInterp(Point* curve, float u);
 	std::pair<Point,Point> bezCurveInterpFormPoints(bool vCurve, int numCurve, float u);
    std::pair<Point,Point> bezPatchInterp(float u, float v);

};
#endif
