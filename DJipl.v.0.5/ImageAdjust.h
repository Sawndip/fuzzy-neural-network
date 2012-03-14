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
					�������̽� �Լ�
	*******************************************************/
	// ����
	void Invert();
	// ��� ����
	void Brightness(int nAdjustStep = 30);
	// ��Ʈ��Ʈ ����
	void Contrast(int nAdjustStep = 30);
	// ����ȭ
	void Threshold(int nRange = GT, int nValue = 1, int nMinTh = 0, int nMaxTh = 255);

protected:
	/*******************************************************
							��� ������
	*******************************************************/
	int m_nWidth, m_nHeight, m_nDepth;
	CmyImage *m_pImage;

	/*******************************************************
					���� �̹��� ���μ��� �Լ�
	*******************************************************/
	// ����
	void InvertColor();
	void InvertGray();

	// ��� ����
	void BrightnessGray(int nAdjustStep);
	void BrightnessColor(int nAdjustStep);

	// ��Ʈ��Ʈ ����
	void ContrastGray(int nAdjustStep);
	void ContrastColor(int nAdjustStep);
};

#endif // !defined(AFX_ADJUSTIMAGE_H__18ACFE74_E632_11D1_8F00_006097283F10__INCLUDED_)
