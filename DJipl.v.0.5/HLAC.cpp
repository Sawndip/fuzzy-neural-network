// HLAC.cpp: implementation of the CHLAC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "HLAC.h"
#include "Algebra.h"
//#include "cimage.h"
#include "SVV.h"
//#include "VisMatrix.h"
#include "cv.h"

#define HLAC_LEARN 1
#define NO_SAMPLE	105
#define NO_FACE		6

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHLAC::CHLAC()
{

}

CHLAC::~CHLAC()
{

}

void CHLAC::HLACget()
{
	int pnFilter[9*35] = {
		// no. 1
		0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 2, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 0, 0,
		// no. 8
		0, 0, 0, 0, 2, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 2, 0, 0, 0,
		0, 0, 1, 0, 2, 0, 0, 0, 0,
		0, 0, 2, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 2, 0, 0, 0, 0,
		0, 2, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 0, 2, 0, 0, 0, 0,
		// no. 15
		2, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 1, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		1, 0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 1, 1, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 1, 0, 0,
		// no. 22
		1, 0, 0, 0, 1, 0, 0, 1, 0,
		0, 0, 0, 1, 1, 0, 0, 0, 1,
		0, 0, 0, 0, 1, 1, 1, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 1, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 1, 0, 1, 1, 0, 0, 0, 0,
		// no. 29
		1, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 1, 1, 0, 0, 1, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 1,
		0, 0, 0, 0, 1, 1, 0, 1, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 1,
		0, 1, 0, 0, 1, 1, 0, 0, 0,
		1, 0, 1, 0, 1, 0, 0, 0, 0
	};
	
	if (m_Color)
		HLACstartColor(pnFilter);
	else
		HLACstartGray(pnFilter);
}

void CHLAC::HLAC(int u,int v,int xf,int yf,int fl)
{
	m_nLPu = u;
	m_nLPv = v;
	m_nXf = xf;
	m_nYf = yf;
	m_nFlag = fl;

	int pnFilter[9*35] = {
		// no. 1
		0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 2, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 0, 0,
		// no. 8
		0, 0, 0, 0, 2, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 2, 0, 0, 0,
		0, 0, 1, 0, 2, 0, 0, 0, 0,
		0, 0, 2, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 2, 0, 0, 0, 0,
		0, 2, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 0, 2, 0, 0, 0, 0,
		// no. 15
		2, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 1, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		1, 0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 1, 1, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 1, 0, 0,
		// no. 22
		1, 0, 0, 0, 1, 0, 0, 1, 0,
		0, 0, 0, 1, 1, 0, 0, 0, 1,
		0, 0, 0, 0, 1, 1, 1, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 1, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 1, 0, 1, 1, 0, 0, 0, 0,
		// no. 29
		1, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 1, 1, 0, 0, 1, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 1,
		0, 0, 0, 0, 1, 1, 0, 1, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 1,
		0, 1, 0, 0, 1, 1, 0, 0, 0,
		1, 0, 1, 0, 1, 0, 0, 0, 0
	};
	
	if (m_Color)
		HLACstartColor(pnFilter);
	else
		HLACstartGray(pnFilter);
}

void CHLAC::HLACstartColor(int * pnFilter)
{
	int i,j,k,l,f_no=35,ww=m_nWidth/m_nXf,hh=m_nHeight/m_nYf,z,y;
//	CColorPixelPtr ptrSorce(*m_pImage);
	//CVisDVector HLACvec;
	float* HLACvec = new float[35];

	float *HLACvec2;
	float *testVec;
	double *testVal;
	float *HLACmat;
	double max,min;
	double dist_face,dist_noface;
	double *mf_face,*mf_noface;
	float faceCVec[2], nofaceCVec[2];
	int max_pos = 1;
	double sigma = 69025;

	FILE *fp;

	HLACvec2 = new float[35];

	if (m_nFlag == 0)
		fp = fopen("C:\\MATLABR11\\work\\HLAC.dat","a");
	else
	{
		fp = fopen("C:\\MATLABR11\\work\\HLACsrc4.dat","r+");
		HLACmat = new float[2*35];
		testVec = new float[2];
		testVal = new double[m_nXf*m_nYf];
		mf_face = new double[m_nXf*m_nYf];
		mf_noface = new double[m_nXf*m_nYf];

		for(i=0;i<2;i++) fscanf(fp,"%f",&faceCVec[i]);
		for(i=0;i<2;i++) fscanf(fp,"%f",&nofaceCVec[i]);
		//for(i=0;i<2;i++) fscanf(fp,"%f",&faceVec[i]);

		z=0;
		for(j=0;j<35;j++)
			for(i=0;i<2;i++,z++)
				fscanf(fp,"%f",&HLACmat[z]);

		max = -1e8;
		min = 1e8;
	}

	// IRG transform
	//ColorImage A = CImgMisc::ImgConvc_C(*m_pImage);
	ColorImage A = m_pImage_c;
	GrayImage R1,G1,B1;
	A.to3Gray(&R1, &G1, &B1);	

	GrayImage I2(R1);
	
	for(i=0;i<I2.w();i++)
		for(j=0;j<I2.h();j++)
			I2.setPixel(i,j,R1.getPixel(i,j)-B1.getPixel(i,j));				// R-B

	y=0;
	for(j=0;j<m_nYf;j++)
		for(i=0;i<m_nXf;i++,y++)
		{
			//HLACvec = HLACparco(pnFilter,ww,hh,I2,i,j);
			HLACparco(pnFilter,ww,hh,I2,i,j, HLACvec);
			for(k=0;k<35;k++) HLACvec2[k] = (float)HLACvec[k];

			/*
			HLACvec = HLACparco(pnFilter,ww,hh,R,i,j);
			for(k=35;k<70;k++) HLACvec2[k] = HLACvec[k-34];
			delete []HLACvec;

			HLACvec = HLACparco(pnFilter,ww,hh,G,i,j);
			for(k=70;k<105;k++) HLACvec2[k] = HLACvec[k-69];
			delete []HLACvec;
			*/

			if (m_nFlag == 0)
			{
				for(k=0;k<105;k++) fprintf(fp,"%.0f ",(float)HLACvec2[k]);
				fprintf(fp,"\n");
			}
			else
			{
				// get y = A'x
				for(l=0;l<2;l++)
				{
					testVec[l] = 0;
					for(k=0;k<35;k++)
					{
						testVec[l] += (float)(HLACmat[k*2+l]*HLACvec2[k]);
					}
				}

				CString str;
				str.Format("%f, %f", testVec[0],testVec[1]);
				AfxMessageBox(str);

				// get score from norm! the smaller, the better!
				//testVal[y] = sqrt((faceVec[0]-testVec[0])*(faceVec[0]-testVec[0])+(faceVec[1]-testVec[1])*(faceVec[1]-testVec[1]));
				dist_face = sqrt((faceCVec[0]-testVec[0])*(faceCVec[0]-testVec[0])+(faceCVec[1]-testVec[1])*(faceCVec[1]-testVec[1]));
				//dist_noface = sqrt((nofaceCVec[0]-faceCVec[0])*(nofaceCVec[0]-faceCVec[0])+(nofaceCVec[1]-faceCVec[1])*(nofaceCVec[1]-faceCVec[1]));
				dist_noface = sqrt((nofaceCVec[0]-testVec[0])*(nofaceCVec[0]-testVec[0])+(nofaceCVec[1]-testVec[1])*(nofaceCVec[1]-testVec[1]));

				//mf_face[y] = 1.0 - dist_face/(dist_face+dist_noface);
/*				if (dist_face > dist_noface)
					mf_face[y] = 0.0;
				else
					mf_face[y] = 1.0 - dist_face/dist_noface;
				//mf_noface[y] = 1.0 - dist_noface/(dist_face+dist_noface);
				mf_noface[y] = 1.0 - mf_face[y];
*/

				mf_face[y] = exp(-dist_face/sigma);
				//mf_noface[y] = exp(-dist_noface/sigma);
				mf_noface[y] = 1 - mf_face[y];

				/*
				if (testVal[y] >= max) max = testVal[y];
				if (testVal[y] <= min)
				{
					min = testVal[y];
					max_pos = y+1;
				}
				*/
			}
		}

	if (m_nFlag == 1)
	{
		y=0;
		for(j=0;j<m_nYf;j++)
			for(i=0;i<m_nXf;i++,y++)
			{
/*				testVal[y] = (testVal[y]-min)/(max-min)*255.0;
				
				if (y+1 == max_pos)
				{
					for(l=-10;l<10;l++)
						for(k=-10;k<10;k++)
							ptrSorce[j*hh+hh/2+l][i*ww+ww/2+k].B = 255;
				}
				else
				{
					for(l=-5;l<5;l++)
						for(k=-5;k<5;k++)
								ptrSorce[j*hh+hh/2+l][i*ww+ww/2+k].B = 255 - (int)testVal[y];
*/
					for(l=0;l<3;l++)
						for(k=0;k<ww;k++)
						{
							m_pImage_c.setPixel(i*ww+k, (j+1)*hh-l-1, 0, 0, 0);

							if (k < (int)(ww*mf_noface[y]))
								m_pImage_c.setPixel(i*ww+k, (j+1)*hh-l-4, 255, 0, 0);
							if (k < (int)(ww*mf_face[y]))
								m_pImage_c.setPixel(i*ww+k, (j+1)*hh-l-4, 0, 0, 255);
						}
//				}
			}
	}

	for(j=1;j<m_nYf;j++)
		for(i=0;i<m_nWidth;i++)
			{
				m_pImage_c.setPixel(i, j*hh, 255, 0, 0);
			}

	for(j=0;j<m_nHeight;j++)
		for(i=1;i<m_nXf;i++)
			{
				m_pImage_c.setPixel(i*ww, j, 255, 0, 0);
			}

	//image.Save("./HLAC.bmp");
	//CImgMisc::ShowImg(image);

	/*
	float **RMat = CAlgebra::mat_new_forRecipe(f_no,f_no);
	float **v = CAlgebra::mat_new_forRecipe(f_no,f_no);
	float *w=CAlgebra::vec_new_forRecipe(f_no);
	int nrot;

	RMat = CAlgebra::vect_cross2(HLACvec,HLACvec,f_no);

	CAlgebra::jacobi(RMat,f_no,w,v,&nrot);

	delete []v;
	delete []w;
	delete []RMat;
*/

	if (m_nFlag == 1)
	{
		delete []testVal;
		delete []mf_face;
		delete []mf_noface;
		delete []testVec;
		delete []HLACmat;
	}

	delete []HLACvec;
	delete []HLACvec2;
	fclose(fp);
}

void CHLAC::AutoHLAC(int u,int v,int xf,int yf,int nf,int nnf)
{
	m_nLPu = u;
	m_nLPv = v;
	m_nXf = xf;
	m_nYf = yf;

	int pnFilter[9*35] = {
		// no. 1
		0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 2, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 0, 0,
		// no. 8
		0, 0, 0, 0, 2, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 2, 0, 0, 0,
		0, 0, 1, 0, 2, 0, 0, 0, 0,
		0, 0, 2, 0, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 2, 0, 0, 0, 0,
		0, 2, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 0, 2, 0, 0, 0, 0,
		// no. 15
		2, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 1, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0,
		1, 0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 1, 1, 1, 0, 0, 0, 0,
		0, 1, 0, 0, 1, 0, 1, 0, 0,
		// no. 22
		1, 0, 0, 0, 1, 0, 0, 1, 0,
		0, 0, 0, 1, 1, 0, 0, 0, 1,
		0, 0, 0, 0, 1, 1, 1, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 1, 0,
		0, 1, 0, 0, 1, 0, 0, 0, 1,
		1, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 1, 0, 1, 1, 0, 0, 0, 0,
		// no. 29
		1, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 1, 1, 0, 0, 1, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 1,
		0, 0, 0, 0, 1, 1, 0, 1, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 1,
		0, 1, 0, 0, 1, 1, 0, 0, 0,
		1, 0, 1, 0, 1, 0, 0, 0, 0
	};

	int i;
	//FILE *fp;
	char str[30];
//	CImage image;
	ColorImage image;

	// face files
	for (i=0;i<nf;i++)
	{
		sprintf(str,".\\testing\\facea%03d.BMP",i*2+50);
		image.load(str);
		SubAutoHLAC(pnFilter, &image);
	}

	// non-face files
	for (i=0;i<nnf;i++)
	{
		sprintf(str,".\\testing\\nofacea%03d.BMP",i*2+50);
		image.load(str);
		SubAutoHLAC(pnFilter, &image);
	}
}

void CHLAC::SubAutoHLAC(int *pnFilter, ColorImage *Image)
{
	int i,j,k,f_no=35,ww=Image->w()/m_nXf,hh=Image->h()/m_nYf,y;
	
	float* HLACvec = new float[f_no];
	float* HLACvec2 = new float[f_no];

	FILE *fp;

	fp = fopen("C:\\MATLABR11\\work\\HLAC.dat","a");

	// I1I2I3 transform
	ColorImage A = *Image;
	GrayImage R1,G1,B1;
	A.to3Gray(&R1, &G1, &B1);	

	GrayImage I2(R1);
	
	for(i=0;i<I2.w();i++)
		for(j=0;j<I2.h();j++)
			I2.setPixel(i,j,R1.getPixel(i,j)-B1.getPixel(i,j));				// R-B
			//I2.setPixel(i,j,abs(R1.getPixel(i,j)-B1.getPixel(i,j)));		// R-B ABS

	y=0;
	for(j=0;j<m_nYf;j++)
		for(i=0;i<m_nXf;i++,y++)
		{
			//HLACvec = HLACparco(pnFilter,ww,hh,I2,i,j);
			HLACparco(pnFilter,ww,hh,I2,i,j, HLACvec);

			for(k=0; k<35; k++) HLACvec2[k] = HLACvec[k];

			/*
			HLACvec = HLACparco(pnFilter,ww,hh,R,i,j);
			for(k=35;k<70;k++) HLACvec2[k] = HLACvec[k-34];
			delete []HLACvec;

			HLACvec = HLACparco(pnFilter,ww,hh,G,i,j);
			for(k=70;k<105;k++) HLACvec2[k] = HLACvec[k-69];
			delete []HLACvec;
			*/

			//for(k=0;k<105;k++) fprintf(fp,"%.0f ",(float)HLACvec2[k]);
			for(k=0; k<35; k++) fprintf(fp,"%.3f ",(float)HLACvec2[k]);
			fprintf(fp,"\n");
		}

	fclose(fp);

	delete []HLACvec2;
	delete []HLACvec;
}

void CHLAC::HLACstartGray(int * pnFilter)
{
	int i,j,k,l,f_no=35,ww=m_nWidth/m_nXf,hh=m_nHeight/m_nYf,z,y;
	BYTE *Img, *rImg;
	//CVisDVector HLACvec(f_no);
	float* HLACvec = new float[f_no];

	CSVV LPImg;
	LPImg.LPtable(ww,hh,m_nLPu,m_nLPv);
	Img = new BYTE[hh*ww];

	FILE *fp;
	fp = fopen("C:\\MATLABR11\\work\\CT21\\HLAC.dat","awt");

	y=0;
	for(j=0;j<m_nYf;j++)
		for(i=0;i<m_nXf;i++)
		{
			z=0;
			for(l=0;l<hh;l++)
				for(k=0;k<ww;k++,z++)
					Img[z] = m_pImage_g.getPixel(i*ww+k, j*hh+l);

			rImg = LPImg.LPtr(Img,m_nLPu,m_nLPv);

			//HLACvec = HLACco(pnFilter,rImg,m_nLPu,m_nLPv);
			HLACco(pnFilter,rImg,m_nLPu,m_nLPv, HLACvec);

			for (k=0; k<35; k++)
				fprintf(fp,"%.3f ",HLACvec[k]/(ww*hh));
			fprintf(fp,"\n");

			delete []rImg;
		}

	delete []Img;

	/*
	for(j=1;j<2;j++)
		for(i=0;i<m_nWidth;i++)
		{
			m_pImage_g.setPixel(i, j*hh, 255);
		}

	for(j=0;j<m_nHeight;j++)
		for(i=1;i<4;i++)
		{
			m_pImage_g.setPixel(i*ww, j, 255);
		}
		*/

	fclose(fp);

	delete []HLACvec;
}

void CHLAC::HLACco(int *co, BYTE *srcImg, int w, int h, float* _co)
{
	int f_no = 35;

	int i,j,k,l,m,pn,z,pixel;
	double tp;
	unsigned char flag;

	//CVisDVector HLACvec(f_no);
	//for(i=0; i<f_no; i++) HLACvec[i] = 0.0;

	for (i=0; i<f_no; i++)	_co[i] = 0.0f;

	for(j=1 ; j<h-1 ; j++)
	{
		for(i=1 ; i<w-1 ; i++)
		{
			for(k=0; k<f_no; k++) 
			{
				tp = 1.0;
				z=0;
				flag = 0;

				for(l=-1;l<2;l++)
					for(m=-1;m<2;m++,z++)
					{
						pn = co[9*k+z];

						if ((i+m > 0) && (i+m < w-1) && (j+l > 0) && (j+l < h-1))
						{
							pixel = srcImg[(j+l)*w+(i+m)];
							
							if (pn > 0)
							{
								flag = 1;
								while (pn-- > 0)	tp *= ((pixel/255.0)+1.0);
							}
						}
					}

				if (flag == 1)
					//HLACvec[k] += tp;
					_co[k] += (float)tp;
			}
		}
	}

	//return HLACvec;
}

void CHLAC::HLACparco(int *pnFilter, int ww, int hh, GrayImage pix, int i, int j, float* _parco)
{
	int l,k,z;
	//CVisDVector HLACvec;
	BYTE *rImg;
	BYTE *Img;
	CSVV LPImg;
	LPImg.LPtable(ww,hh,m_nLPu,m_nLPv);

	Img = new BYTE[hh*ww];
	z=0;
	for(l=0;l<hh;l++)
		for(k=0;k<ww;k++,z++)
			Img[z] = pix.getPixel(i*ww+k,j*hh+l);

	rImg = LPImg.LPtr(Img,m_nLPu,m_nLPv);
	
	//HLACvec = HLACco(pnFilter,rImg,m_nLPu,m_nLPv);
	HLACco(pnFilter,rImg,m_nLPu,m_nLPv, _parco);

	delete []rImg;
	delete []Img;

	//return HLACvec;
}

/*
CVisDVector CHLAC::HLACco(int *co, BYTE *srcImg, int w, int h)
{
	int f_no = 35;

	int i,j,k,l,m,pn,z,pixel;
	double tp;
	unsigned char flag;

	CVisDVector HLACvec(f_no);

	for(i=0; i<f_no; i++) HLACvec[i] = 0.0;

	for(j=1 ; j<h-1 ; j++)
	{
		for(i=1 ; i<w-1 ; i++)
		{
			for(k=0; k<f_no; k++) 
			{
				tp = 1.0;
				z=0;
				flag = 0;

				for(l=-1;l<2;l++)
					for(m=-1;m<2;m++,z++)
					{
						pn = co[9*k+z];

						if ((i+m > 0) && (i+m < w-1) && (j+l > 0) && (j+l < h-1))
						{
							pixel = srcImg[(j+l)*w+(i+m)];
							
							if (pn > 0)
							{
								flag = 1;
								while (pn-- > 0)	tp *= ((pixel/255.0)+1.0);
							}
						}
					}

				if (flag == 1)
					HLACvec[k] += tp;
			}
		}
	}

	return HLACvec;
}

CVisDVector CHLAC::HLACparco(int *pnFilter, int ww, int hh, GrayImage pix, int i, int j)
{
	int l,k,z;
	CVisDVector HLACvec;
	BYTE *rImg;
	BYTE *Img;
	CSVV LPImg;
	LPImg.LPtable(ww,hh,m_nLPu,m_nLPv);

	Img = new BYTE[hh*ww];
	z=0;
	for(l=0;l<hh;l++)
		for(k=0;k<ww;k++,z++)
			Img[z] = pix.getPixel(i*ww+k,j*hh+l);

	rImg = LPImg.LPtr(Img,m_nLPu,m_nLPv);
	
	HLACvec = HLACco(pnFilter,rImg,m_nLPu,m_nLPv);

	delete []rImg;
	delete []Img;

	return HLACvec;
}
*/

void CHLAC::HLACLearn(float *coeff, int no)
{
	int nf = NO_FACE, nnf = NO_SAMPLE - NO_FACE;
	float wf = (float)(nf/NO_SAMPLE), wnf = 1.0f - wf;

	// get db, dw
	float **a, *d, **v;
	int nrot;

	int n = 3;
	a = CAlgebra::mat_new_forRecipe(n,n);
	v = CAlgebra::mat_new_forRecipe(n,n);
	d = CAlgebra::vec_new_forRecipe(n);

	CAlgebra::jacobi(a,n,d,v,&nrot);
	CAlgebra::eigsrt(d,v,n);

/*	CString str;
	str.Format("%6.4f,%6.4f,%6.4f : %6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f,%6.4f",d[1],d[2],d[3],v[1][1],v[2][1],v[3][1],v[1][2],v[2][2],v[3][2],v[1][3],v[2][3],v[3][3]);
	AfxMessageBox(str);*/

	delete []a;
	delete []v;
	delete []d;
}