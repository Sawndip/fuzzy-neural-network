
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cimage.h"
#include "histogram.h"


Hist2d::Hist2d(int m, int n, int *d)
{
	M = m; N = n; bucket = d;
}

Hist2d::~Hist2d()
{
	delete bucket;
}

void Hist2d::clearhist()
{
    int *ptr = bucket;
	int mn = M*N;
	for (int b = 0; b < mn; b++)
		*ptr++ = 0;
}

double Hist2d::calcAutocorrelation()
{
	double dsum = 0.0;
	int *p = bucket;
	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++)
			dsum += (double)(a * b * (*p++));

	return (dsum / (double)(M * N));
}

double Hist2d::calcCovar()
{
	int *p = bucket;
	double dsum = 0.0, asum = 0.0, bsum = 0.0;
	int mn = M *N;
	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++, p++) {
			asum += (double)(a * (*p));
			bsum += (double)(b * (*p));
		}
	double amean = dsum / ((double) mn);
	double bmean = dsum / ((double) mn);

	double atmp, btmp;
	for (a = 0; a < M; a++)
		for (b = 0; b < N; b++) {
			atmp = (double)a - amean;
			btmp = (double)b - bmean;
			dsum += atmp * btmp * (double) (*p++);
		}

	return (dsum / (double)mn);
}

double Hist2d::calcInertia()
{
	int *p = bucket;
	double dsum = 0.0, dtmp;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)(a - b);
			dsum += dtmp * dtmp * (double)(*p++);
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcAbsValue()
{
	int *p = bucket;
	double dsum = 0.0;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dsum += abs(a - b) * (*p++);
		}

	return (dsum / (double)(M * N));
}

// inverse difference
double Hist2d::calcInvDif()
{
	int *p = bucket;
	double dsum = 0.0, dtmp;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)a - (double) b;
			dsum += (double) (*p++) / ((double)1.0 + dtmp * dtmp);
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcHomogeneity()
{
	int *p = bucket;
	double dsum = 0.0;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dsum += (double) (*p++) / ((double)1.0 + (double) abs(a-b));
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcEnergy()
{
	int *p = bucket;
	double dsum = 0.0, dtmp;

	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)(*p ++);
			dsum += dtmp * dtmp; 
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcEntropy()
{
	int *p = bucket;
	double dsum = 0.0;
	double dtmp;
	double log2 = log(2.0);
	double mn = 1.0 / (double)(M*N);
	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++) {
			dtmp = (double)(*p++) * mn;
			dsum += dtmp * log (dtmp); 
		}

	return (- dsum / log2);
}

double Hist2d::calcUniformity()
{
	return (calcEnergy() * M * N);
}

int Hist2d::calcMaximum()
{
	int *p = bucket;
	int	maxi = -10000;
	for (int a = 0; a < M; a++) 
		for (int b = 0; b < N; b++, p++) {
			if (maxi < (*p));
				maxi = *p;
		}
	return maxi;
}

double Hist2d::calcGenContrast(int k, int l)
{
	int *p = bucket;
	double	rst = 0.0;

	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++) {
			rst += pow((double) abs(a-b), k) * pow(double(*p++), l);
		}
	return rst;
}

double Hist2d::calcGenInvDif(int k, int l)
{
	int *p = bucket;
	double dsum = 0.0;
	double dtmp;

	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++) {
			if (a != b) {
				dtmp = (double)a - (double) b;
				dsum += pow((double) (*p++),l) / pow(dtmp, k); 
			}
		}

	return (dsum / (double)(M * N));
}

double Hist2d::calcClusterTendency(int k)
{
	int *p = bucket;
	double dsum = 0.0, asum = 0.0, bsum = 0.0;
	int mn = M*N;

	for (int a = 0; a < M; a++)
		for (int b = 0; b < N; b++, p++) {
			asum += a * (*p);
			bsum += b * (*p);
		}
	double amean = asum / ((double) mn);
	double bmean = bsum / ((double) mn);

	p = bucket;
	for (a = 0; a < M; a++) 
		for (b = 0; b < N; b++) {
			dsum += pow((double)(a + b - 2.0 * amean), k) *(double) (*p++);
		}

	return (dsum / (double) mn);
}

void Hist2d::print()
{
	int *p = bucket;
	for (int r = 0; r < m(); r++) {
		for (int c = 0; c < n(); c++)
			printf ("%d ", *p++);
		printf("\n");
	}
}

Hist2d *makeCooccurenceMatrix(GrayImage *g, double d, double deg, int size,
 int verbose)
{
	Hist2d *co_matrix = new Hist2d(size, size, new int[size*size]);	
	co_matrix->clearhist();
	int *p = co_matrix->gets();
	int *pp;
	int x, y, xx, yy; int r, c;
	int dx = (int)rint(d * cos(deg*M_PI/180.0));
	int	dy = (int)rint(d * sin(deg*M_PI/180.0));

	if (verbose)
		printf ("%d degree: dx = %d, dy = %d\n", (int) deg, dx, dy);
	for (y = 0; y < g->h(); y++)
		for (x = 0; x < g->w(); x++) {
			xx =  x + dx; yy =  y + dy;
			if (yy < co_matrix->m() && xx < co_matrix->n() &&
				yy >= 0 && xx >= 0) {
				r = g->getPixel(x, y); c = g->getPixel(xx, yy);
				pp = p + c + r * co_matrix->m();
				(*pp)  ++;
			}
			if (debug) {
				printf ("(%d,%d) r,c = (%d,%d)\n", x, y,  r,c);
				co_matrix->print();
			}
			xx =  x - dx; yy =  y - dy;
			if (yy < co_matrix->m() && xx < co_matrix->n() &&
				yy >= 0 && xx >= 0) {
				r = g->getPixel(x, y); c = g->getPixel(xx, yy);
				pp = p + c + r * co_matrix->m();
				(*pp)  ++;
			}
			if (debug) {
				printf ("(%d,%d) r,c = (%d,%d)\n", x, y,  r,c);
				co_matrix->print();
			}
		}
	return co_matrix;
}
