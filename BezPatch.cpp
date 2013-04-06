#include <iostream>
#include "BezPatch.h"
#include "Point.h"

//****************************************************
// BezPatch Class - Constructors
//****************************************************
BezPatch::BezPatch() {
    Point* tempPoint;    
//    patchDrawList = NULL;

    for(int i = 0; i < 4; i++) {
	    for(int j = 0; j < 4; j++) {
	        tempPoint = new Point();
	        controlPts[i][j] = tempPoint; 
	    }
    }
}

//****************************************************
// BezPatch Class - Functions
//****************************************************
void BezPatch::print() {
    for(int i = 0; i < 4; i++) {
    	for(int j = 0; j < 4; j++) {
	        controlPts[i][j]->printInline();
	        std::cout << "  ";
	    }
	    std::cout << std::endl;
    }
}


