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
void Point::print() {
    std::cout << "(" << this->x, ", " << this->y << ", " << this->z << ")" << std::endl;
}

void Point::printInline() {
    std::cout << "(" << this->x, ", " << this->y << ", " << this->z << ")";
}
