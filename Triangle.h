#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Point.h"

//****************************************************
// Triangle Header Definition
//****************************************************

class Triangle {
  public:
    Point pt1;
    Point pt2;
    Point pt3;

    Point midPoint1;
    Point midPoint2;
    Point midPoint3;

    Triangle();
    Triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    Triangle(Point a, Point b, Point c);

    void setMidpoints();

};

#endif
