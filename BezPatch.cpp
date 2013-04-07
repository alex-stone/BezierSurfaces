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


std::pair<Point,Point> BezPatch::bezCurveInterp(Point* curve, float u) {

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

    return pointDerivative;
}

//****************************************************
// BezCurveInterpFormPoints
//   - Builds the Control Points for Bezier Curve
//		in either U or V curve.
//	 - Then calls bezCurve Interp on that
//****************************************************
std::pair<Point,Point> BezPatch::bezCurveInterpFormPoints(bool vCurve, int numCurve, float u) {
	Point point[4];
	// Set up Points For Math
	if(vCurve) {
		point[0] = *this->controlPts[numCurve][0];
		point[1] = *this->controlPts[numCurve][1];
		point[2] = *this->controlPts[numCurve][2];
		point[3] = *this->controlPts[numCurve][3];
	} else {
		point[0] = *this->controlPts[0][numCurve];
		point[1] = *this->controlPts[1][numCurve];
		point[2] = *this->controlPts[2][numCurve];
		point[3] = *this->controlPts[3][numCurve];
	}

	return this->bezCurveInterp(point, u);
}

//****************************************************
// BezPatchInterp Function:
//   - Given a Control Patch and (u,v) values, find
//      the surface point and normal
//****************************************************
std::pair<Point,Point> BezPatch::bezPatchInterp(float u, float v) {
    Point vcurve[4];
    Point ucurve[4];
    Point tempcurve[4];

    std::pair <Point,Point> pointNormal;
    std::pair <Point,Point> VptDeriv;
    std::pair <Point,Point> UptDeriv;

    // Build Control Points for Bezier Curve in V   
    vcurve[0] = this->bezCurveInterpFormPoints(true, 0, u).first;
    vcurve[1] = this->bezCurveInterpFormPoints(true, 1, u).first;
    vcurve[2] = this->bezCurveInterpFormPoints(true, 2, u).first;
    vcurve[3] = this->bezCurveInterpFormPoints(true, 3, u).first;

    // Build Control Points for Bezier Curve in U
    ucurve[0] = this->bezCurveInterpFormPoints(false, 0, v).first;
    ucurve[1] = this->bezCurveInterpFormPoints(false, 1, v).first;
    ucurve[2] = this->bezCurveInterpFormPoints(false, 2, v).first;
    ucurve[3] = this->bezCurveInterpFormPoints(false, 3, v).first;

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

    return pointNormal;
}

