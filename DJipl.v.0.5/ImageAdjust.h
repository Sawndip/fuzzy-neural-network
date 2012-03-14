// ImageAdjust.h: interface for the CImageAdjust class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADJUSTIMAGE_H__18ACFE74_E632_11D1_8F00_006097283F10__INCLUDED_)
#define AFX_ADJUSTIMAGE_H__18ACFE74_E632_11D1_8F00_006097283F10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// threshold method
#define INS	0
#define	OUTS	1
#define	GT	2
#define	LT	3

class CmyImage;

class CImageAdjust  
{
public:
	CImageAdjust();
	CImageAdjust(CmyImage &Image);
	virtual ~CImageAdjust();

	/*******************************************************
					인터페이스 함수
	*******************************************************/
	// 반전
	void Invert();
	// 밝기 조절
	void Brightness(int nAdjustStep = 30);
	// 콘트라스트 조절
	void Contrast(int nAdjustStep = 30);
	// 이진화
	void Threshold(int nRange = GT, int nValue = 1, int nMinTh = 0, int nMaxTh = 255);

protected:
	/*******************************************************
							멤버 변수들
	*******************************************************/
	int m_nWidth, m_nHeight, m_nDepth;
	CmyImage *m_pImage;

	/*******************************************************
					실제 이미지 프로세싱 함수
	*******************************************************/
	// 반전
	void InvertColor();
	void InvertGray();

	// 밝기 조절
	void BrightnessGray(int nAdjustStep);
	void BrightnessColor(int nAdjustStep);

	// 콘트라스트 조절
	void ContrastGray(int nAdjustStep);
	void ContrastColor(int nAdjustStep);
};

#endif // !defined(AFX_ADJUSTIMAGE_H__18ACFE74_E632_11D1_8F00_006097283F10__INCLUDED_)
