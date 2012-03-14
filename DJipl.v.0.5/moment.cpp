//
//	FILENAME: moment.c++
//

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <iostream.h>
#include "cimage.h"
#include "moment.h"

inline double POW(double x, int y)
{
	if (y == 0) return (double)1.0;
	if (y == 1) return x;
	return pow(x, (double)y);
}

double Moment::getU(int m, int n, double cx, double cy)
{
	int i, j;
	int N, M;
	unsigned char *p = img->buf[0];
	N = img->height; M = img->width;
	double xi, yj;	
	double rst = 0.0;

	for (j = 0; j < N; j++)
		for (i = 0; i < M; i++) {
			xi = POW((double)i + (double)0.5 - cx, m);
			yj = POW((double)j + (double)0.5 - cy, n);
			
			rst += xi * yj * (double) (*p);
			p++;
		}

	return rst;
}

double Moment::get(int m, int n, double cx, double cy)
{
	int N = img->height, M = img->width;
	return getU(m, n, cx, cy)/(POW(M, m) * POW(N, n));
}

double *Moment::getCenter()
{
	double m00 = get(0, 0);
	double m10 = get(1, 0);
	double m01 = get(0, 1);
	center[0] = m10 / m00;		// x center
	center[1] = m01 / m00;		// y center
	ucenter[0] = center[0] * img->width;
	ucenter[1] = center[1] * img->height;
	return center;
}

double Moment::getC(int m, int n)
{
	if (equals(center[0], -1.0)) getCenter();
	return get(m, n, ucenter[0], ucenter[1]);
}

int Moment::calcEigens()
{
	double u20 = getC(2, 0);
	double u11 = getC(1, 1);
	double u02 = getC(0, 2);
	double lM, lN;	// eigenvalues
	double a, b;
	
	a = (u20 +u02) * 0.5;
	b = sqrt(u20*u20 +u02*u02 - 2.0*u20*u02 + 4.0 * u11 * u11) * 0.5;
	eigenvalues[0] = a + b;
	eigenvalues[1] = a - b;
	lM = eigenvalues[0] > eigenvalues[1] ? eigenvalues[0] : eigenvalues[1]; 
	lN = eigenvalues[0] < eigenvalues[1] ? eigenvalues[0] : eigenvalues[1]; 

	orientation = atan((lM - u02) / u11);
	aspectratio = lN / lM;
	tan2o = tan(2 * u11 / (u20 - u02));
	return  0;
}

double *Moment::getEigenValues(int recalc)
{
	if (recalc) calcEigens();
	return  eigenvalues;
}

double Moment::getOriAngle(int recalc)
{
	if (recalc) calcEigens();
	return orientation;
}

double Moment::getAspectRatio(int recalc)
{
	if (recalc) calcEigens();
	return aspectratio;
}

double Moment::getTan2O(int recalc)
{
	if (recalc) calcEigens();
	return tan2o;
}

double *Moment::calcInvariantFeatures()
{
	double m00 = get(0,0);
	double alpha;
	double V[4][4];
	double U_u[4][4];
	double den;
	int m,n;
	int N = img->height, M = img->width;

	for (n = 0; n < 4; n++)
		for (m = 0; m < 4; m++) {
			alpha = (double)(m + n + 2 ) * 0.5;
			den = pow(m00, alpha);
			U_u[m][n] = getC(m,n) * POW(M, m) * POW(N, n);
			V[m][n] = U_u[m][n]/den;
		}

	h[0] = V[2][0] + V[0][2];
	h[1] = sqr(V[2][0] - V[0][2]) + 4.0 * sqr(V[1][1]);
	h[2] = sqr(V[3][0] - 3.0 * V[1][2]) + sqr(V[0][3] - 3.0 * V[2][1]);
	h[3] = sqr(V[3][0] + V[1][2]) + sqr(V[0][3] - V[2][1]);
	h[4] = (V[3][0] - 3.0 * V[1][2]) * (V[3][0] + V[1][2])   *    (
			sqr(V[3][0] + V[1][2]) - 3.0 * sqr(V[0][3] + V[2][1]) )
			+ (3.0 * V[2][1] - V[0][3]) * (V[0][3] + V[2][1]) *   (
			3.0 * sqr(V[3][0] + V[1][2]) - sqr(V[0][3] + V[2][1]) );
	h[5] = (V[2][0] - V[0][2]) * (sqr(V[3][0] + V[1][2]) - sqr(V[0][3] +
V[2][1])) + 4.0 * V[1][1] * (V[3][0] + V[1][2]) * (V[0][3] + V[2][1]);
	h[6] = (3.0 * V[2][1] - V[0][3])*(V[3][0] + V[1][2])     *    (
			sqr(V[3][0] + V[1][2]) - 3.0 * sqr(V[0][3] + V[2][1]) )
			+ (3.0 * V[1][2] - V[3][0]) * (V[0][3] + V[2][1]) *   (
			3.0 * sqr(V[3][0] + V[1][2]) - sqr(V[0][3] + V[2][1]) );
	return h;
}

