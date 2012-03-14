// GaborFilter.h: interface for the GaborFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GABORFILTER_H__FE764580_9EFF_11D4_B766_00E0294BE14D__INCLUDED_)
#define AFX_GABORFILTER_H__FE764580_9EFF_11D4_B766_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "highgui.h"

typedef struct {
  unsigned int hsize, vsize;
  double sum;
  double *co;
} Filter_mask;

typedef struct {
  unsigned int hsize, vsize;
  float sum;
  float *co;
} Filter_mask2;

class GaborFilter  
{
public:
	GaborFilter();
	virtual ~GaborFilter();

	// imported gabor filter routines
	void making_gabor_filter(double omega,double theta,double sigma_x,double sigma_y,Filter_mask *real_part,Filter_mask *img_part);
	// from IEEE PAMI oct. 1999.
	// G. donato et al.
	// Classfying Facial Actions
	// eq. 16 - 18
	void making_gabor_filter2(double omega,double theta,double sigma_x,double sigma_y,Filter_mask *real_part,Filter_mask *img_part);
	// PRL 22(2001) 731-739
	// W.Chan et al.'s definition
	void making_gabor_filter3(double u0, double br, double bth, double th, Filter_mask *real_part,Filter_mask *img_part);
	// float version
	// 02/04/15
	void making_gabor_filter4(float u0, float br, float bth, float th, Filter_mask2 *real_part,Filter_mask2 *img_part);

	double gaussian(double x, double y, double sigma_x, double sigma_y, double omega);
	int horizontal_filter_size(double theta, double sigma_x, double sigma_y);
	int vertical_filter_size(double theta,double sigma_x, double sigma_y);
	//double *gaborCo(GrayImage A, double wave, double theta);
	double *gaborCo(IPLIMAGE A, double wave, double theta);
	// double type projection.
	// 0 for horizontal
	// 1 for vertical 
	double *projCo(double *orgCo, int w, int h, int dir = 0);

	void gaborFiltering(IPLIMAGE src, IPLIMAGE dst, float wave, float theta);
};

#endif // !defined(AFX_GABORFILTER_H__FE764580_9EFF_11D4_B766_00E0294BE14D__INCLUDED_)
