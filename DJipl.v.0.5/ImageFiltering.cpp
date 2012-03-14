// ImageFiltering.cpp: implementation of the CImageFiltering class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"
#include "ImageFiltering.h"
#include "ImagePixel.h"
#include "ImgMisc.h"
#include "ProgressBar.h"
#include "Algebra.h"
#include "math.h"
#include "CImage.h"
#include "morph.h"
#include "SVV.h"
#include "GaborFilter.h"

#define XFRAG	1
#define YFRAG	1

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageFiltering::CImageFiltering()
{
	m_pImage = NULL;
}

CImageFiltering::CImageFiltering(CmyImage &Image)
{
	m_pImage = &Image;
	m_nWidth = m_pImage->GetWidth();
	m_nHeight = m_pImage->GetHeight();
	m_nDepth = m_pImage->GetBitCount();
}

CImageFiltering::~CImageFiltering()
{

}

void CImageFiltering::Blur()
{
	int pnFilter[9] = {1, 3, 1, 3, 4, 3, 1, 3, 1};
	Filtering(pnFilter, _T("부드럽게 하기"));
}

void CImageFiltering::Sharpening()
{
	int pnFilter[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
	Filtering(pnFilter, _T("날카롭게 하기"));
}

void CImageFiltering::Laplacian()
{
	int pnFilter[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
	Filtering(pnFilter, _T("경계선 추출"));
}

void CImageFiltering::Embossing()
{
	int pnFilter[9] = {1, 1, 0, 1, 0, -1, 0, -1, -1};
	Filtering(pnFilter, _T("엠보싱"));
}

void CImageFiltering::HEdge()
{
	int pnFilter[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	Filtering(pnFilter, _T("수평에지"));
}

void CImageFiltering::VEdge()
{
	int pnFilter[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
	Filtering(pnFilter, _T("수직에지"));
}

void CImageFiltering::Filtering(int * pnFilter, CString strProgressBar)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8)
	{
		int i, div=0;

		CmyImage img = CImgMisc::MakeImg(m_nWidth,m_nHeight,m_nDepth);
		
		GrayImage org_img = CImgMisc::ImgConvc_G(*m_pImage);
		
		for(i=0 ; i<8 ; i++) div += pnFilter[i];
		
		// div: total sum of filter coeff.		offset: ?
		GrayImage *conv_img = org_img.convolution(3,3,pnFilter,div);
		
		CImgMisc::ShowImg(CImgMisc::ImgConvG_c(conv_img));
	}
	else FilteringColor(pnFilter, strProgressBar);
}

void CImageFiltering::FilteringColor(int * pnFilter, CString strProgressBar)
{
	CProgressBar bar(strProgressBar, 100, m_nHeight);
	int i, j, tot=0;
	CColorPixelPtr ptrSorce(m_pImage->GetUndoHandle());
	CColorPixelPtr ptrDest(*m_pImage);
	CColorPixel s[9];
	CColorPixel p;

	for(i=0 ; i<8 ; i++)
		tot += pnFilter[i];

	for(j=1 ; j<m_nHeight-1 ; j++)
	{
		for(i=1 ; i<m_nWidth-1 ; i++)
		{
			s[0] = ptrSorce[j-1][i-1];
			s[1] = ptrSorce[j-1][ i ];
			s[2] = ptrSorce[j-1][i+1];
			s[3] = ptrSorce[ j ][i-1];
			s[4] = ptrSorce[ j ][ i ];
			s[5] = ptrSorce[ j ][i+1];
			s[6] = ptrSorce[j+1][i-1];
			s[7] = ptrSorce[j+1][ i ];
			s[8] = ptrSorce[j+1][i+1];

			p = s[0]*pnFilter[0]+
				s[1]*pnFilter[1]+
				s[2]*pnFilter[2]+
				s[3]*pnFilter[3]+
				s[4]*pnFilter[4]+
				s[5]*pnFilter[5]+
				s[6]*pnFilter[6]+
				s[7]*pnFilter[7]+
				s[8]*pnFilter[8];
			if(tot != 0 && tot != 1) p /= tot;
			p >> ptrDest[j][i];
		}
		bar.StepIt();
	}
}



void CImageFiltering::Morp(int method)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	// structuring element
	unsigned char buf[9] = {255,255,255,255,255,255,255,255,255};
	Morphology morph(3,3,buf);
	
	BinaryImage srcimg = CImgMisc::ImgConvc_B(*m_pImage);
	BinaryImage resimg(&srcimg);

	switch(method)
	{
		case 0:
			morph.complement();
			break;
		case 1:
			morph.reflection();
			break;
		case 2:
			morph.dilation(srcimg,resimg);
			break;
		case 3:
			morph.erosion(srcimg,resimg);
			break;
		case 4:
			morph.opening(srcimg,resimg);
			break;
		case 5:
			morph.closing(srcimg,resimg);
			break;
		case 6:
			morph.thickening(srcimg,resimg);
			break;
		case 7:
			morph.thinning(srcimg,resimg);
			break;
		case 8:
			morph.filling(120,160,srcimg,resimg);
			break;
	}

	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&resimg));
}

void CImageFiltering::EdgeDominance()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	int i, j, tot=0;
	CPixelPtr ptrSorce(m_pImage->GetUndoHandle());
	
	// new image for dominance map
	CmyImage image = CImgMisc::MakeImg(m_nWidth*2,m_nHeight,8);
	CPixelPtr pDest(image);

	CPixel pH, pV;

	int pnFilterH[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};

	int pnFilterV[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

	for(j=1 ; j<m_nHeight-1 ; j++)
	{
		for(i=1 ; i<m_nWidth-1 ; i++)
		{
			// normal filtering
			// H edge
			pH = ptrSorce[j-1][i-1]*pnFilterH[0] +
				ptrSorce[j-1][ i ]*pnFilterH[1] +
				ptrSorce[j-1][i+1]*pnFilterH[2] +
				ptrSorce[ j ][i-1]*pnFilterH[3] +
				ptrSorce[ j ][ i ]*pnFilterH[4] +
				ptrSorce[ j ][i+1]*pnFilterH[5] +
				ptrSorce[j+1][i-1]*pnFilterH[6] +
				ptrSorce[j+1][ i ]*pnFilterH[7] +
				ptrSorce[j+1][i+1]*pnFilterH[8];
			// V edge
			pV = ptrSorce[j-1][i-1]*pnFilterV[0] +
				ptrSorce[j-1][ i ]*pnFilterV[1] +
				ptrSorce[j-1][i+1]*pnFilterV[2] +
				ptrSorce[ j ][i-1]*pnFilterV[3] +
				ptrSorce[ j ][ i ]*pnFilterV[4] +
				ptrSorce[ j ][i+1]*pnFilterV[5] +
				ptrSorce[j+1][i-1]*pnFilterV[6] +
				ptrSorce[j+1][ i ]*pnFilterV[7] +
				ptrSorce[j+1][i+1]*pnFilterV[8];

			// H map
			if (pH >= pV+m_nMinTh)
				pDest[j][i] = 255;
			else
				pDest[j][i] = 0;
			// V map
			if (pV >= pH+m_nMinTh)
				pDest[j][i+m_nWidth] = 255;
			else
				pDest[j][i+m_nWidth] = 0;
		}
	}

	// image save
	image.Save("./EdgeDom.bmp");

	CImgMisc::ShowImg(image);
}

void CImageFiltering::SetVar(int method, int value, int min, int max)
{
	m_nMethod = method;
	m_nValue = value;
	m_nMinTh = min;
	m_nMaxTh = max;
}

void CImageFiltering::MyGabor(double wave, double theta)
{
	GaborFilter gabor;
	//double *gaborImg = gabor.gaborCo(CImgMisc::ImgConvc_G(*m_pImage),wave,theta);
	double *gaborImg = gabor.gaborCo(CImgMisc::ImgConvc_G(*m_pImage), wave, theta);

	CImgMisc::Normalize(gaborImg,m_nWidth,m_nHeight);
}

// 20010309@8:47a@DJ
void CImageFiltering::GaussianFilterCo(double u, double a, int kersize, double *kernel)
{
	int i, j, k = 0;
	int midsize = kersize/2;
	int x, y;

	for (j=0; j<kersize; j++)
		for (i=0; i<kersize; i++, k++)
		{
			x = i-midsize;
			y = j-midsize;

			kernel[k] = u*(a/M_PI)*sqrt(a/M_PI)*exp(-a*(x*x+y*y+u*u));
		}
}
