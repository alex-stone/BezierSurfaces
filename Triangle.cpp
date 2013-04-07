#include <iostream>
#include <math.h>
#include "Triangle.h"

//****************************************************
// Triangle Class - Constructors
//****************************************************

Triangle::Triangle() {
    this->pt1.x = 0.0f;
    this->pt1.y = 0.0f;
     
    this->pt2.x = 0.0f;
    this->pt2.y = 1.0f;

    this->pt3.x = 1.0f;
    this->pt3.y = 0.0f;

    setMidpoints();
}

Triangle::Triangle(float x1, float y1, float x2, float y2, float x3, float y3) {

    this->pt1.x = x1;
    this->pt1.y = y1;
            
    this->pt2.x = x2;
    this->pt2.y = y2;

    this->pt3.x = x3;
    this->pt3.y = y3;

    this->setMidpoints();

}

Triangle::Triangle(Point a, Point b, Point c) {
    this->pt1.x = a.x;
    this->pt1.y = a.y;

    this->pt2.x = b.x;
    this->pt2.y = b.y;

    this->pt3.x = c.x;
    this->pt3.y = c.y;

    this->setMidpoints();
}

void Triangle::setMidpoints() {
    this->midPoint1.x = (this->pt1.x + this->pt2.x) / 2;
    this->midPoint1.y = (this->pt1.y + this->pt2.y) / 2;

    this->midPoint2.x = (this->pt1.x + this->pt3.x) / 2;
    this->midPoint2.y = (this->pt1.y + this->pt3.y) / 2;

    this->midPoint3.x = (this->pt2.x + this->pt3.x) / 2;
    this->midPoint3.y = (this->pt2.y + this->pt3.y) / 2;    

}


