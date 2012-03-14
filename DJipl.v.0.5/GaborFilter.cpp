// GaborFilter.cpp: implementation of the GaborFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "GaborFilter.h"
#include "cimage.h"
#include "math.h"
#include "ImgMisc.h"

#define M_PI	3.1415926535897932384626433832795

//#define  FILTER_SIZE_RATIO_SIGMA 3.0
#define  FILTER_SIZE_RATIO_SIGMA 3.0
//#define OFFSET 1000000.0   
#define OFFSET 1000000.0   

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GaborFilter::GaborFilter()
{

}

GaborFilter::~GaborFilter()
{

}

// edited by DJ
// dynamic vision pp.312-313
// by S. gong et al.
void GaborFilter::making_gabor_filter(double omega,double theta,double sigma_x,double sigma_y,Filter_mask *real_part,Filter_mask *img_part)
{
	int k,h_half,v_half;
	unsigned int h, v;
	double h_sum, v_sum;
	double h_, v_,h_1,v_1, ch, cv;
	double gauss;
	double costh = cos(theta), sinth = sin(theta);

	if (sigma_x < 0.1 && sigma_y < 0.1)	exit(1);

	h_half = horizontal_filter_size(theta,sigma_x, sigma_y);
	v_half = vertical_filter_size(theta,sigma_x, sigma_y);

	real_part->hsize = 2*h_half+1;
	real_part->vsize = 2*v_half+1;
	img_part->hsize = real_part->hsize;
	img_part->vsize = real_part->vsize;

	real_part->co = new double[real_part->vsize*real_part->hsize];
	img_part->co = new double[img_part->vsize*img_part->hsize];

	h_sum = 0; 
	v_sum = 0; 

	double omg2 = omega*omega;

	k=0;
	for(v= 0; v < real_part->vsize; v++)
		for(h= 0; h < real_part->hsize; h++,k++)
		{
			h_1 = h-h_half;
			v_1 = v-v_half;

			h_ = ((double)h_1)*costh + ((double)v_1) *sinth;
			v_ = -((double)h_1)*sinth + ((double)v_1) *costh;  
			/**  h_1 = ((double)h)*cos(theta) + ((double)v) *(-sin(theta));
			v_1 = ((double)h)*sin(theta) + ((double)v) *cos(theta); **/

			gauss = gaussian(h_,v_, sigma_x, sigma_y, omg2);
			//ch = gauss  * cos(2.0*M_PI*omega*h_);
			//ch = gauss*(cos(omega*(h_+v_))-exp(-sigma_x*sigma_y/2.0));
			ch = gauss*(cos(omega*(h_))-exp(-sigma_x*sigma_y/2.0));
			h_sum += fabs(ch);
			//cv = gauss  * sin(2.0*M_PI*omega*h_);
			//cv = gauss*sin(omega*(h_+v_));
			cv = gauss*sin(omega*(h_));
			v_sum += fabs(cv);

			real_part->co[k] = ch;
			img_part->co[k] = cv; 
		}


	real_part->sum = h_sum;
	img_part->sum = v_sum;
}

// edited by DJ
void GaborFilter::making_gabor_filter3(double u0, double br, double bth, double th, Filter_mask *real_part, Filter_mask *img_part)
{
	int k, mx, my;
	int x, y;
	int dx, dy;
	double rx, ry, gx, gy;
	double minx, miny, maxx, maxy;
	
	double h_sum, v_sum;
	double gauss;
	double costh = cos(th), sinth = sin(th);
	double a = pow(2, br);
	double sigma_x = ((sqrt(2)*u0)/(2*M_PI))*((a + 1)/(a - 1));
	double sigma_y = (sqrt(2)*u0)/(2*M_PI*tan(bth/2));

	//mx = my = (int)5;
	mx = my = (int)u0;

	real_part->hsize = 2*mx+1;
	real_part->vsize = 2*my+1;
	img_part->hsize = real_part->hsize;
	img_part->vsize = real_part->vsize;

	real_part->co = new double[real_part->vsize*real_part->hsize];
	img_part->co = new double[img_part->vsize*img_part->hsize];

	h_sum = 0; 
	v_sum = 0; 

	minx = miny = 1e5;
	maxx = maxy = -1e5;
	k=0;
	for(y= 0; y < real_part->vsize; y++)
		for(x= 0; x < real_part->hsize; x++, k++)
		{
			dx = x - mx;
			dy = y - my;

			rx = ((double)dx*costh) + ((double)dy*sinth);
			ry = -((double)dx*sinth) + ((double)dy*costh);  

			gauss = exp(-0.5*(((rx*rx)/(sigma_x*sigma_x)) + ((ry*ry)/(sigma_y*sigma_y))));

			gx = gauss*(cos((2*M_PI*rx)/u0)-exp(-0.5*(sigma_x*sigma_y)));
			h_sum += fabs(gx);
			gy = gauss*sin((2*M_PI*rx)/u0);
			v_sum += fabs(gy);

			if (gx < minx)	minx = gx;
			if (gx > maxx)	maxx = gx;
			if (gy < miny)	miny = gy;
			if (gy > maxy)	maxy = gy;

			real_part->co[k] = gx;
			img_part->co[k] = gy;
		}

		/*
		k = 0;
	for (y = 0; y < real_part->vsize; y++)
		for (x = 0; x < real_part->hsize; x++, k++)
		{
			real_part->co[k] -= (maxx + minx);
			img_part->co[k] -= (maxy + miny);
		}
		*/

		/*
	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\gk_r.dat", "wt");

	k = 0;
	for (y = 0; y < real_part->vsize; y++)
	{
		for (x = 0; x < real_part->hsize; x++, k++)
		{
			fprintf(fp, "%f ", real_part->co[k]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	FILE *fp2;
	fp2 = fopen("c:\\matlabr11\\work\\gk_i.dat", "wt");

	k = 0;
	for (y = 0; y<real_part->vsize; y++)
	{
		for (x = 0; x<real_part->hsize; x++, k++)
		{
			fprintf(fp2, "%f ", img_part->co[k]);
		}
		fprintf(fp2, "\n");
	}

	fclose(fp2);
	*/

	real_part->sum = h_sum;
	img_part->sum = v_sum;
}

void GaborFilter::making_gabor_filter4(float u0, float br, float bth, float th, Filter_mask2 *real_part, Filter_mask2 *img_part)
{
	int k, mx, my;
	int x, y;
	int dx, dy;
	float rx, ry, gx, gy;
	float minx, miny, maxx, maxy;
	
	float h_sum, v_sum;
	float gauss;
	float costh = cos(th), sinth = sin(th);
	float a = pow(2, br);
	float sigma_x = ((sqrt(2)*u0)/(2*M_PI))*((a + 1)/(a - 1));
	float sigma_y = (sqrt(2)*u0)/(2*M_PI*tan(bth/2));

	//mx = my = (int)5;
	mx = my = (int)u0;

	real_part->hsize = 2*mx+1;
	real_part->vsize = 2*my+1;
	img_part->hsize = real_part->hsize;
	img_part->vsize = real_part->vsize;

	real_part->co = new float[real_part->vsize*real_part->hsize];
	img_part->co = new float[img_part->vsize*img_part->hsize];

	h_sum = 0; 
	v_sum = 0; 

	minx = miny = 1e5;
	maxx = maxy = -1e5;

	k=0;
	for(y= 0; y < real_part->vsize; y++)
		for(x= 0; x < real_part->hsize; x++, k++)
		{
			dx = x - mx;
			dy = y - my;

			rx = ((float)dx*costh) + ((float)dy*sinth);
			ry = -((float)dx*sinth) + ((float)dy*costh);  

			gauss = exp(-0.5*(((rx*rx)/(sigma_x*sigma_x)) + ((ry*ry)/(sigma_y*sigma_y))));

			gx = gauss*(cos((2*M_PI*rx)/u0)-exp(-0.5*(sigma_x*sigma_y)));
			h_sum += fabs(gx);
			gy = gauss*sin((2*M_PI*rx)/u0);
			v_sum += fabs(gy);

			/*
			if (gx < minx)	minx = gx;
			if (gx > maxx)	maxx = gx;
			if (gy < miny)	miny = gy;
			if (gy > maxy)	maxy = gy;
			*/

			real_part->co[k] = gx;
			img_part->co[k] = gy;
		}

		/*
		k = 0;
	for (y = 0; y < real_part->vsize; y++)
		for (x = 0; x < real_part->hsize; x++, k++)
		{
			real_part->co[k] -= (maxx + minx);
			img_part->co[k] -= (maxy + miny);
		}
		*/

		/*
	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\gk_r.dat", "wt");

	k = 0;
	for (y = 0; y < real_part->vsize; y++)
	{
		for (x = 0; x < real_part->hsize; x++, k++)
		{
			fprintf(fp, "%f ", real_part->co[k]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	FILE *fp2;
	fp2 = fopen("c:\\matlabr11\\work\\gk_i.dat", "wt");

	k = 0;
	for (y = 0; y<real_part->vsize; y++)
	{
		for (x = 0; x<real_part->hsize; x++, k++)
		{
			fprintf(fp2, "%f ", img_part->co[k]);
		}
		fprintf(fp2, "\n");
	}

	fclose(fp2);
	*/

	real_part->sum = h_sum;
	img_part->sum = v_sum;
}

// 20001213
// DJ
void GaborFilter::making_gabor_filter2(double omega,double theta,double sigma_x,double sigma_y,Filter_mask *real_part,Filter_mask *img_part)
{
	int k,h_half,v_half;
	unsigned int h,v;
	double h_sum, v_sum;
	double h_, v_,h_1,v_1, ch, cv;
	double gauss;
	double costh = cos(theta), sinth = sin(theta);

	if (sigma_x < 0.1 && sigma_y < 0.1)	exit(1);

	h_half = horizontal_filter_size(theta,sigma_x, sigma_y);
	v_half = vertical_filter_size(theta,sigma_x, sigma_y);

	real_part->hsize = 2*h_half+1;
	real_part->vsize = 2*v_half+1;
	img_part->hsize = real_part->hsize;
	img_part->vsize = real_part->vsize;

	real_part->co = new double[real_part->vsize*real_part->hsize];
	img_part->co = new double[img_part->vsize*img_part->hsize];

	h_sum = 0; 
	v_sum = 0; 

	double omg2 = omega*omega;

	k=0;
	for(v= 0; v < real_part->vsize; v++)
		for(h= 0; h < real_part->hsize; h++,k++)
		{
			h_1 = h-h_half;
			v_1 = v-v_half;

			h_ = ((double)h_1)*costh + ((double)v_1) *sinth;
			v_ = -((double)h_1)*sinth + ((double)v_1) *costh;  
			/**  h_1 = ((double)h)*cos(theta) + ((double)v) *(-sin(theta));
			v_1 = ((double)h)*sin(theta) + ((double)v) *cos(theta); **/

			gauss = gaussian(h_,v_, sigma_x, sigma_y, omg2);
			//ch = gauss  * cos(2.0*M_PI*omega*h_);
			//ch = gauss*(cos(omega*(h_+v_))-exp(-sigma_x*sigma_y/2.0));
			ch = gauss*(cos(omega*(h_))-exp(-sigma_x*sigma_y/2.0));
			h_sum += fabs(ch);
			//cv = gauss  * sin(2.0*M_PI*omega*h_);
			//cv = gauss*sin(omega*(h_+v_));
			cv = gauss*sin(omega*(h_));
			v_sum += fabs(cv);

			real_part->co[k] = ch;
			img_part->co[k] = cv; 
		}


	real_part->sum = h_sum;
	img_part->sum = v_sum;
}

// Here, omega means wavenumber^2
double GaborFilter::gaussian(double x, double y, double sigma_x, double sigma_y, double omega)
{
  double r;

  r = -((x/sigma_x)*(x/sigma_x) + (y/sigma_y)*(y/sigma_y))/2.0;
  r = omega*exp(omega*r);
  r /= (sigma_x*sigma_y);
  //r /= 2.0*M_PI*sigma_x*sigma_y;
  return (r);

}

int GaborFilter::horizontal_filter_size(double theta, double sigma_x, double sigma_y)
{
  double h0,h1;
  h0 = FILTER_SIZE_RATIO_SIGMA*cos(theta)*sigma_x;
  h1 = FILTER_SIZE_RATIO_SIGMA*sin(theta)*sigma_y;

  if(h0 > h1)
    return (int)( ceil(h0) );
  else
    return (int)( ceil(h1) );
}

int GaborFilter::vertical_filter_size(double theta,double sigma_x, double sigma_y)
{
  double h0,h1;
  h0 = FILTER_SIZE_RATIO_SIGMA*sin(theta)*sigma_x;
  h1 = FILTER_SIZE_RATIO_SIGMA*cos(theta)*sigma_y;
  if(h0 > h1)
    return (int)( ceil(h0) );
  else
    return (int)( ceil(h1) );
}

/*
// Derivative of Gaussian function
// upto thire derivative
double GaborFilter::gauss_der(double x, double x0, double sigma, int der)
{
	double value;
	double pow_x, pow_sigma;

	pow_x = pow(x-x0,2);
	pow_sigma = pow(sigma,2);

	value = exp(-pow_x/(2*pow_sigma));

	switch(der) {
	case 1:
		value *= -(x-x0)/pow_sigma;
		break;
	case 2:
		value *= (pow_x-pow_sigma)/pow(pow_sigma,2);
		break;
	case 3:
		value *= ((x-x0)*(3*pow_sigma-pow_x))/pow(pow_sigma,3);
		break;
	default:
		break;
	}

	return value;
}	

// filter generation function
// output filter type: int *
// normalized output
void GaborFilter::moms_gauss(int width, int height, int *outImg)
{
	int i,j;
	int size, der;
	double sigma, theta;
	double *tmpImg;
	double sin_th, cos_th;
	double tmpPixel;

	tmpImg = new double[width*height];

	size = (int)floor(m_nSize/2);
	sigma = (double)m_nSize/8;
	der = m_nDer;
	theta = m_dTheta*T2R;
	sin_th = sin(theta);
	cos_th = cos(theta);

	// generate filter
	for(i=0;i<m_nSize;i++)
		for(j=0;j<m_nSize;j++) {
			tmpPixel = gauss_der((i-size)*sin_th+(j-size)*cos_th+size,(double)size,sigma,0)*gauss_der((i-size)*cos_th-(j-size)*sin_th+size,(double)size,sigma,der);
			PutPixel(i, j, tmpImg, width, height, tmpPixel);
		}

	NormalizeImg(width,height,tmpImg,outImg);

	delete tmpImg;
}*/

double *GaborFilter::gaborCo(GrayImage A, double wave, double theta)
{
	Filter_mask real_part, img_part;    

	int i;

	// get gabor filter coeff.
	//making_gabor_filter(M_PI/wave,M_PI*((theta+45.0)/180.0), M_PI, M_PI, &real_part, &img_part);
	//making_gabor_filter(M_PI/wave,M_PI*((theta)/180.0), M_PI, M_PI, &real_part, &img_part);
	// 20010611@16:49@DJ
	making_gabor_filter3(wave, 1.5, M_PI*((45.0)/180.0), M_PI*((theta)/180.0), &real_part, &img_part);
	//making_gabor_filter2(M_PI/wave,M_PI*((theta)/180.0), M_PI, M_PI, &real_part, &img_part);

	double *bank1 = A.convolutionf0(real_part.hsize,real_part.vsize, real_part.co);
	double *bank2 = A.convolutionf0(img_part.hsize,img_part.vsize, img_part.co);

	double *gabor_co = new double[A.w()*A.h()];

	for(i=0;i<A.w()*A.h();i++)
		  gabor_co[i] = sqrt((bank1[i])*(bank1[i])+(bank2[i])*(bank2[i]));

	delete [] bank2;
	delete [] bank1;
	delete [] real_part.co;
	delete [] img_part.co;

	return gabor_co;
}

double *GaborFilter::projCo(double *orgCo, int w, int h, int dir)
{
	int i,j;
	double *proj;

	switch(dir)
	{
	case 0:
		proj = new double[w];
		for(i=0;i<w;i++) proj[i] = 0.0;
		
		for(j=0;j<h;j++)
			for(i=0;i<w;i++)
				proj[i] += orgCo[j*w+i];
		break;
	case 1:
		proj = new double[h];
		for(i=0;i<h;i++) proj[i] = 0.0;
		
		for(j=0;j<h;j++)
			for(i=0;i<w;i++)
				proj[j] += orgCo[j*w+i];
		break;
	}

	return proj;
}