// ImageTransform.cpp: implementation of the CImageTransform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"
#include "ImageTransform.h"
#include "ImagePixel.h"
#include "ImgMisc.h"
#include "ProgressBar.h"
#include "math.h"
#include "lft.h"
#include "CImage.h"
#include "SVV.h"
#include "myresource.h"

#define M_PI	3.1415926535897932384626433832795

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageTransform::CImageTransform()
{
	m_pImage = NULL;
}

CImageTransform::~CImageTransform()
{

}

CImageTransform::CImageTransform(CmyImage &Image)
{
	m_pImage = &Image;
	m_nWidth = m_pImage->GetWidth();
	m_nHeight = m_pImage->GetHeight();
	m_nDepth = m_pImage->GetBitCount();
}


// show image
void CImageTransform::MyLFT()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) MyLFTGray();
	else MyLFTColor();
}

void CImageTransform::MyLFTGray()
{
	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\timeSDFT.txt","at");

	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	//char *filenamei = "c:\\matlabr11\\work\\lftx.dat", *filenameo = "c:\\matlabr11\\work\\\\lfty.dat";
	char strFig[100];
	char strX[100];
	char strY[100];

	CProgressBar bar(_T("영상 LFT"), 100, m_nHeight/2);

	int tmp[5] = {1,2,3,4,5};

	CmyImage A;

	for(int i=0;i<13;i++)
	{
		//sprintf(strFig,"D:\\DJKim\\Research\\Figure\\sDFT\\crow%02d.bmp", tmp[i]);
		sprintf(strFig,"D:\\user\\DJKim\\Research\\Figure\\sDFT\\angle%03d.bmp", i*15);
		A.Load(strFig);
		LFT lft(&A);
		
		//sprintf(strX,"c:\\matlabr11\\work\\lftx_crow%02d.dat", tmp[i]);
		//sprintf(strY,"c:\\matlabr11\\work\\lfty_crow%02d.dat", tmp[i]);
		sprintf(strX,"c:\\matlabr11\\work\\lftx_angle%03d.dat", i*15);
		sprintf(strY,"c:\\matlabr11\\work\\lfty_angle%03d.dat", i*15);

		CHECK_TIME_START;
		lft.xtransform();
		CHECK_TIME_END(Time1, err);

		lft.printMag(strX, 0, 1);

		CHECK_TIME_START;
		lft.ytransform();
		CHECK_TIME_END(Time2, err);

		fprintf(fp, "%f %f \n", Time1, Time2);

		lft.printMag(strY, 0, 1);
	}

	fclose(fp);
}

void CImageTransform::MyLFTColor()
{
	char *filenamei = "./lftx.dat", *filenameo = "./lfty.dat";

	CProgressBar bar(_T("영상 LFT"), 100, m_nHeight/2);

	LFT lft(m_pImage);
	
	lft.xtransform();
	lft.printMag(filenamei, 0, 1);
	lft.ytransform();
	lft.printMag(filenameo, 0, 1);
}

void CImageTransform::MyLP(int horiz, int vert)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) MyLPGray(horiz, vert);
	else MyLPColor(horiz, vert);
}

void CImageTransform::MyLPGray(int horiz, int vert)
{
	CProgressBar bar(_T("LP 변환"), 100, m_nHeight);
	CPixelPtr ptrSorce(m_pImage->GetUndoHandle());

	CmyImage img = CImgMisc::MakeImg(horiz,vert,8);
	CPixelPtr ptrDest(img);

	CSVV LPImg;
	LPImg.LPtable(m_nWidth,m_nHeight,horiz,vert);

	int i,j,k;
	BYTE *sImg = new BYTE[m_nHeight*m_nWidth];
	BYTE *rImg;

	k=0;
	for( j=0 ; j<m_nHeight; j++) 
		for( i=0 ; i<m_nWidth; i++,k++) {
			sImg[k] = (BYTE)ptrSorce[j][i];
		}

	rImg = LPImg.LPtr(sImg,horiz,vert);

	// rImg 정보가 죽은 것은 아니다.
	// ??
	k=0;
	for( i=0 ; i<horiz; i++)	
		for( j=0 ; j<vert; j++,k++)
		{
			ptrDest[j][i] = rImg[k];
		}

	delete []sImg;
	delete []rImg;

	CImgMisc::ShowImg(img);
}

void CImageTransform::MyLPColor(int horiz, int vert)
{
	CProgressBar bar(_T("LP 변환"), 100, m_nHeight);
	CColorPixelPtr ptrSorce(m_pImage->GetUndoHandle());

	CmyImage img = CImgMisc::MakeImg(horiz,vert,24);
	CColorPixelPtr ptrDest(img);

	int i,j,k;
	CSVV LPImg;
	LPImg.LPtable(m_nWidth,m_nHeight,horiz,vert);

	BYTE *sImg[3];
	BYTE *rImg[3];

	for(i=0;i<3;i++)
		sImg[i] = new BYTE[m_nHeight*m_nWidth];

	k=0;
	for(j=0;j<m_nHeight;j++)
		for(i=0;i<m_nWidth;i++,k++)
		{
			sImg[0][k] = (BYTE)ptrSorce[j][i].R;
			sImg[1][k] = (BYTE)ptrSorce[j][i].G;
			sImg[2][k] = (BYTE)ptrSorce[j][i].B;
		}

	for(i=0;i<3;i++)
	{
		rImg[i] = LPImg.LPtr(sImg[i],horiz,vert);
	}

	k=0;
	for(i=0;i<horiz;i++)
		for(j=0;j<vert;j++,k++)
		{
			ptrDest[j][i].R = rImg[0][k];
			ptrDest[j][i].G = rImg[1][k];
			ptrDest[j][i].B = rImg[2][k];
		}

	for(i=0;i<3;i++)
	{
		delete []sImg[i];
		delete []rImg[i];
	}

	CImgMisc::ShowImg(img);
}

void CImageTransform::MyColor(int flag, BOOL flagAll, BOOL flagR, BOOL flagG,BOOL flagB)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	// only color image...
	if(m_nDepth != 8)
	{
		ColorImage A = CImgMisc::ImgConvc_C(*m_pImage);
		ColorSpaceTransform rgb;

//	ColorImage A = CImgMisc::ImgConvc_C(*Image);
	GrayImage R1,G1,B1;
	A.to3Gray(&R1, &G1, &B1);	

	GrayImage I2(R1);
	
	for(int i=0;i<I2.w();i++)
		for(int j=0;j<I2.h();j++)
			I2.setPixel(i,j,R1.getPixel(i,j)-B1.getPixel(i,j));

	I2.save("ttest.bmp");


		switch(flag)
		{
			case 0:
				rgb.setcolorsys(ColorSpaceTransform::RGB);
				break;
			case 1:
				rgb.setcolorsys(ColorSpaceTransform::CMY);
				break;
			case 2:
				rgb.setcolorsys(ColorSpaceTransform::HSV);
				break;
			case 3:
				rgb.setcolorsys(ColorSpaceTransform::HLS);
				break;
			case 4:
				rgb.setcolorsys(ColorSpaceTransform::YIQ);
				break;
			case 5:
				rgb.setcolorsys(ColorSpaceTransform::HSI);
				break;
			case 6:
				rgb.setcolorsys(ColorSpaceTransform::rgI);
				break;
			case 7:
				rgb.setcolorsys(ColorSpaceTransform::gbI);
				break;
			case 8:
				rgb.setcolorsys(ColorSpaceTransform::brI);
				break;
			case 9:
				rgb.setcolorsys(ColorSpaceTransform::rgb);
				break;
			case 10:
				rgb.setcolorsys(ColorSpaceTransform::I1I2I3);
				break;
			case 11:
				rgb.setcolorsys(ColorSpaceTransform::I1I2I3N);
				break;
			case 12:
				rgb.setcolorsys(ColorSpaceTransform::YCRCB);
				break;
			case 13:
				rgb.setcolorsys(ColorSpaceTransform::I1I2I3NN);
				break;
			case 14:
				rgb.setcolorsys(ColorSpaceTransform::STV);
				break;
			default:
				break;
		}

		// clock start!
		//clock_t start = clock();

		rgb.fromtransform(A);

		//clock_t finish = clock();
		// clock end!
		//double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		//CString str;
		//str.Format("Conversion time : %.3f sec", duration);
		//AfxMessageBox(str);

		GrayImage R,G,B;
		A.to3Gray(&R, &G, &B);	

		if (flagAll)
			CImgMisc::ShowImg(CImgMisc::ImgConvC_c(&A));
		if (flagR)
			CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&R));
		if (flagG)
			CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&G));
		if (flagB)
			CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&B));
	}
}

void CImageTransform::MyRLP(int w, int h)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) MyRLPGray(w, h);
	else MyRLPColor(w, h);
}

void CImageTransform::MyRLPColor(int w, int h)
{
	CProgressBar bar(_T("LP Remapping"), 100, m_nHeight);
	CColorPixelPtr ptrSorce(m_pImage->GetUndoHandle());

	int y = min(w,h);
	CmyImage img = CImgMisc::MakeImg(y,y,24);
	CColorPixelPtr ptrDest(img);

	int i,j,k;
	CSVV LPImg;
	LPImg.LPtable(w,h,m_nWidth,m_nHeight);

	BYTE *sImg[3];
	BYTE *oImg[3];

	for(i=0;i<3;i++)
		sImg[i] = new BYTE[m_nHeight*m_nWidth];

	k=0;
	for(i=0;i<m_nWidth;i++)
		for(j=0;j<m_nHeight;j++,k++)
		{
			sImg[0][k] = (BYTE)ptrSorce[j][i].R;
			sImg[1][k] = (BYTE)ptrSorce[j][i].G;
			sImg[2][k] = (BYTE)ptrSorce[j][i].B;
		}

	for(i=0;i<3;i++)
	{
		oImg[i] = LPImg.LPtrInv(sImg[i],y,y);
	}

	k=0;
	for(j=0;j<y;j++)
		for(i=0;i<y;i++,k++)
		{
			ptrDest[j][i].R = oImg[0][k];
			ptrDest[j][i].G = oImg[1][k];
			ptrDest[j][i].B = oImg[2][k];
		}

	for(i=0;i<3;i++)
	{
		delete []sImg[i];
		delete []oImg[i];
	}

	CImgMisc::ShowImg(img);
}

void CImageTransform::MyRLPGray(int w, int h)
{
	CProgressBar bar(_T("LP Remapping"), 100, m_nHeight);
	CPixelPtr ptrSorce(m_pImage->GetUndoHandle());
	int y = min(w,h);

	CmyImage img = CImgMisc::MakeImg(y,y,8);
	CPixelPtr ptrDest(img);

	int i,j,k;
	CSVV LPImg;
	LPImg.LPtable(w,h,m_nWidth,m_nHeight);

	BYTE *sImg = new BYTE[m_nWidth*m_nHeight];
	BYTE *oImg;

	k=0;
	for(i=0;i<m_nWidth;i++)
		for(j=0;j<m_nHeight;j++,k++)
		{
			sImg[k] = (BYTE)ptrSorce[j][i];
		}

	oImg = LPImg.LPtrInv(sImg,y,y);

	k=0;
	for(j=0;j<y;j++)
		for(i=0;i<y;i++,k++)
		{
			ptrDest[j][i] = oImg[k];
		}

	delete []sImg;
	delete []oImg;

	CImgMisc::ShowImg(img);
}