// SVV.cpp: implementation of the CSVV class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SVV.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define M_PI	3.1415926535897932384626433832795

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSVV::CSVV(long rho)
{
	rho_min = rho;
	btrX = NULL;
	btrY = NULL;
	btrIX = NULL;
	btrIY = NULL;
}

CSVV::~CSVV()
{
	if (btrIY != NULL)
	{
		delete [] btrIY;
		btrIY = NULL;
	}
	if (btrIX != NULL)
	{
		delete [] btrIX;
		btrIX = NULL;
	}
	if (btrY != NULL)
	{
		delete [] btrY;
		btrY = NULL;
	}
	if (btrX != NULL)
	{
		delete [] btrX;
		btrX = NULL;
	}
}

// Img: source Img
// w: width, u: rho v: theta
BYTE * CSVV::LPtr(BYTE *Img, int u, int v)
{
	int k;

	BYTE *rImg = new BYTE[u*v];
	for(k=0;k<u*v;k++)
		rImg[k] = 0;

	for(k=0;k<u*v;k++)
		rImg[k] = Img[btrY[k]*m_w+btrX[k]];

	return rImg;
}

/*/ DJ
void CImageTransform::MyLPGray2()
{
	CProgressBar bar(_T("LP º¯È¯"), 100, m_nHeight);
	CPixelPtr ptrSorce(m_pImage->GetUndoHandle());
	CPixelPtr ptrDest(*m_pImage);
	CPixel p;

	double x0, y0;
	double x, y;
	double r, th1;
	double ro, th2;
	double ro_max;
	double lpx, lpy;
	int i,j;

	lpx = m_nWidth;
	lpy = m_nHeight;

	x0 = (double)m_nWidth/2;
	y0 = (double)m_nHeight/2;
	ro_max = log(sqrt(x0*x0+y0*y0));
	//ro_max = sqrt(x0*x0+y0*y0);

	for( j=0 ; j<m_nHeight ; j++) 
		for( i=0 ; i<m_nWidth ; i++) {
			p = 0;
			p >> ptrDest[j][i];
		}

	for( j=0 ; j<m_nHeight ; j++) 
	{
		for( i=0 ; i<m_nWidth ; i++)
		{
			p << ptrSorce[j][i];

			x = (double)i-x0;
			y = (double)j-y0;

			r = sqrt(x*x+y*y);
			th1 = atan2(x,y);

			ro = floor((r > 1.0) ? (double)(lpx-1)*(log(r)/ro_max) : 0.0);
			//ro = floor((r > 1.0) ? (double)(lpx-1)*(r/ro_max) : 0.0);
			th2 = floor((double)(lpy-1)*((th1+M_PI)/(2*M_PI)));

			p >> ptrDest[(int)th2][(int)ro];
		}
		bar.StepIt();
	}
}*/

void CSVV::LPtable(int w, int h, int u, int v)
{
	m_w = w;
	m_h = h;
	m_u = u;
	m_v = v;

	int i,j;
	long x;		// X-value in Cartesian coordinate (unit: pixel)
	long y;		// Y-value in Cartesian coordinate (unit: pixel)
	long x0;		// Center X-value in Cartesian coordinate (unit: pixel)
	long y0;		// Center Y-value in Cartesian coordinate (unit: pixel)

	long rho;	// radius in Log-Polar Image (unit: pixel)
	long tht;	// angle  in Log-Polar Image (unit: pixel)
	long rho_max;//32;//2*64;//100;//64*4;
	long tht_max;//64;//2*64;//200;//64*4;
	int index;

	long tht_min;
	long sourceSize;	

	double pi=M_PI;
	double r;	// X-value in polar coordinate (unit: pixel)
	double phi;	// Y-value in polar coordinate (unit: pixel)
	double m;
	double det;
	double k;
	double q;

	btrX = new long[u*v];
	btrY = new long[u*v];
	btrIX = new long[w*h];
	btrIY = new long[w*h];

	x0 = w/2;	// Center X-value in Cartesian coordinate (unit: pixel)
	y0 = h/2;	// Center Y-value in Cartesian coordinate (unit: pixel)

	rho_max = u;
	tht_max = v;

	tht_min = 0;

	sourceSize = min(w, h);

	m   = (2.0*pi)/(double)tht_max;
	det = -log((double)rho_min) + log((double)(sourceSize)/2.0);
	k   = det / (double)(- rho_min + rho_max);
	q   = (-log((double)sourceSize/2.0) * (double)rho_min 
		+ log((double)rho_min) * (double)rho_max) / det;

	// log-polar
	index = 0;
	for(rho = 0 ; rho < rho_min ; rho++) 
		for(tht = 0 ; tht < tht_max ; tht++, index++) {
			r = (double)rho;
			phi = m * (double)tht;
			x = (int)(r * cos(phi)) + x0;
			y = -(int)(r * sin(phi)) + y0;

			btrX[index] = x;
			btrY[index] = y;
		}
		
	for(rho = rho_min ; rho < rho_max ; rho++) 
		for(tht = 0 ; tht < tht_max ; tht++,index++) {
			r = exp(k * ((double)rho+q));
			phi = m * (double)tht;
			x = (int)(r * cos(phi)) + x0;
			y = -(int)(r * sin(phi)) + y0;
			
			btrX[index] = x;
			btrY[index] = y;
		}

	double mm;
	// inverse log-polar
	index=0;
	for(j=0;j<sourceSize;j++)
		for(i=0;i<sourceSize;i++,index++)
		{
			x=i-sourceSize/2;
			y=-(j-sourceSize/2);
			r = (long)sqrt(x*x+y*y);
			
			if (y==0)
			{
				if (x>0)
					phi=0.0;
				else
					phi=M_PI;
			}
			else
			{
				if (y>0)
					phi=atan2(y,x);
				else
					phi=atan2(y,x)+2*M_PI;
			}

			phi /= m;

			if (r <= rho_min)
			{
				btrIX[index] = (long)r;
				btrIY[index] = (long)phi;
			}
			else
			{
				//btrIX[index] = log(r-rho_min)/log((double)(sourceSize/(2.0))-rho_min)*(rho_max-rho_min)+rho_min;
				if (r>(double)(sourceSize/(2.0)))
					r = (double)(sourceSize/(2.0))-1.0; 


				mm = log(r)/log((double)(sourceSize/(2.0)))*rho_max;
				btrIX[index] = (long)mm;
				btrIY[index] = (long)phi;
			}
		}
}

BYTE * CSVV::LPtrInv(BYTE *Img, int w, int h)
{
	int k;

	BYTE *rImg = new BYTE[w*h];
	for(k=0;k<w*h;k++) 
		rImg[k] = 0;

	for(k=0;k<w*h;k++)
		rImg[k] = Img[btrIX[k]*m_v+btrIY[k]];
		//rImg[btrX[k]+btrY[k]*w] = Img[k];

	return rImg;
}
