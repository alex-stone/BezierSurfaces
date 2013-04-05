#include <iostream>
#include "Point.h"

//****************************************************
// Point Class - Constructors
//****************************************************
Point::Point() {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}

Point::Point(float a, float b, float c) {
    this->x = a;
    this->y = b;
    this->z = c;
}

//****************************************************
// Point Class - Functions
//****************************************************
Point Point::add(Point b) {
    Point p;
    p.x = this->x + b.x;
    p.y = this->y + b.y;
    p.z = this->z + b.z;
 
    return p;
}

Point Point::cross(Point b) {
    Point p;

    p.x = (this->y * b.z) - (this->z * b.y);
    p.y = (this->z * b.x) - (this->x * b.z);
    p.z = (this->x * b.y) - (this->y * b.x);

    return p;
}

Point Point::scalarMultiply(float c) {
    Point p;
    p.x = this->x * c;
    p.y = this->y * c;
    p.z = this->z * c;

    return p;
}

void Point::normalize() {
    float length = sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
     
    this->x = x/length;
    this->y = y/length;
    this->z = z/length;
}

void Point::print() {
    printf("(%3.3f, %3.3f, %3.3f)", this->x, this->y, this->z);
   // std::cout << "(" << this->x << ", " << this->y << ", " << this->z << ")" << std::endl;
}

void Point::printInline() {
    printf("(%3.3f, %3.3f, %3.3f)", this->x, this->y, this->z);
    //std::cout << "(" << this->x << ", " << this->y << ", " << this->z << ")";
}
