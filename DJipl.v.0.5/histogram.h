#ifndef _HISTDEF_
#define _HISTDEF_
#include <math.h>
#include "cimage.h"

class Hist1d {
	// 20010601@10:08@DJ
	// Otsu's Method
	float *pbucket;
	float *mbucket;
	float *wbucket;
	float *sbucket;

	// 20010605@01:47@DJ
	// Otsu's Method (2nd)
	float ***w_bucket;
	float ***m_bucket;
	float **s_bucket;

    int *bucket;
    int nbuckets;
    int min, max;

	double features[6];

	double calcMean();
	double calcSD();
	double calcSkewness();
	double calcKurtosis();
	double calcEnergy();
	double calcEntropy();

	void clearfeatures();
public:
	enum HistFeature {Mean=0, SD, Skewness, Kurtosis, Energy, Entropy};
	int getMax() {return max;}
	int getMin() {return min;}
	int getLength() {return nbuckets;}
	int *gets() {return bucket;}
	void set(int m=0, int M=0, int n=0);

	Hist1d(int m=65536, int M=-1, int n=256);
	~Hist1d();	

	void clearhist();
	void addtohist(register unsigned char *sptr, register int n);
	void addtohist(register unsigned short *sptr, register int n);

	// 20010605@15:04@DJ
	void DJotsu(int *th0);
	void DJotsu(int *th0, int *th1);
	void DJkittler(int *th0);
	void DJfuzzy_entropy(int *th0, unsigned char *ptr, int n);

//	void showhist();
	void histeqtable(short *tab);
	double getMean() {return (features[Mean] < 0 ? calcMean():features[Mean]);}
	double getSD() 	 {return (features[SD] < 0 ? calcSD():features[SD]);}
	double getSkewness() {return (features[Skewness] < 0 ? 
						calcSkewness():features[Skewness]);}
	double getKurtosis() {return (features[Kurtosis] < 0 ? 
						calcKurtosis():features[Kurtosis]);}
	double getEnergy()  {return calcEnergy();}
	double getEntropy() {return calcEntropy();}

	// print format mode = 1 (%d), mode= 2 (%d %d)
	int save(char *filename, int mode = 2, int verbose = 0);
};

class Hist2d {
	int M, N; 			//  M x N
	int *bucket;

public:
	Hist2d(int m=0, int n=0, int *d=NULL);
	~Hist2d();
	int m() {return M;}
	int n() {return N;}
	int *gets() {return bucket;}
	void print();

	void clearhist();

	double calcAutocorrelation();
	double calcCovar();
	double calcInertia();
	double calcAbsValue();
	double calcHomogeneity();
	double calcInvDif();		// Inverse Difference Moment
	double calcGenInvDif(int k, int l);		// Inverse Difference Moment
	double calcEnergy();
	double calcEntropy();

	double calcUniformity();	// Uniformity of energy
	int    calcMaximum();		// Maximum Probability
	double calcGenContrast(int k, int l);
//	double calcCorrelation();
//	double calcProbRunLength();
	double calcClusterTendency(int k = 1);
};

Hist2d *makeCooccurenceMatrix(GrayImage *g, double d, double deg, 
				int size = 256, int verbose = 0);

#endif
