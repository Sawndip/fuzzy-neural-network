#include "cimage.h"

#define  FILTER_SIZE_RATIO_SIGMA 3.0
#define MASK 0xFF
#define OFFSET 1000000.0   

typedef struct {
  unsigned int hsize, vsize;
  double sum;
  long **co;
} Filter_mask;

void fusion(int width,int height,long int **w0,long int **w1,GrayImage dest);
GrayImage process(GrayImage in,int width,int height,Filter_mask *real_part,
	       Filter_mask *img_part);
void making_gabor_filter(double omega,double theta,double sigma_x,double sigma_y,Filter_mask *real_part,Filter_mask *img_part);
long **conv(int width,int height,GrayImage in, Filter_mask *fil);

