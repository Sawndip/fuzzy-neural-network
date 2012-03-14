// OpticalFlow.h: interface for the OpticalFlow class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPTICALFLOW_H_
#define _OPTICALFLOW_H_

#include "cimage.h"

class OpticalFlow  
{
public:
	OpticalFlow();
	virtual ~OpticalFlow();

	void Horn(GrayImage A, GrayImage B, int grid, double r);
	void Horn(GrayImage A, GrayImage B, int grid, double r, int sample, int iter);
	void Horn(GrayImage A, GrayImage B, GrayImage *C, int grid, double r);
	void March(GrayImage A, GrayImage B, int grid, double r);

};

#endif
