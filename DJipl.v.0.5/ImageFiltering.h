// ImageFiltering.h: interface for the CImageFiltering class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _IMAGEFILTERING_H_
#define _IMAGEFILTERING_H_

#include "ImagePixel.h"	// Added by ClassView

#include "CImage.h"

class CImageFiltering  
{
public:
	void GaussianFilterCo(double u, double a, int kersize, double *kernel);
	CImageFiltering();
	CImageFiltering(CmyImage &pImage);
	virtual ~CImageFiltering();

	/*******************************************************
					인터페이스 함수
	*******************************************************/
	void Blur();		// 부드럽게 하기
	void Sharpening();	// 날카롭게 하기
	void Laplacian();	// 경계선 추출 (라플라시안)
	void Embossing();	// 엠보싱
	void HEdge();		// 수평에지
	void VEdge();		// 수직에지
	void Morp(int method=1);		// morphological processing
	void EdgeDominance();	// Dominance Edge Map
	void SetVar(int method, int value, int min, int max); // 각종 변수 setting


	// Gabor 필터링
	void MyGabor(double wave, double theta);
	// 범용 필터링
	void Filtering(int *pnFilter, CString strProgressBar = _T("이미지 필터링"));

protected:
	/*******************************************************
							멤버 변수들
	*******************************************************/
	int m_nWidth, m_nHeight, m_nDepth;
	CmyImage *m_pImage;
	int m_nMethod, m_nValue, m_nMinTh, m_nMaxTh;

	/*******************************************************
					실제 이미지 필터링 함수
	*******************************************************/
	// 필터링
	void FilteringColor(int *pnFilter, CString strProgressBar);
};

#endif