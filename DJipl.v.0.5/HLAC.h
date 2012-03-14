// HLAC.h: interface for the CHLAC class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HLAC_H_
#define _HLAC_H_

#include "cimage.h"
//#include "VisMatrix.h"
#include "cv.h"

//20010804
// HLAC ���� class ����.
class CHLAC  
{
public:
	// func
	CHLAC();
	void init(GrayImage img) { m_pImage_g = img; m_Color = 0; m_nWidth = img.w(); m_nHeight = img.h();}
	void init(ColorImage img) { m_pImage_c = img; m_Color = 1; m_nWidth = img.w(); m_nHeight = img.h();}
	virtual ~CHLAC();

	// HLAC ���͸�
	//CVisDVector HLACparco(int *pnFilter, int ww, int hh, GrayImage pix, int i, int j);
	//CVisDVector HLACco(int *co, BYTE *srcImg, int w, int h);
	void HLACparco(int *pnFilter, int ww, int hh, GrayImage pix, int i, int j, float* _parco);
	void HLACco(int *co, BYTE *srcImg, int w, int h, float* _co);
	void HLACLearn(float *coeff, int no);

	void HLAC(int u,int v,int xf,int yf,int fl);		// HLAC ��� ���ϱ�
	void HLACget();		// HLAC ����� �̿��� ��ü/�� �ν�
	void AutoHLAC(int u,int v,int xf,int yf,int nf,int nnf);
	void SubAutoHLAC(int *pnFilter, ColorImage *Image);
	void HLACstartGray(int *pnFilter);	// HLAC ���͸�
	void HLACstartColor(int *pnFilter);	// HLAC ���͸�

	// var
	int m_nWidth, m_nHeight;
	int m_nLPu, m_nLPv, m_nXf, m_nYf, m_nFlag;
	GrayImage m_pImage_g;
	ColorImage m_pImage_c;
	char m_Color;
};

#endif
