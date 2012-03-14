// ImgMisc.cpp: implementation of the CImgMisc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "ImgMisc.h"
#include "cimage.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImgMisc::CImgMisc()
{

}

CImgMisc::~CImgMisc()
{

}

void CImgMisc::ShowImg(CmyImage image)
{
	// 클립보드에 데이터 복사
	::OpenClipboard(NULL);
	::SetClipboardData(CF_DIB, ::CopyHandle(image.GetHandle()));
	::CloseClipboard();

	// paste new image
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_EDIT_PASTE);
}

CmyImage CImgMisc::MakeImg(int width, int height, int depth)
{
	CmyImage image;
	image.Create(width,height,depth);
	image.InitDIB(FALSE);
	return image;
}

GrayImage CImgMisc::ImgConvc_G(CmyImage Img)
{
	int i,j;

	GrayImage org_img(Img.GetWidth(),Img.GetHeight());
	CPixelPtr ptrSorce(Img);

	for(j=0;j<org_img.h();j++)
		for(i=0;i<org_img.w();i++)
		{
			org_img.setPixel(i,j,(BYTE)ptrSorce[j][i]);
		}

	return org_img;
}

BinaryImage CImgMisc::ImgConvc_B(CmyImage Img)
{
	int i,j;

	BinaryImage org_img(Img.GetWidth(),Img.GetHeight());
	CPixelPtr ptrSorce(Img);

	for(j=0;j<org_img.h();j++)
		for(i=0;i<org_img.w();i++)
		{
			if (ptrSorce[j][i] == 255) org_img.set(i,j);
			else org_img.reset(i,j);
		}

	return org_img;
}

ColorImage CImgMisc::ImgConvc_C(CmyImage Img)
{
	int i,j;

	ColorImage org_img(Img.GetWidth(),Img.GetHeight(),3);
	CColorPixelPtr ptrSorce(Img);

	for(j=0;j<org_img.h();j++)
		for(i=0;i<org_img.w();i++)
		{
			org_img.setPixel(i,j,(BYTE)ptrSorce[j][i].R,(BYTE)ptrSorce[j][i].G,(BYTE)ptrSorce[j][i].B);
		}

	return org_img;
}

CmyImage CImgMisc::ImgConvG_c(GrayImage *Img)
{
	int i,j;
	CmyImage CImg = MakeImg(Img->w(),Img->h(),Img->d()*8);
	CPixelPtr ptrDest(CImg);

	for(j=0;j<Img->h();j++)
		for(i=0;i<Img->w();i++)
		{
			ptrDest[j][i] = *(Img->getp(i,j));
		}

	return CImg;
}

CmyImage CImgMisc::ImgConvB_c(BinaryImage *Img)
{
	int i,j;
	CmyImage CImg = MakeImg(Img->w(),Img->h(),Img->d()*8);
	CPixelPtr ptrDest(CImg);

	for(j=0;j<Img->h();j++)
		for(i=0;i<Img->w();i++)
		{
			ptrDest[j][i] = *(Img->getp(i,j));
		}

	return CImg;
}

CmyImage CImgMisc::ImgConvC_c(ColorImage *Img)
{
	int i,j;
	CmyImage CImg = MakeImg(Img->w(),Img->h(),Img->d()*8);
	CColorPixelPtr ptrDest(CImg);

	for(j=0;j<Img->h();j++)
		for(i=0;i<Img->w();i++)
		{
			ptrDest[j][i].R = *(Img->getp(i,j,0));
			ptrDest[j][i].G = *(Img->getp(i,j,1));
			ptrDest[j][i].B = *(Img->getp(i,j,2));
		}

	return CImg;
}

void CImgMisc::Normalize(double *Img, int w, int h)
{
	int i;
	double max = -65536, min = 65536;
	double scale, offset;

	GrayImage A(w,h);

	for(i=0;i<w*h;i++)
	{
		if (Img[i] >= max)
			max = Img[i];
		else if (Img[i] <= min)
			min = Img[i];
	}

	scale = 255.0/(max-min);
	offset = min;

	for (i=0; i<w*h; i++)
		A.buf[0][i] = (unsigned char)(scale*Img[i]-offset);

	ShowImg(ImgConvG_c(&A));
}

void CImgMisc::drawRectgray(CPixelPtr ptr, int x1, int y1, int x2, int y2, unsigned char val)
{
	int i;

	for (i=x1; i<x2; i++)
	{
		ptr[y1][i] = val;
		ptr[y2][i] = val;
	}

	for (i=y1; i<y2; i++)
	{
		ptr[i][x1] = val;
		ptr[i][x2] = val;
	}
}

void CImgMisc::drawRectcolor(CColorPixelPtr ptr, int x1, int y1, int x2, int y2, unsigned char val)
{
	/*
	int i;

	for (i=x1; i<x2; i++)
	{
		pDest[rect1.top][j] = 128;
		pDest[rect1.bottom][j] = 128;
	}

	for (j=eyebrrect2.left; j<eyebrrect2.right; j++)
	{
		pDest[rect1.top][j] = 128;
		pDest[rect1.bottom][j] = 128;
	}

	for (i=rect1.top; i<rect1.bottom; i++)
	{
		pDest[i][eyebrrect1.left] = 128;
		pDest[i][eyebrrect1.right] = 128;
		pDest[i][eyebrrect2.left] = 128;
		pDest[i][eyebrrect2.right] = 128;
	}
	*/
}

// 16.11.01
// show message in status bar
void CImgMisc::DjMessageBox(CString str, int PaneNum)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	pFrame->m_wndStatusBar.SetPaneText(PaneNum, str);
}
