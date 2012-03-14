#include "cimage.h"
#include "gabor.h"
#include <math.h>

//#if (PGM_MAXMAXVAL >255)
//#define MAX_VAL (PGM_MAXMAXVAL/257)
//#endif
//#define	PI 3.141592653589793238
#define NUL 0.0     
#define SIGMA_X  10.0 
#define SIGMA_Y  10.0 
#define OMEGA 0.2
#define THETA 45.0
#define FILTER_NUM 20

int **noise_plane(GrayImage mem,GrayImage mem2,int width,int height,int *max_of_gap);

int **make_filterbank(GrayImage in,int width,int height,FILE *fpo2);

GrayImage make_noise(GrayImage in,int **bank,int **gap,
		  int width,int height,int max_of_gap);

// gabor image
GrayImage gabor(GrayImage in,int width,int height,double degree,double sigma_x,
	     double sigma_y,double f);