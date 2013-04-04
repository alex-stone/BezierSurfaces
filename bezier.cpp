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
#include "BezPatch.h"
#include "Point.h"

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

//****************************************************
// Global Variables 
//****************************************************
Viewport 	viewport;
int		numBezPatches;
BezPatch**	bezPatches;

// Command Line Arguments
char*		inputFile;
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
	bezPatches = new BezPatch*[numBezPatches];	

	// Iterate through the Patches
	for(int numPatch = 0; numPatch < numBezPatches; numPatch++) {

	    BezPatch* patch = new BezPatch();

	    // Iterate through each Control Point in the Patch
	    for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
		    float x,y,z;
		    inpfile >> x;
		    inpfile >> y;
		    inpfile >> z; 
		    
		    Point* controlPt = new Point(x,y,z);
		
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
// Parse Command Arguments Function:
//	- Sets Variables
//		- inputFile
//		- Subdivision Paramater
//		- Uniform Boolean (from -a flag)
//		- Debug Boolean (from -debug flag)
//****************************************************
void parseCommandArguments(int argc, char* argv[]) { 
    if(argc < 3 || argc > 5) {
	std::cerr << "Incorrect # of Arguments. Required (1) Input File Name, (2) Subdivision Parameter. Optional Flags: '-a' for adaptive subdivision, '-debug' for Debug print statements" << std::endl;
	std::exit(1);
    } else {
	inputFile = argv[1];
	subdivParam = atof(argv[2]);
	debug = false;
    }

    if(argc == 4) {
        if(string(argv[3]) == "-a") {
	    uniform = false;
	    debug = false;
	} else {
	    if(string(argv[3]) == "-debug") {
		uniform = true;
		debug = true;
	    } else {
		std::cerr << "Incorrect Flags Parameter" << std::endl;
		std::exit(1);
	    }
	}
    }
    
    if(argc == 5) {
	if(string(argv[3]) == "-a" && string(argv[4]) == "-debug") {
	    uniform = false;
	    debug = true;
	} else {
	    std::cerr << "Incorrect Flag Parameters" << std::endl;
	}
    }

}

//****************************************************
// Test Functions
//****************************************************
void testCommandArguments() {

    std::cout <<"*******************************************" << std::endl;
    std::cout <<"  Command Line Argument - Test Info" << std::endl;
    std::cout <<"*******************************************" << std::endl;
    
    std::cout << "Input File Name: " << inputFile << std::endl;
    std::cout << "Subdivision Type = ";
    if(uniform) {
	std::cout << "UNIFORM" << std::endl;
    } else {
	std::cout << "ADAPTIVE" << std::endl;
    }

    std::cout << std::endl;
}

void testBezPatch() {
    std::cout <<"*******************************************" << std::endl;
    std::cout <<"  Bez Patch File Input  - Test Info" << std::endl;
    std::cout <<"*******************************************" << std::endl;
    
    std::cout << "# of Patches: " << numBezPatches << std::endl;
    for(int i = 0; i < numBezPatches; i++) {
	std::cout << "BezPatch #" << i+1 << std::endl;
	bezPatches[i]->print();
	std::cout << std::endl;
    }
}

//****************************************************
// Main Function
//****************************************************
int main(int argc, char *argv[]) {

    // Initialize Paramaters: inputFile, subdivParam, uniform, & debug
    parseCommandArguments(argc, argv);

    // Initialize the Bez Patches from Input File
    initBezPatches(inputFile);

    if(debug) {
	testCommandArguments();
    	testBezPatch();
    }
}



