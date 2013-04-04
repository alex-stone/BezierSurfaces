
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>


#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }

using namespace std;

//****************************************************
// Class Definitions
//****************************************************

class Viewport {
  public:
    int w, h; // width and height
};

class Point {
  public:
    float x, y, z;
    Point(float a, float b, float c);
};

class BezPatch {
  public: 
    Point *controlPts[4][4];
};


//****************************************************
// Class Functionality
//****************************************************

Point::Point(float a, float b, float c) {
    this->x = a;
    this->y = b;
    this->z = c;
}


//****************************************************
// Global Variables 
//****************************************************
Viewport 	viewport;
int		numBezPatches;
BezPatch*	bezPatches;
float		subdivParam;		// Step Size in U & V for uniform, OR Error measure for adaptive
bool		uniform;		// If true, uniform subdivision, else adaptive subdivision.
bool		debug;

//****************************************************
// loadBezPatches Function
//	- First line is # of Patches (numBezPatches)
//	- Reads that many patches into bezPatches
//****************************************************
void initBezPatches(char* input) {
    std::ifstream inpfile(input, ifstream::in);	// Allows for input operations to be called on inpfile

    // Check if file is good, then check # of patches.
    if(inpfile.good()) {

	// First line is number of patches
	inpfile >> numBezPatches;
		
	// Create an Array of BezPatches
	bezPatches = new BezPatch[numBezPatches];	

	// Iterate through the Patches
	for(int numPatch = 0; numPatch < numBezPatches; numPatch++) {

	    BezPatch *patch = new BezPatch[numBezPatches];

	    // Iterate through each Control Point in the Patch
	    for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
		    float x,y,z;
		    inpfile >> x;
		    inpfile >> y;
		    inpfile >> z; 
		    
		    Point *controlPt = new Point(x,y,z);
		
		    // Add current Control Point into current Patch
		    patch->controlPts[i][j] = controlPt;
		}	
	    }
	    // Add Patch to array of Bez Patches
	    bezPatches[numPatch] = patch;
	}
    }
    inpfile.close();
}



//****************************************************
// Test Functions
//****************************************************
void testCommandArguments(char* inputName) {
    std::cout << "Input File Name: " << inputName << std::endl;
    std::cout << "Subdivision Type = ";
    if(uniform) {
	std::cout << "UNIFORM" << std::endl;
    } else {
	std::cout << "ADAPTIVE" << std:endl;
    }

}

//****************************************************
// Main Function
//****************************************************
int main(int argc, char *argv[]) {

    char* inputFile;
    debug = false;

    // Read in the Command Line Arguments, and initialize inputFile, subdivParam, and uniform boolean.
    // If # of arguments is less than 3 it is missing either the input file name, the subdivision parameter or both.
    if(argc < 3 || argc > 5) {
	std::cerr << "Incorrect Number of Command Line Arguments: Requires an Input File Name & Subdivision Parameter, with optional flag -a" << std::endl;
    } else {
	inputFile = argv[1];
	subdivParam = arv[2];
	if (argc == 3) {
	    uniform = true;
	} else {
	    if(argc == 5) {
		if(argv[4] == "-a" && argv[5] == "-debug") {
		    uniform = false;
		    debug = true;
		} else {
		    std::cerr << "Malformed Input" << std::endl;
		} 
	    } else {
		if(argv[4] == "-a") {
		    uniform = true;
		} else {
		    if(argv[4] == "-debug") {
			debug = true;
			uniform = false;
		    } else {
			std::cerr << "Malformed Input" << std::endl;
		    }
		}
	    }
	}
    }

    if(debug) {
	testCommandArguments(inputFile);
    }


    // Initialize the Bez Patches from Input File
    initBezPatches(inputFile);

}



