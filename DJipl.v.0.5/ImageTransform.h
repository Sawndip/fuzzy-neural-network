// ImageTransform.h: interface for the CImageTransform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGETRANSFORM_H__98458A41_F915_11D3_BF48_00E0294BE14D__INCLUDED_)
#define AFX_IMAGETRANSFORM_H__98458A41_F915_11D3_BF48_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImagePixel.h"

class CImageTransform  
{
public:
	void MyRLPGray(int w,int h);
	void MyRLPColor(int w,int h);
	void MyRLP(int w, int h);
	CImageTransform(CmyImage &Image);
	CImageTransform();
	virtual ~CImageTransform();

	void MyLP(int horiz, int vert);
	void MyLFT();
	void MyColor(int flag, BOOL flagAll, BOOL flagR, BOOL flagG,BOOL flagB);

protected:
	/*******************************************************
							¸â¹ö º¯¼öµé
	*******************************************************/
	CmyImage *m_pImage;
	int m_nWidth, m_nHeight, m_nDepth;

	void MyLPGray(int horiz, int vert);
	void MyLPColor(int horiz, int vert);

	void MyLFTGray();
	void MyLFTColor();
};

#endif // !defined(AFX_IMAGETRANSFORM_H__98458A41_F915_11D3_BF48_00E0294BE14D__INCLUDED_)
