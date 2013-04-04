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
    printf("(%3.3f, %3.3f, %3.3f)", this->x, this->y, this->z);
   // std::cout << "(" << this->x << ", " << this->y << ", " << this->z << ")" << std::endl;
}

void Point::printInline() {
    printf("(%3.3f, %3.3f, %3.3f)", this->x, this->y, this->z);
    //std::cout << "(" << this->x << ", " << this->y << ", " << this->z << ")";
}
