#ifndef _QV_MOMENT_
#define _QV_MOMENT_
//
// FILENAME: moment.h
// DISCRIPTION: 2 dimensional moments
//

#include <math.h>
#include "cimage.h"

enum Bool { False, True};

#define RAD2DEG(x)	((x) * (double) 180.0 / M_PI)
#define sqr(x) ((x) * (x))

class Moment {
	GrayImage *img;
	// tempory calc
	double center[2];	// scaled center
	double ucenter[2];	// unscaled center

	double eigenvalues[2];
	double orientation;
	double aspectratio;
	double tan2o;
	double h[7];			// 7 invariant features
	// equal & zero
   	double tol;      // tolerence about zero
	Bool equals(double x, double y,double tolerance)
			{ return iszero( x - y, tolerance);}
	Bool equals(double x, double y)
			{ return iszero( x - y);}
	Bool iszero(double x, double tolerance) 
                { return ((fabs (x) < tolerance) ? True: False); }
	Bool iszero(double x) 
                { return ((fabs (x) < tol) ? True: False); }

	double max2(double a, double b) { return (a > b ? a : b);}
	double min2(double a, double b) { return (a < b ? a : b);}

	int calcEigens();
public:
	Moment(GrayImage *gi=NULL, float t=1.0e-10)
								{img=gi; center[0] = center[1] = -1; tol = t;}
	~Moment(){}

	void setImage(GrayImage *gi){img=gi;center[0] = center[1] = -1;}
	GrayImage *getImage(){ return img;}

	//
	double get(int m, int n, double cx = 0.0, double cy = 0.0);	
													// scaled spatial moment
	double getU(int m, int n, double cx = 0.0, double cy = 0.0);	
												  // unscaled spatial moment
	double getC(int m, int n);	// scaled spatial central moment
	double* getCenter(); 
	double getCenterX() { return getCenter()[0];}
	double getCenterY() { return getCenter()[1];}
	double getUCenterX() { return ucenter[0];}
	double getUCenterY() { return ucenter[1];}
	
	double *getEigenValues(int recalc = 1);
	double getOriAngle(int recalc = 1);
	double getAspectRatio(int recalc = 1);
	double getTan2O(int recalc = 1);
	double *calcInvariantFeatures();
	double getInvariantFeature(int n, int recalc=1)
							{if (recalc) calcInvariantFeatures();return h[n];}
};
#endif
