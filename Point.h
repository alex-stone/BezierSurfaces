#ifndef POINT_H
#define POINT_H

//****************************************************
// Point Header Definition
//****************************************************

class Point {
  public:
    // Variables
    float x, y, z;
    
    // Constructors
    Point();
    Point(float a, float b, float c);

    // Functions
    void print();
    void printInline();
}
#endif
