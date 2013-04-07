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
#include "Triangle.h"

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

// OpenGL Transformation Variables:
GLfloat theta;          // Angle on X-axis
GLfloat phi;            // Angle on Y-Axis
GLfloat xTranslate;
GLfloat yTranslate;
GLfloat zTranslate;
const GLfloat TRANSLATE_INC = 0.05f;
const GLfloat Z_TRANSLATE_INC = 0.2f;
const GLfloat ROTATE_INC = 3.0f;

// OpenGL Drawing Type Variables
bool light;
bool smooth;
bool wire;

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

    wire = false;
    smooth = false;
    

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
    
    glRotatef(theta, 1.0f, 0.0f, 0.0f);
    glRotatef(phi, 0.0f, 0.0f, 1.0f);

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
            smooth = !smooth;
            break;
        case 'w':
            wire = !wire;
            if(wire) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            }
            break;
        case '+':
            zTranslate += Z_TRANSLATE_INC;
            break;
        case '-':
            zTranslate -= Z_TRANSLATE_INC;
            break;
         case ' ':
            std::exit(1);
            break;
    }

    myDisplay();
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
                xTranslate -= TRANSLATE_INC;
            } else {
                phi -= ROTATE_INC;
            }
            break;
        case 101:       // Up Arrow 
            if(shift) {
                yTranslate += TRANSLATE_INC;
            } else {
                theta -= ROTATE_INC;
            }
            break;
        case 102:       // Right Arrow 
            if(shift) {
                xTranslate += TRANSLATE_INC;
            } else {
                phi += ROTATE_INC;
            }
            break;
        case 103:       // Down Arrow 
            if(shift) {
                yTranslate -= TRANSLATE_INC;
            } else {
                theta += ROTATE_INC;
            }
            break;
    }

    myDisplay();

}

//****************************************************
// uniformSubdividePatch - Uniform Subdivision
//      Return a Draw List for a given Patch
//****************************************************
GLuint uniformSubdividePatch(BezPatch* patch, float step) {

    std::pair <Point,Point> pointNormal;
    float u, v;
    float epsilon = 0.1;

    // Compute # of subdivisions for step size
    int numDiv = (int)((1.0 + epsilon) / step)+1;


    Point ptArray[numDiv][numDiv];
    Point normArray[numDiv][numDiv];

    GLuint patchDrawList = glGenLists(1);

    for(int iu = 0; iu < numDiv; iu++) {
        u = iu * step;
       
        // For each Parametric Value of V:
        for(int iv = 0; iv < numDiv; iv++) {
            v = iv * step;

            // Evaluate Surface:
            pointNormal = patch->bezPatchInterp(u, v);

            ptArray[iu][iv] = pointNormal.first;
            normArray[iu][iv] = pointNormal.second;
        }
    }

    glNewList(patchDrawList, GL_COMPILE);


    for(int i = 0; i < numDiv-1; i++) {
        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j < numDiv; j++) {
            //glBegin(GL_QUADS);

            glColor3f(normArray[i][j].x, normArray[i][j].y, normArray[i][j].z);
            
            glVertex3f(ptArray[i][j].x, ptArray[i][j].y, ptArray[i][j].z);
            glVertex3f(ptArray[i+1][j].x, ptArray[i+1][j].y, ptArray[i+1][j].z);
           // glVertex3f(ptArray[i][j+1].x, ptArray[i][j+1].y, ptArray[i][j+1].z);
           // glVertex3f(ptArray[i+1][j+1].x, ptArray[i+1][j+1].y, ptArray[i+1][j+1].z);

            
        }
        glEnd();
    }

    glEndList();

    return patchDrawList;
}

void subdivideTriangles(BezPatch* patch, float threshold, Triangle tri) {
    
    std::pair<Point,Point> x1, x2, x3, bezMid1, bezMid2, bezMid3;

    // The 3 Points From the Vertices of the triangle
    x1 = patch->bezPatchInterp(tri.pt1.x, tri.pt1.y);
    x2 = patch->bezPatchInterp(tri.pt2.x, tri.pt2.y);
    x3 = patch->bezPatchInterp(tri.pt3.x, tri.pt3.y);

    // The 3 Points From the Midpoints of the Triangles
    bezMid1 = patch->bezPatchInterp(tri.midPoint1.x, tri.midPoint1.y);
    bezMid2 = patch->bezPatchInterp(tri.midPoint2.x, tri.midPoint2.y);
    bezMid3 = patch->bezPatchInterp(tri.midPoint3.x, tri.midPoint3.y);

    float dist1 = abs(x1.first.add(x2.first).scalarMultiply(-0.5).distTo(bezMid1.first));
    float dist2 = abs(x1.first.add(x3.first).scalarMultiply(-0.5).distTo(bezMid2.first));
    float dist3 = abs(x2.first.add(x3.first).scalarMultiply(-0.5).distTo(bezMid3.first));

    std::cout << "dist1 = :" << dist1 << std::endl;
    std::cout << "dist2 = :" << dist2 << std::endl;
    std::cout << "dist3 = :" << dist3 << std::endl;

    // The 3 Midpoint Tests used to determine how to split up triangle
    bool test1 = dist1 < threshold;
    bool test2 = dist2 < threshold;
    bool test3 = dist3 < threshold;
  
    if(test1 && test2 && !test3) { 
        
        std::cout << "Subdivide Triangles - test3 fails" << std::endl;
        Triangle tri1(tri.pt1, tri.midPoint2, tri.midPoint3);     
        Triangle tri2(tri.pt1, tri.midPoint3, tri.pt3);
        
        subdivideTriangles(patch, threshold, tri1);
        subdivideTriangles(patch, threshold, tri2);     
    }
     
    if(test1 && !test2 && test3) {
        
        std::cout << "Subdivide Triangles - test2 fails" << std::endl;
        Triangle tri1(tri.pt1, tri.midPoint2, tri.pt3);
        Triangle tri2(tri.midPoint2, tri.pt2, tri.pt3);
        
        subdivideTriangles(patch, threshold, tri1);
        subdivideTriangles(patch, threshold, tri2);    
    } 
   
    if(!test1 && test2 && test3) {
        
        std::cout << "Subdivide Triangles - test1 fails" << std::endl;
        Triangle tri1(tri.pt1, tri.pt2, tri.midPoint1);
        Triangle tri2(tri.midPoint1, tri.pt2, tri.pt3); 
        
        subdivideTriangles(patch, threshold, tri1);
        subdivideTriangles(patch, threshold, tri2);   
    }
   
    if(test1 && !test2 && !test3) {
        
        std::cout << "Subdivide Triangles - test2 and test3" << std::endl;
        Triangle tri1(tri.pt1, tri.midPoint3, tri.pt3);
        Triangle tri2(tri.pt1, tri.midPoint2, tri.midPoint3);
        Triangle tri3(tri.midPoint2, tri.pt2, tri.midPoint3);
        
        subdivideTriangles(patch, threshold, tri1);    
        subdivideTriangles(patch, threshold, tri2);    
        subdivideTriangles(patch, threshold, tri3);    
    }

    if(!test1 && test2 && !test3) {
        
        std::cout << "Subdivide Triangles - test1 and test3 fail" << std::endl;
        Triangle tri1(tri.pt1, tri.pt2, tri.midPoint1);

        Triangle tri2(tri.midPoint1, tri.midPoint3, tri.pt3);
        Triangle tri3(tri.midPoint1, tri.pt2, tri.midPoint3);
       
        
        subdivideTriangles(patch, threshold, tri1);    
        subdivideTriangles(patch, threshold, tri2);    
        subdivideTriangles(patch, threshold, tri3);    
    }

    if(!test1 && !test2 && test3) {
        
        std::cout << "Subdivide Triangles - test1 and test 2 fail" << std::endl;
        Triangle tri1(tri.pt1, tri.midPoint2, tri.midPoint1); 
        Triangle tri2(tri.midPoint2, tri.pt3, tri.midPoint1);
        Triangle tri3(tri.midPoint2, tri.pt2, tri.pt3);
        
        subdivideTriangles(patch, threshold, tri1);
        subdivideTriangles(patch, threshold, tri2);
        subdivideTriangles(patch, threshold, tri3);        
    }
    
    // All MidPoint Tests Fail
    if(!test1 && !test2 && !test3) {

       // std::cout << "Subdivide Triangles - All Tests fails" << std::endl;
        Triangle tri1(tri.pt1, tri.midPoint2, tri.midPoint1);
        Triangle tri2(tri.midPoint1, tri.midPoint3, tri.pt3);
        Triangle tri3(tri.midPoint2, tri.pt2, tri.midPoint3);
        Triangle tri4(tri.midPoint1, tri.midPoint2, tri.midPoint3);
         
        subdivideTriangles(patch, threshold, tri1);
        subdivideTriangles(patch, threshold, tri2);
        subdivideTriangles(patch, threshold, tri3);
        subdivideTriangles(patch, threshold, tri4);
    }

    // All MidPoint Tests Pass - Draw the Triangle
    if(test1 && test2 && test3) {
        std::cout << "Drawing Triangle" << std::endl;
        glBegin(GL_TRIANGLES);
        
        glNormal3f(x1.second.x, x1.second.y, x1.second.z);
        glVertex3f(x1.first.x, x1.first.y, x1.first.z);
        
        glNormal3f(x2.second.x, x2.second.y, x2.second.z);
        glVertex3f(x2.first.x, x2.first.y, x2.first.z);
        
        glNormal3f(x3.second.x, x3.second.y, x3.second.z);
        glVertex3f(x3.first.x, x3.first.y, x3.first.z);
        
        glEnd(); 
    }
}

//****************************************************
// adaptiveSubdividePatch - Adaptive Subdivision
//      Return a Draw List for a given Patch
//****************************************************
GLuint adaptiveSubdividePatch(BezPatch* patch, float step) {
    
    Triangle tri1(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
    Triangle tri2(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    GLuint patchDrawList = glGenLists(1);

    glNewList(patchDrawList, GL_COMPILE);

    std::cout << "Adaptive Subdivide Called" << std::endl;

    subdivideTriangles(patch, step, tri1);
    subdivideTriangles(patch, step, tri1);

    glEndList();

    return patchDrawList; 
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

void testDrawPoints() {
    std::pair <Point,Point> pointNormal;
    float u, v;
    float epsilon = 0.1;

    // Compute # of subdivisions for step size
    int numDiv = (int)((1.0 + epsilon) / subdivParam)+1;

    for(int a = 0; a < numBezPatches; a++) {
        BezPatch* patch = bezPatches[a];

        GLuint patchDrawList = glGenLists(1);

        glNewList(patchDrawList, GL_COMPILE);


        // For each Parametric Value of U:      
        for(int iu = 0; iu < numDiv; iu++) {
            u = iu * subdivParam;
   
            glBegin(GL_POINTS);
    
            // For each Parametric Value of V:
            for(int iv = 0; iv < numDiv; iv++) {
                v = iv * subdivParam;

                // Evaluate Surface:
                pointNormal = patch->bezPatchInterp(u, v);

                glColor3f(pointNormal.second.x, pointNormal.second.y, pointNormal.second.z);
   
                glNormal3f(pointNormal.second.x, pointNormal.second.y, pointNormal.second.z);  
                glVertex3f(pointNormal.first.x, pointNormal.first.y, pointNormal.first.z);
   
            }

            glEnd();
        }

        glEndList();

        drawLists[a] = patchDrawList;
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



