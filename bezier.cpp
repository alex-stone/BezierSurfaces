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
Point**     points;
int         numDiv;

// Command Line Arguments
char*		inputFile;
float		subdivParam;		// Step Size in U & V for uniform, OR Error measure for adaptive
bool		uniform;		// If true, uniform subdivision, else adaptive subdivision.
bool		debug;


// Debugging Variables
const bool DRAW_TEST = true;
 

//****************************************************
// Glut Functions
//****************************************************
void initScene() {
    // TODO: /glutC/
    if(debug) {
        std::cout << "Function: initScene" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);

    glDepthFunc(GL_LEQUAL);

}

void myReshape(int w, int h) {
    if(debug) {
        std::cout << "Function: myReshape" << std::endl; 
    }

    viewport.w = w;
    viewport.h = h;

    glViewport(0,0,viewport.w,viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // TODO: Modify this code from AS1 to work for this 

    

    //gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
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
// BezCurveInterp Function: 
//   - Given Control Points of a Bezier Curve, and a
//        parametric value, return the curve point &
//        derivative
//   - Used pseudo-code from Professor O'Brien's 
//        Slides:  Spring 2013 Lecture 12 - Surfaces
//
//****************************************************
pair<Point,Point> bezCurveInterp(Point* curve, float u) {
   
    // The Return Value - Curve Point & Derivative
    std::pair <Point,Point> pointDerivative;    

    // Split each of the 3 segments to form two segments AB & AC
    Point segmentA = curve[0].scalarMultiply(1.0 - u).add(curve[1].scalarMultiply(u));
    Point segmentB = curve[1].scalarMultiply(1.0 - u).add(curve[2].scalarMultiply(u));
    Point segmentC = curve[2].scalarMultiply(1.0 - u).add(curve[3].scalarMultiply(u));

    // Split AB and BC to form new segment DE
    Point segmentD = segmentA.scalarMultiply(1.0 - u).add(segmentB.scalarMultiply(u));
    Point segmentE = segmentB.scalarMultiply(1.0 - u).add(segmentC.scalarMultiply(u));

    // Pick the right point on DE, the point on the curve
    pointDerivative.first = segmentD.scalarMultiply(1.0 - u).add(segmentE.scalarMultiply(u));

    // Compute Derivative: (E - D) * 3
    pointDerivative.second = segmentE.add(segmentD.scalarMultiply(-1.0)).scalarMultiply(3);

    //std::cout << "(" << pointDerivative.first.x << ", " << pointDerivative.first.y << ", " << pointDerivative.first.z << ")" << std::endl;

    return pointDerivative;
}


//****************************************************
// Helper Functions for BezPatchInterp
//      Return the Array of 4 points, for the 
//      appropriate V & U Curves
//****************************************************

Point* getVcurvePoints(BezPatch* patch, int n) {
    Point* curve[4];
 
    curve[0] = patch->controlPts[n][0];
    curve[1] = patch->controlPts[n][1];
    curve[2] = patch->controlPts[n][2];
    curve[3] = patch->controlPts[n][3];

    curve[0]->print();
    curve[1]->print();
    curve[2]->print();
    curve[3]->print();
    

    std::cout << std::endl;
    
    return *curve;
}

Point* getUcurvePoints(BezPatch* patch, int n) {
    Point* curve[4];

    curve[0] = patch->controlPts[0][n];
    curve[1] = patch->controlPts[1][n];
    curve[2] = patch->controlPts[2][n];
    curve[3] = patch->controlPts[3][n];
    
    return *curve;
}


//****************************************************
// BezPatchInterp Function:
//   - Given a Control Patch and (u,v) values, find
//      the surface point and normal
//****************************************************
pair<Point,Point> bezPatchInterp(BezPatch* patch, float u, float v) {
    Point vcurve[4];
    Point ucurve[4];
    Point tempcurve[4];

    std::pair <Point,Point> pointNormal;
    std::pair <Point,Point> VptDeriv;
    std::pair <Point,Point> UptDeriv;

    // Build Control Points for Bezier Curve in V
    
    tempcurve[0] = *patch->controlPts[0][0];
    tempcurve[1] = *patch->controlPts[0][1];
    tempcurve[2] = *patch->controlPts[0][2];
    tempcurve[3] = *patch->controlPts[0][3];
    
    vcurve[0] = bezCurveInterp(tempcurve, u).first;
    
    tempcurve[0] = *patch->controlPts[1][0];
    tempcurve[1] = *patch->controlPts[1][1];
    tempcurve[2] = *patch->controlPts[1][2];
    tempcurve[3] = *patch->controlPts[1][3];
    
    vcurve[1] = bezCurveInterp(tempcurve, u).first;
    
    tempcurve[0] = *patch->controlPts[2][0];
    tempcurve[1] = *patch->controlPts[2][1];
    tempcurve[2] = *patch->controlPts[2][2];
    tempcurve[3] = *patch->controlPts[2][3];

    vcurve[2] = bezCurveInterp(tempcurve, u).first;
    
    tempcurve[0] = *patch->controlPts[3][0];
    tempcurve[1] = *patch->controlPts[3][1];
    tempcurve[2] = *patch->controlPts[3][2];
    tempcurve[3] = *patch->controlPts[3][3];
    
    vcurve[3] = bezCurveInterp(tempcurve, u).first;

    // Build Control Points for Bezier Curve in U

    tempcurve[0] = *patch->controlPts[0][0];
    tempcurve[1] = *patch->controlPts[1][0];
    tempcurve[2] = *patch->controlPts[2][0];
    tempcurve[3] = *patch->controlPts[3][0];

    ucurve[0] = bezCurveInterp(tempcurve, v).first;
    
    tempcurve[0] = *patch->controlPts[0][1];
    tempcurve[1] = *patch->controlPts[1][1];
    tempcurve[2] = *patch->controlPts[2][1];
    tempcurve[3] = *patch->controlPts[3][1];
    
    ucurve[1] = bezCurveInterp(tempcurve, v).first;
    
    tempcurve[0] = *patch->controlPts[0][2];
    tempcurve[1] = *patch->controlPts[1][2];
    tempcurve[2] = *patch->controlPts[2][2];
    tempcurve[3] = *patch->controlPts[3][2];

    ucurve[2] = bezCurveInterp(tempcurve, v).first;
    
    tempcurve[0] = *patch->controlPts[0][3];
    tempcurve[1] = *patch->controlPts[1][3];
    tempcurve[2] = *patch->controlPts[2][3];
    tempcurve[3] = *patch->controlPts[3][3];
    
    ucurve[3] = bezCurveInterp(tempcurve, v).first;


    // Evaluate Surface & Derivative for U & V
    VptDeriv = bezCurveInterp(vcurve, v);
    UptDeriv = bezCurveInterp(ucurve, u);

    Point dPdv = VptDeriv.second;
    Point dPdu = UptDeriv.second;
 
    // Take Cross Product of partials to find Normal:
    Point normal = dPdu.cross(dPdv);
    normal.normalize();

    pointNormal.first = VptDeriv.first;
    pointNormal.second = normal;

    std::cout << "(" << pointNormal.first.x << ", " << pointNormal.first.y << ", " << pointNormal.first.z << ")" << std::endl;
    return pointNormal;
}


//****************************************************
// uniformSubdividePatch - Uniform Subdivision
//****************************************************
void uniformSubdividePatch(BezPatch* patch, float step) {

    std::pair <Point,Point> pointNormal;
    float u, v;
    float epsilon = 0.001;

    // Compute # of subdivisions for step size
//    numDiv = (int)((1.0 + epsilon) / step);



    //glBegin(GL_POINTS);

    // For each Parametric Value of U:      -1 because There is one fewer row of patches.
    for(int iu = 0; iu < numDiv; iu++) {
        u = iu * step;
    
        // For each Parametric Value of V:
        for(int iv = 0; iv < numDiv; iv++) {
            v = iv * step;


            // Evaluate Surface:
            pointNormal = bezPatchInterp(patch, u, v);
            //glColor3f(pointNormal.second.x, pointNormal.second.y, pointNormal.second.z);
   
            
            std::cout << "test " << std::endl;        
            points[iu][iv] = pointNormal.first; 
            std::cout << "test " << std::endl;        
     //       glColor3f(1.0f, 1.0f, 1.0f); 
   //         glVertex3f(pointNormal.first.x, pointNormal.first.y, pointNormal.first.z);
   
///            glVertex3f(0.0f, 0.0f, 0.0f);         
           // glNormal3f(pointNormal.second.x, pointNormal.second.y, pointNormal.second.z);  

        }
    }

    //glEnd();
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

void testDisplayControlPoints() {
    
    std::cout << "Testing Drawing" << std::endl;

    BezPatch* patch;

    glBegin(GL_POINTS);

    for(int i = 0; i < 20; i++) {
        for(int j = 0; j < 20; j++) {
            float x = (float)(i - 10) /10;
            float y = (float)(j - 10) /10;
            
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex3f(x, y, -1);
        }
    }
    glEnd();
   /* 
    for(int i = 0; i < numBezPatches; i++) {
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                
                patch = bezPatches[i];
              
                GLfloat r = 1.0f;
                GLfloat g = 0.0f;
                GLfloat b = 0.0f;

                glColor3f(r,g,b);
                
                glVertex3f(patch->controlPts[j][k]->x, patch->controlPts[j][k]->y, patch->controlPts[j][k]->z);
            }
        }
    }


    glEnd(); */
}

// Function that does the actual drawing
void myDisplay() {
    if(debug) {
        std::cout << "MY DISPLAY" << std::endl;
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
    // Make sure transformation is "zero'd"
    glLoadIdentity();

  //  glBegin(GL_POINTS);

    std::cout << "MY DISPLAY" << std::endl;

    if(DRAW_TEST) {
        testDisplayControlPoints();
    }

/*
    for(int i = 0; i < numDiv ; i++) {
        for(int j = 0; j < numDiv; j++) {
            glColor3f(1.0f, 1.0f, 1.0f);
            std::cout << "Points[0][0] = " << std::endl;
            points[0][0].print();
            glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
        }
    }

    glEnd();
*/
    if(debug) {
        for(int i = 0; i < numBezPatches; i++) {
     //       uniformSubdividePatch(bezPatches[i], subdivParam);
        

        }
    }
     
    // TODO: Add Transformations and Translations to be drawn 
     
    glFlush();
    glutSwapBuffers();
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

    numDiv = (int)(1.0 + 0.001)/subdivParam;
    Point points[numDiv][numDiv];

    if(DRAW_TEST) {
        testDisplayControlPoints();
    }

/*
    int count = 0;
 
    // Add the points to the array 
    for (int i = 0; i < numBezPatches; i++) {
    
        std::cout << count << std::endl;
        uniformSubdividePatch(bezPatches[i], subdivParam);
        count++;
    }
 */ 
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyPress);
    glutMainLoop();
    
    return 0; 

}



