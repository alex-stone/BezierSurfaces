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
int		    numBezPatches;
BezPatch**	bezPatches;
GLuint*     drawLists;

// Command Line Arguments
char*		inputFile;
float		subdivParam;		// Step Size in U & V for uniform, OR Error measure for adaptive
bool		uniform;		// If true, uniform subdivision, else adaptive subdivision.
bool		debug;

// OpenGL Perspective Variables
GLdouble aspectRatio;
const GLdouble FOV_Y = 45.0;
const GLdouble Z_NEAR = 1.0;
const GLdouble Z_FAR = 40.0;

// OpenGL Drawing Variables:
GLfloat theta;          // Angle on X-axis
GLfloat phi;            // Angle on Y-Axis
GLfloat xTranslate;
GLfloat yTranslate;
GLfloat zTranslate;

// Debugging Variables
const bool DRAW_TEST = true;

//****************************************************
// Glut Functions
//****************************************************
void initScene() {
    theta = 0.0f;
    phi = 0.0f;
    xTranslate = 0.0f;
    yTranslate = 0.0f;
    zTranslate = -15.0f;       // Set Based on size of Input

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    glDepthFunc(GL_LEQUAL);

}

//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {

    viewport.w = w;
    viewport.h = h;

    aspectRatio = ((GLdouble) w) / ((GLdouble) h);
    
    glViewport(0,0,viewport.w,viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(FOV_Y, aspectRatio, Z_NEAR, Z_FAR);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
}

// Function that does the actual drawing
void myDisplay() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
    // Make sure transformation is "zero'd"
    glLoadIdentity();

    // Setting Up the Translations for Object 
    glTranslatef(xTranslate, yTranslate, zTranslate);


    for(int i = 0; i < numBezPatches; i++) {
        glCallList(drawLists[i]);
    }
 
    glFlush();
    glutSwapBuffers();
}  

//****************************************************
// On keyPress:
//   if key:
//      's':  Toggle Flat & Smooth Shading
//      'w':  Toggle Filled & Wire Framing
//      '+':  Zoom In
//      '-':  Zoom out
//      ' ':  Close the window
//****************************************************
void keyPress(unsigned char key, int x, int y) {
   
    switch(key) {
        case 's':
                
            break;
        case 'w':
    
            break;
        case '+':
        
            break;
        case '-':

            break;
         case ' ':
            std::exit(1);
            break;
    }
}

//****************************************************
// Arrow Key Press
//      
//****************************************************
void arrowKeyPress(int key, int x, int y) {

    bool shift = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);

    switch(key) {
        case 100:       // Left Arrow
            if(shift) {
                
            } else {

            }
            break;
        case 101:       // Up Arrow 
            
            break;
        case 102:       // Right Arrow 
            
            break;
        case 103:       // Down Arrow 
    
            break;
    }
}

//****************************************************
// uniformSubdividePatch - Uniform Subdivision
//      Return a Draw List for a given Patch
//****************************************************
GLuint uniformSubdividePatch(BezPatch* patch, float step) {

    std::pair <Point,Point> pointNormal;
    float u, v;
    float epsilon = 0.001;

    GLuint patchDrawList = glGenLists(1);

    // Compute # of subdivisions for step size
    int numDiv = (int)((1.0 + epsilon) / step);

    glNewList(patchDrawList, GL_COMPILE);

    // For each Parametric Value of U:      -1 because There is one fewer row of patches.
    for(int iu = 0; iu < numDiv; iu++) {
        u = iu * step;
   
        glBegin(GL_POINTS);
    
        // For each Parametric Value of V:
        for(int iv = 0; iv < numDiv; iv++) {
            v = iv * step;

            // Evaluate Surface:
            pointNormal = patch->bezPatchInterp(u, v);
            glColor3f(pointNormal.second.x, pointNormal.second.y, pointNormal.second.z);
   
            glNormal3f(pointNormal.second.x, pointNormal.second.y, pointNormal.second.z);  
            glVertex3f(pointNormal.first.x, pointNormal.first.y, pointNormal.first.z);
   
        }

        glEnd();
    }

    glEndList();

    return patchDrawList;
}

//****************************************************
// uniformSubdividePatch - Uniform Subdivision
//      Return a Draw List for a given Patch
//****************************************************

GLuint adaptiveSubdividePatch(BezPatch* patch, float step) {
    

    return uniformSubdividePatch(patch, step);
}

//****************************************************
// makeDrawLists
//      - Create the drawlists from the patches using
//        either Uniform or Adaptive Subdivision
//****************************************************
void makeDrawLists() {
    for (int i = 0; i < numBezPatches; i++) {
        if(uniform) {
            drawLists[i] = uniformSubdividePatch(bezPatches[i], subdivParam);
        } else {
            drawLists[i] = adaptiveSubdividePatch(bezPatches[i], subdivParam);
        }
    }
}

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

    // Initalize GLUT
    glutInit(&argc, argv);

    // Initialize Paramaters: inputFile, subdivParam, uniform, & debug
    parseCommandArguments(argc, argv);

    // Initialize the Bez Patches from Input File
    initBezPatches(inputFile);

    // Added GLUT_DEPTH for z-buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);

    // Initialize the Viewport Size
    viewport.w = 400;
    viewport.h = 400;

    // The size and position of the window
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow("CS184 - AS3 Bezier Surfaces");

    initScene();
   
    if(debug) {
        testCommandArguments();
        testBezPatch();
    }

    drawLists = new GLuint[numBezPatches];

    // Create the Draw Lists
    makeDrawLists();
 
    // GLUT Function Setup 
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPress);
    glutSpecialFunc(arrowKeyPress);
    glutMainLoop();
    
    return 0; 

}



