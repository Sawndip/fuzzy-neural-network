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
					�������̽� �Լ�
	*******************************************************/
	void Blur();		// �ε巴�� �ϱ�
	void Sharpening();	// ��ī�Ӱ� �ϱ�
	void Laplacian();	// ��輱 ���� (���ö�þ�)
	void Embossing();	// ������
	void HEdge();		// ������
	void VEdge();		// ��������
	void Morp(int method=1);		// morphological processing
	void EdgeDominance();	// Dominance Edge Map
	void SetVar(int method, int value, int min, int max); // ���� ���� setting


	// Gabor ���͸�
	void MyGabor(double wave, double theta);
	// ���� ���͸�
	void Filtering(int *pnFilter, CString strProgressBar = _T("�̹��� ���͸�"));

protected:
	/*******************************************************
							��� ������
	*******************************************************/
	int m_nWidth, m_nHeight, m_nDepth;
	CmyImage *m_pImage;
	int m_nMethod, m_nValue, m_nMinTh, m_nMaxTh;

	/*******************************************************
					���� �̹��� ���͸� �Լ�
	*******************************************************/
	// ���͸�
	void FilteringColor(int *pnFilter, CString strProgressBar);
};

#endif