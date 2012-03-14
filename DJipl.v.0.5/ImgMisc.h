// ImgMisc.h: interface for the CImgMisc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGMISC_H__189290A1_107A_11D4_B763_00E0294BE14D__INCLUDED_)
#define AFX_IMGMISC_H__189290A1_107A_11D4_B763_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// image class of default image
#include "ImagePixel.h"
// image class of Qpark
#include "CImage.h"

class CImgMisc  
{
public:
	static void drawRectgray(CPixelPtr ptr, int x1, int y1, int x2, int y2, unsigned char val = 255);
	static void drawRectcolor(CColorPixelPtr ptr, int x1, int y1, int x2, int y2, unsigned char val = 255);
	CImgMisc();
	virtual ~CImgMisc();

	// show & make image
	static void ShowImg(CmyImage image);
	static CmyImage MakeImg(int width, int height, int depth);
	static GrayImage ImgConvc_G(CmyImage Img);
	static BinaryImage ImgConvc_B(CmyImage Img);
	static ColorImage ImgConvc_C(CmyImage Img);
	static CmyImage ImgConvG_c(GrayImage *Img);
	static CmyImage ImgConvC_c(ColorImage *Img);
	static CmyImage ImgConvB_c(BinaryImage *Img);
	
	// data normalize.
	static void Normalize(double *Img, int w, int h);
	static void DjMessageBox(CString str, int PaneNum);
};

#endif // !defined(AFX_IMGMISC_H__189290A1_107A_11D4_B763_00E0294BE14D__INCLUDED_)
