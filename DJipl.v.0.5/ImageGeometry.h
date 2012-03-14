// ImageGeometry.h: interface for the CImageGeometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEGEOMETRY_H__18ACFE75_E632_11D1_8F00_006097283F10__INCLUDED_)
#define AFX_IMAGEGEOMETRY_H__18ACFE75_E632_11D1_8F00_006097283F10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImagePixel.h"

class GrayImage;
class CmyImage;

class CImageGeometry  
{
public:
	CImageGeometry(CmyImage &Image);
	CImageGeometry();
	virtual ~CImageGeometry();

	/*******************************************************
					인터페이스 함수
	*******************************************************/
	void FlipVertical();
	void FlipHorizontal();
	void TurnRight();
	void TurnLeft();
	void Affine(float deg, float zx, float zy, float px, float py);
	void RCER();
	void Histogram();
	void Feature();
	void hough(char *str, BYTE th);
	// LP Centering
	void LPCenter();

	// general projection function.
	// 07.11.01
	void projection(BOOL xdir, BOOL ydir, BOOL weight);
	// 07.11.01
	// weighted projection
	// dir: 0(x), 1(y)
	long *weighted_projection(GrayImage A, int sizex, int sizey, int dir);

protected:
	/*******************************************************
							멤버 변수들
	*******************************************************/
	CmyImage *m_pImage;
	int m_nWidth, m_nHeight, m_nDepth;
	//CPixelPtr m_ptr;

	/*******************************************************
					실제 이미지 좌표 변환 함수
	*******************************************************/
	void FlipVerticalGray();
	void FlipVerticalColor();

	void FlipHorizontalGray();
	void FlipHorizontalColor();

	void TurnRightGray();
	void TurnRightColor();

	void TurnLeftGray();
	void TurnLeftColor();

	// Affine Transform
	void AffineGray(float deg, float zx, float zy, float px, float py);
	void AffineColor(float deg, float zx, float zy, float px, float py);

	// Perspective Transform
	void Perspect(float zx,float zy,float p_x,float p_y,float p_z, float rot_z,float rot_x,float rot_y,float view,float screen);
	void ParameterCalculate(float k[],float zx,float zy,float p_x,float p_y,float p_z, float rot_z,float rot_x,float rot_y,float view,float screen);
	void Matrix(float ma[][4],float mb[][4],float mc[][4]);

	// RCER routine
	// ... not yet complete
	void RCERGray();
	void RCERSearch(int x, int y);

	// Projection
	void HProj();
	void VProj();
	void HVProj();


	// Get Histogram
	void HistogramGray();
	void HistogramColor();
	float **get_histogramGray(CmyImage *imageP);
	float **get_histogramColor(CmyImage *imageP);

	// imported from CVIPclass
	CmyImage hough_transform(CmyImage* cvipImage,char *interpret,int num);
	char *parse_string(char *input);
	CmyImage hough_inverse(CmyImage *cvipImage,int height, int width,int threshold);
};

#endif // !defined(AFX_IMAGEGEOMETRY_H__18ACFE75_E632_11D1_8F00_006097283F10__INCLUDED_)
