// OpticalFlow.cpp: implementation of the OpticalFlow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "OpticalFlow.h"
#include "ImgMisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OpticalFlow::OpticalFlow()
{

}

OpticalFlow::~OpticalFlow()
{

}

void OpticalFlow::Horn(GrayImage A, GrayImage B, int grid, double r)
{
	double *U, *V;

	int i, j, k, m, n;
	int offset = grid/2;
	int size = grid*grid;

	double vx = 0, vy = 0;
	double mvx = 0, mvy = 0;
	double fx, fy, ft;

	U = new double[A.w()*A.h()];
	V = new double[A.w()*A.h()];

	for (k=0; k<A.w()*A.h(); k++)
		U[k] = V[k] = 0.0;

	for (int l=0; l<1; l++)
	{
	k = 0;
	for (j=0; j<A.h(); j++)
		for (i=0; i<A.w() && k<A.w()*A.h(); i++, k+= 8)
		{
			ft = A.buf[0][k] - B.buf[0][k];
			if (k%A.w() != 0)
				fx = A.buf[0][k] - A.buf[0][k-1];
			else
				fx = 0.0;
			if (k > A.w())
				fy = A.buf[0][k] - A.buf[0][k-A.w()];
			else
				fy = 0.0;

			mvx = mvy = 0;
			for (m=0; m<grid; m++)
				for (n=0; n<grid; n++)
				{
					if (k-offset+n-(offset-m)*A.w() >= 0 &&
						k-offset+n-(offset-m)*A.w() < A.w()*A.h())
					{
						mvx += U[k-offset+n-(offset-m)*A.w()];
						mvy += V[k-offset+n-(offset-m)*A.w()];
					}
				}

			vx = mvx - fx*(fx*mvx+fy*mvy+ft)/(r+fx*fx+fy*fy);
			vy = mvy - fy*(fx*mvx+fy*mvy+ft)/(r+fx*fx+fy*fy);

			U[k] = vx;
			V[k] = vy;
		}
}

	CImgMisc::Normalize(U, A.w(), A.h());
	CImgMisc::Normalize(V, A.w(), A.h());

	delete []V;
	delete []U;
}

void OpticalFlow::Horn(GrayImage A, GrayImage B, int grid, double r, int sample, int iter)
{
	double **U, **V;
	double **Uc, **Vc;
	int w = A.w();
	int h = A.h();
	int mw = w/sample;
	int mh = h/sample;

	int i, j, k, m, n;
	int ii, jj;
	int offset = grid/2;
	int size = grid*grid;

	double vx = 0, vy = 0;
	double mvx = 0, mvy = 0;
	double fx, fy, ft;

	U = new double *[mh];
	V = new double *[mh];
	Uc = new double *[mh];
	Vc = new double *[mh];

	double *Ux = new double[w*h];
	double *Vx = new double[w*h];

	for (i=0; i<mh; i++)
	{
		U[i] = new double[mw];
		V[i] = new double[mw];
		Uc[i] = new double[mw];
		Vc[i] = new double[mw];
	}

	for (j=0; j<mh; j++)
		for (i=0; i<mw; i++)
		{
			U[j][i] = 0.0;
			V[j][i] = 0.0;
		}

	for (int l=0; l<iter; l++)
	{
		for (j=0; j<h; j += sample)
			for (i=0; i<w; i += sample)
			{
				ii = i/sample;
				jj = j/sample;

				k = i+j*w;
				ft = A.buf[0][k] - B.buf[0][k];
				
				if (i > 0)
					fx = A.buf[0][k] - A.buf[0][k-1];
				else
					fx = 0.0;
				if (j > 0)
					fy = A.buf[0][k] - A.buf[0][k-w];
				else
					fy = 0.0;

				mvx = mvy = 0;
				for (m=0; m<grid; m++)
					for (n=0; n<grid; n++)
					{
						if ((i > offset && i < w-offset-1) &&
							(j > offset && j < h-offset-1))
						{
							//mvx += U[jj-offset+m][ii-offset+n];
							mvx += U[jj-offset+m][ii-offset+n];
							//mvy += V[jj-offset+m][ii-offset+n];
							mvy += V[jj-offset+m][ii-offset+n];
						}
					}

				vx = mvx - fx*(fx*mvx+fy*mvy+ft)/(r+fx*fx+fy*fy);
				vy = mvy - fy*(fx*mvx+fy*mvy+ft)/(r+fx*fx+fy*fy);

				Uc[jj][ii] = vx;
				Vc[jj][ii] = vy;
			}

		for (j=0; j<mh; j++)
			for (i=0; i<mw; i++)
			{
				U[j][i] = Uc[j][i];
				V[j][i] = Vc[j][i];
			}
	}

	for (i=0; i<w*h; i++)
		Ux[i] = Vx[i] = 0.0;

	for (j=0; j<mh; j++)
		for (i=0; i<mw; i++)
		{
			k = (i+j*mw)*sample;
			Ux[k] = U[j][i];
			Vx[k] = V[j][i];
		}
	
	CImgMisc::Normalize(Ux, A.w(), A.h());
	CImgMisc::Normalize(Vx, A.w(), A.h());

	delete []Vx;
	delete []Ux;

	delete []Vc;
	delete []Uc;
	delete []V;
	delete []U;
}

void OpticalFlow::Horn(GrayImage A, GrayImage B, GrayImage *C, int grid, double r)
{
	double *U, *V;

	int i, j, k, m, n;
	int offset = grid/2;
	int size = grid*grid;

	double vx = 0, vy = 0;
	double mvx = 0, mvy = 0;
	double fx, fy, ft;

	U = new double[A.w()*A.h()];
	V = new double[A.w()*A.h()];

	for (k=0; k<A.w()*A.h(); k++)
		U[k] = V[k] = 0.0;

	for (int l = 0; l<10; l++)
	{

	k = 0;
	for (j=0; j<A.h(); j++)
		for (i=0; i<A.w(); i++, k++)
		{
			ft = A.buf[0][k] - B.buf[0][k];
			if (k%A.w() != 0)
				fx = A.buf[0][k] - A.buf[0][k-1];
			else
				fx = 0.0;
			if (k > A.w())
				fy = A.buf[0][k] - A.buf[0][k-A.w()];
			else
				fy = 0.0;

			mvx = mvy = 0;
			for (m=0; m<grid; m++)
				for (n=0; n<grid; n++)
				{
					if (k-offset+n-(offset-m)*A.w() >= 0 &&
						k-offset+n-(offset-m)*A.w() < A.w()*A.h())
					{
						mvx += U[k-offset+n-(offset-m)*A.w()];
						mvy += V[k-offset+n-(offset-m)*A.w()];
					}
				}

			vx = mvx - fx*(fx*mvx+fy*mvy+ft)/(r+fx*fx+fy*fy);
			vy = mvy - fy*(fx*mvx+fy*mvy+ft)/(r+fx*fx+fy*fy);

			U[k] = vx;
			V[k] = vy;

		}
	}

	C->buf[0][k] = (unsigned char)(vx*vx + vy*vy);

//	CImgMisc::Normalize(U, A.w(), A.h());
//	CImgMisc::Normalize(V, A.w(), A.h());

	delete []V;
	delete []U;
}

void OpticalFlow::March(GrayImage A, GrayImage B, int grid, double r)
{
}
