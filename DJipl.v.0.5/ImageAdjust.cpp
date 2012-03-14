// ImageAdjust.cpp: implementation of the CImageAdjust class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"
#include "ImageAdjust.h"
#include "ImagePixel.h"
#include "ImgMisc.h"
#include "ProgressBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageAdjust::CImageAdjust()
{
	m_pImage = NULL;
}

CImageAdjust::~CImageAdjust()
{

}

CImageAdjust::CImageAdjust(CmyImage &Image)
{
	m_pImage = &Image;
	m_nWidth = m_pImage->GetWidth();
	m_nHeight = m_pImage->GetHeight();
	m_nDepth = m_pImage->GetBitCount();
}

void CImageAdjust::Invert()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);

	if(m_nDepth == 8) InvertGray();
	else InvertColor();
}

void CImageAdjust::InvertGray()
{
	CProgressBar bar(_T("반전"), 100, m_nHeight);
	CPixelPtr ptr(*m_pImage);

	int i, j;

	for(j=0 ; j<m_nHeight ; j++) 
	{
		for(i=0 ; i<m_nWidth ; i++)
		{
			ptr[j][i] =~ptr[j][i];
		}
		bar.StepIt();
	}
}

void CImageAdjust::InvertColor()
{
	CProgressBar bar(_T("반전"), 100, m_nHeight);
	CColorPixelPtr ptr(*m_pImage);

	int i, j;

	for(j=0 ; j<m_nHeight ; j++) 
	{
		for(i=0 ; i<m_nWidth ; i++)
		{
			ptr[j][i].R =~ptr[j][i].R;
			ptr[j][i].G =~ptr[j][i].G;
			ptr[j][i].B =~ptr[j][i].B;
		}
		bar.StepIt();
	}
}

void CImageAdjust::Brightness(int nAdjustStep)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);

	if(m_nDepth == 8) BrightnessGray(nAdjustStep);
	else BrightnessColor(nAdjustStep);
}

void CImageAdjust::BrightnessGray(int nAdjustStep)
{
	CProgressBar bar(_T("밝기 조절"), 100, m_nHeight);
	CPixelPtr ptr(*m_pImage);
	CPixel p;

	for(int j=0 ; j<m_nHeight ; j++) 
	{
		for(int i=0 ; i<m_nWidth ; i++)
		{
			p << ptr[j][i];
			p += nAdjustStep;
			p >> ptr[j][i];
		}
		bar.StepIt();
	}
}

void CImageAdjust::BrightnessColor(int nAdjustStep)
{
	CProgressBar bar(_T("밝기 조절"), 100, m_nHeight);
	CColorPixelPtr ptr(*m_pImage);
	CColorPixel p;

	for(int j=0 ; j<m_nHeight ; j++) 
	{
		for(int i=0 ; i<m_nWidth ; i++)
		{
			p << ptr[j][i];
			p += nAdjustStep;
			p >> ptr[j][i];
		}
		bar.StepIt();
	}
}

void CImageAdjust::Contrast(int nAdjustStep)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);

	if(m_nDepth == 8) ContrastGray(nAdjustStep);
	else ContrastColor(nAdjustStep);
}

void CImageAdjust::ContrastGray(int nAdjustStep)
{
	CProgressBar bar(_T("콘트라스트 조절"), 100, m_nHeight);
	CPixelPtr ptr(*m_pImage);
	CPixel p;

	for(int j=0 ; j<m_nHeight ; j++) 
	{
		for(int i=0 ; i<m_nWidth ; i++)
		{
			p << ptr[j][i];
			p += (p-128)*nAdjustStep/128;
			p >> ptr[j][i];
		}
		bar.StepIt();
	}
}

void CImageAdjust::ContrastColor(int nAdjustStep)
{
	CProgressBar bar(_T("콘트라스트 조절"), 100, m_nHeight);
	CColorPixelPtr ptr(*m_pImage);
	CColorPixel p;

	for(int j=0 ; j<m_nHeight ; j++) 
	{
		for(int i=0 ; i<m_nWidth ; i++)
		{
			p << ptr[j][i];
			p += (p-128)*nAdjustStep/128;
			p >> ptr[j][i];
		}
		bar.StepIt();
	}
}

// threshold
// nRange: IN, OUT, GT, LT
// nValue: 1, 255
void CImageAdjust::Threshold(int nRange, int nValue, int nMinTh, int nMaxTh)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);

	if(m_nDepth == 8)
	{
		GrayImage A = CImgMisc::ImgConvc_G(*m_pImage);

		switch(nRange)
		{
			case INS:
				A.thresholdSelf(nMinTh, nMaxTh, GrayImage::InBound);
				break;
			case OUTS:
				A.thresholdSelf(nMinTh, nMaxTh, GrayImage::OutOfBound);
				break;
			case GT:
				A.thresholdSelf(nMinTh, 255, GrayImage::InBound);
				break;
			case LT:
				A.thresholdSelf(0, nMinTh, GrayImage::InBound);
				break;
		}

		*m_pImage = CImgMisc::ImgConvG_c(&A);
	}
}