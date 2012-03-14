// ImageGeometry.cpp: implementation of the CImageGeometry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Image.h"
#include "ImageGeometry.h"
#include "ImagePixel.h"
#include "ImgMisc.h"
#include "ProgressBar.h"
#include "math.h"
#include "Algebra.h"
#include "moment.h"
#include "FaceTool.h"

#include "histogram.h"
#include "CImage.h"

#define RCERTH	(BYTE)128

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageGeometry::CImageGeometry()
{
	m_pImage = NULL;
}

CImageGeometry::~CImageGeometry()
{
	
}

CImageGeometry::CImageGeometry(CmyImage &Image)
{
	m_pImage = &Image;
	m_nWidth = m_pImage->GetWidth();
	m_nHeight = m_pImage->GetHeight();
	m_nDepth = m_pImage->GetBitCount();
}

void CImageGeometry::FlipVertical()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);
	
	if(m_nDepth == 8) FlipVerticalGray();
	else FlipVerticalColor();
}

void CImageGeometry::FlipVerticalGray()
{
	CProgressBar bar(_T("상하로 뒤집기"), 100, m_nHeight/2);
	BYTE *temp;

	CPixelPtr ptr(*m_pImage);
	temp = new BYTE [m_nWidth];

	for(int j=0 ; j<m_nHeight/2 ; j++)
	{
		memcpy(temp, ptr[j], m_nWidth);
		memcpy(ptr[j], ptr[m_nHeight-j-1], m_nWidth);
		memcpy(ptr[m_nHeight-j-1], temp, m_nWidth);
		bar.StepIt();
	}
	delete temp;
}

void CImageGeometry::FlipVerticalColor()
{
	CProgressBar bar(_T("상하로 뒤집기"), 100, m_nHeight/2);
	BYTE *temp;

	CColorPixelPtr ptrColor(*m_pImage);
	temp = new BYTE [m_nWidth*3];

	for(int j=0 ; j<m_nHeight/2 ; j++)
	{
		memcpy(temp, ptrColor[j], m_nWidth*3);
		memcpy(ptrColor[j], ptrColor[m_nHeight-j-1], m_nWidth*3);
		memcpy(ptrColor[m_nHeight-j-1], temp, m_nWidth*3);
		bar.StepIt();
	}
	delete temp;
}

void CImageGeometry::FlipHorizontal()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) FlipHorizontalGray();
	else FlipHorizontalColor();
}

void CImageGeometry::FlipHorizontalGray()
{
	CProgressBar bar(_T("좌우로 뒤집기"), 100, m_nHeight);

	CPixelPtr ptr(*m_pImage);
	BYTE g;

	for(int j=0 ; j<m_nHeight ; j++)
	{
		for(int i=0 ; i<m_nWidth/2; i++)
		{
			g = ptr[j][i];
			ptr[j][i] = ptr[j][m_nWidth-i-1];
			ptr[j][m_nWidth-i-1] = g;
		}
		bar.StepIt();
	}
}

void CImageGeometry::FlipHorizontalColor()
{
	CProgressBar bar(_T("좌우로 뒤집기"), 100, m_nHeight);

	CColorPixelPtr ptrColor(*m_pImage);
	CColorPixel a, b;

	for(int j=0 ; j<m_nHeight ; j++)
	{
		for(int i=0 ; i<m_nWidth/2 ; i++)
		{
			a << ptrColor[j][i];
			b << ptrColor[j][m_nWidth-i-1];
			a >> ptrColor[j][m_nWidth-i-1];
			b >> ptrColor[j][i];
		}
		bar.StepIt();
	}
}


void CImageGeometry::TurnRight()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) TurnRightGray();
	else TurnRightColor();
}

void CImageGeometry::TurnRightGray()
{
	CProgressBar bar(_T("오른쪽으로 회전"), 100, m_nWidth);

	GlobalFree(m_pImage->GetHandle());
	m_pImage->Create(m_nHeight, m_nWidth, m_nDepth);
	m_pImage->InitDIB(FALSE);

	CPixelPtr Destptr(*m_pImage);
	CPixelPtr ptr(m_pImage->GetUndoHandle());

	for(int j=0 ; j<m_nWidth ; j++)
	{
		for(int i=0 ; i<m_nHeight ; i++)
			Destptr[j][m_nHeight-i-1] = ptr[i][j];
		bar.StepIt();
	}
}

void CImageGeometry::TurnRightColor()
{
	CProgressBar bar(_T("오른쪽으로 회전"), 100, m_nWidth);

	GlobalFree(m_pImage->GetHandle());
	m_pImage->Create(m_nHeight, m_nWidth, m_nDepth);
	m_pImage->InitDIB(FALSE);

	CColorPixelPtr DestptrColor(*m_pImage);
	CColorPixelPtr ptrColor(m_pImage->GetUndoHandle());

	for(int j=0 ; j<m_nWidth ; j++)
	{
		for(int i=0 ; i<m_nHeight ; i++)
			memcpy(&DestptrColor[j][m_nHeight-i-1], &ptrColor[i][j], 3);
		bar.StepIt();
	}
}


void CImageGeometry::TurnLeft()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) TurnLeftGray();
	else TurnLeftColor();
}

void CImageGeometry::TurnLeftGray()
{
	CProgressBar bar(_T("왼쪽으로 회전"), 100, m_nWidth);

	GlobalFree(m_pImage->GetHandle());
	m_pImage->Create(m_nHeight, m_nWidth, m_nDepth);
	m_pImage->InitDIB(FALSE);

	CPixelPtr Destptr(*m_pImage);
	CPixelPtr ptr(m_pImage->GetUndoHandle());

	for(int j=0 ; j<m_nWidth ; j++)
	{
		for(int i=0 ; i<m_nHeight ; i++)
			Destptr[m_nWidth-j-1][i] = ptr[i][j];
		bar.StepIt();
	}
}

void CImageGeometry::TurnLeftColor()
{
	CProgressBar bar(_T("왼쪽으로 회전"), 100, m_nWidth);

	GlobalFree(m_pImage->GetHandle());
	m_pImage->Create(m_nHeight, m_nWidth, m_nDepth);
	m_pImage->InitDIB(FALSE);

	CColorPixelPtr DestptrColor(*m_pImage);
	CColorPixelPtr ptrColor(m_pImage->GetUndoHandle());

	for(int j=0 ; j<m_nWidth ; j++)
	{
		for(int i=0 ; i<m_nHeight ; i++)
			memcpy(&DestptrColor[m_nWidth-j-1][i], &ptrColor[i][j], 3);
		bar.StepIt();
	}
}

void CImageGeometry::Affine(float deg, float zx, float zy, float px, float py)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) AffineGray(deg, zx, zy, px, py);
	else AffineColor( deg,  zx,  zy,  px,  py);
}

/*****************************/
/* function for Affin transform      */
/*****************************/
void CImageGeometry::AffineGray(float deg, float zx, float zy, float px, float py)
{
	CPixelPtr ptr(m_pImage->GetUndoHandle());
	CPixelPtr Destptr(*m_pImage);
	CPixel op;

	 int i, j, buf1, buf2, u, v;
	 float x, y, p, q;
	 int xs=m_nWidth/2;
	 int ys=m_nHeight/2;
	 double r;
	 float c, s;
	 int data;

	 r = deg * M_PI/180.0;   /* radian 값  */
	 c = (float)cos(r);    /* cosine 값  */
	 s = (float)sin(r);     /* sine 값  */


	 for (i=-ys; i<ys; i++) {
		   for (j=-xs; j<xs; j++) {
				 v=(int)(i - py);              /*  v는 출력 좌표에서 위치이동한 값을 뺀 것 */
				 u=(int)(j - px);              /*  u는 출력 좌표에서 위치이동한 값을 뺀 것 */
				 y=(u*s + v*c)/zy;      /*  확대율로 나눈다. */
				 x=(u*c - v*s)/zx;    

				 if (y>0) buf1 = (int)y;        /* 양수인 경우 */
				 else     buf1 = (int)(y - 1);  /* 음수인 경우 */
				 if (x>0) buf2 = (int)x;
				 else     buf2 = (int)(x - 1);

				 q = y - buf1;
				 p = x - buf2;

				/* -SIZE/2<buf1<SIZE/2, -SIZE/2<buf2<SIZE/2의 경우 */
				 if ((buf1 > -ys) && (buf1 < ys-1) && (buf2 > -xs) && (buf2 < xs-1))
					 data = (int)((1.0-q)*((1.0-p)*ptr[buf1+ys][buf2+xs]
					  + p*ptr[buf1+ys][buf2+1+xs])
					  + q*((1.0-p)*ptr[buf1+1+ys][buf2+xs]
					  + p*ptr[buf1+1+ys][buf2+1+xs]));
				 else
					  data = 0;   /* 범위에 들어가지 않는 경우 */

				 data = __min(255,__max(data,0));

				 op = data;
				 op >> Destptr[i+ys][j+xs];
				 /* 그외의 경우는 data를 mapping 한 위치에 저장한다. */
		  }
	 }
}

void CImageGeometry::AffineColor(float deg, float zx, float zy, float px, float py)
{
	CColorPixelPtr ptr(m_pImage->GetUndoHandle());
	CColorPixelPtr Destptr(*m_pImage);

	 int i, j, buf1, buf2, u, v;
	 float x, y, p, q;
	 int xs=m_nWidth/2;
	 int ys=m_nHeight/2;
	 double r;
	 float c, s;
	 int data[3];

	 r = deg * M_PI/180.0;   /* radian 값  */
	 c = (float)cos(r);    /* cosine 값  */
	 s = (float)sin(r);     /* sine 값  */

	 for (i=-ys; i<ys; i++) {
		   for (j=-xs; j<xs; j++) {
				 v=(int)(i - py);              /*  v는 출력 좌표에서 위치이동한 값을 뺀 것 */
				 u=(int)(j - px);              /*  u는 출력 좌표에서 위치이동한 값을 뺀 것 */
				 y=(u*s + v*c)/zy;      /*  확대율로 나눈다. */
				 x=(u*c - v*s)/zx;    

				 if (y>0) buf1 = (int)y;        /* 양수인 경우 */
				 else     buf1 = (int)(y - 1);  /* 음수인 경우 */
				 if (x>0) buf2 = (int)x;
				 else     buf2 = (int)(x - 1);

				 q = y - buf1;
				 p = x - buf2;

				/* -SIZE/2<buf1<SIZE/2, -SIZE/2<buf2<SIZE/2의 경우 */
				 if ((buf1 > -ys) && (buf1 < ys-1) && (buf2 > -xs) && (buf2 < xs-1))
				 {
					 data[0] = (int)((1.0-q)*((1.0-p)*ptr[buf1+ys][buf2+xs].R
					  + p*ptr[buf1+ys][buf2+1+xs].R)
					  + q*((1.0-p)*ptr[buf1+1+ys][buf2+xs].R
					  + p*ptr[buf1+1+ys][buf2+1+xs].R));
					 data[1] = (int)((1.0-q)*((1.0-p)*ptr[buf1+ys][buf2+xs].G
					  + p*ptr[buf1+ys][buf2+1+xs].G)
					  + q*((1.0-p)*ptr[buf1+1+ys][buf2+xs].G
					  + p*ptr[buf1+1+ys][buf2+1+xs].G));
					 data[2] = (int)((1.0-q)*((1.0-p)*ptr[buf1+ys][buf2+xs].B
					  + p*ptr[buf1+ys][buf2+1+xs].B)
					  + q*((1.0-p)*ptr[buf1+1+ys][buf2+xs].B
					  + p*ptr[buf1+1+ys][buf2+1+xs].B));
				 }
				 else
					  data[0] = data[1] = data[2] = 0;   /* 범위에 들어가지 않는 경우 */

				 data[0] = __min(255,__max(data[0],0));
				 data[1] = __min(255,__max(data[1],0));
				 data[2] = __min(255,__max(data[2],0));
				 
				 Destptr[i+ys][j+xs].R = data[0];
				 Destptr[i+ys][j+xs].G = data[1];
				 Destptr[i+ys][j+xs].B = data[2];
				 /* 그외의 경우는 data를 mapping 한 위치에 저장한다. */
		  }
	 }
}

/*********************************/
/* function to get perspect transform */
/*********************************/
void CImageGeometry::Perspect(float zx,float zy,float p_x,float p_y,float p_z, float rot_z,float rot_x,float rot_y,float view,float screen)
{
	CPixelPtr ptr(m_pImage->GetUndoHandle());
	CPixelPtr Destptr(*m_pImage);
	CPixel op;

     int i,j,buf1,buf2; /* declaration */
     float x,y,w,p,q;
     float k[9];
     int xs = m_nWidth/2;
     int ys = m_nHeight/2;
     int data;


     /* compute the parameter value */
     ParameterCalculate(k,zx,zy,p_x,p_y,p_z,rot_z,rot_x,rot_y,view,screen);
     for(i = -ys; i < ys; i++) {
         for(j = -xs; j < xs; j++) {
               w = k[0]*j + k[1]*i + k[2];
                x = k[3]*j + k[4]*i + k[5];
                y = k[6]*j + k[7]*i + k[8];
                x = x/w;
                y = y/w;
                if(y > 0) buf1 = (int)y; else buf1 = (int)(y-1);
                if(x > 0) buf2 = (int)x; else buf2 = (int)(x-1);
                q = y - buf1;
                p = x - buf2;
                if((buf1 > -ys) && (buf1 < ys) && (buf2 > -xs) && (buf2 < xs))
                    data = (int)((1.0-q)*((1.0-p)*ptr[buf1+ys][buf2+xs]
					  + p*ptr[buf1+ys][buf2+1+xs])
					  + q*((1.0-p)*ptr[buf1+1+ys][buf2+xs]
					  + p*ptr[buf1+1+ys][buf2+1+xs]));
                else
                    data = 0;
 
                if(data < 0) data = 0;
                if(data > 255) data = 255;
   
				 op = data;
				 op >> Destptr[i+ys][j+xs];
        }
    }
}

 
 
 /***************************************/
// function to compute parameter value
/***************************************/
void CImageGeometry::ParameterCalculate(float k[],float zx,float zy,float p_x,float p_y,float p_z, float rot_z,float rot_x,float rot_y,float view,float screen)
{
      float ma[4][4],mb[4][4],mc[4][4],k1,k2,k3,k4,k5,k6,k7,k8,k9;
      double u,v,w;
      int xs = m_nWidth/2;


      /* rotation degree on x,y,z axis */
      u = rot_x*3.141592/180.0; v = rot_y*3.141592/180.0; w = rot_z*3.141592/180.0;


      /* nomalization matrix */
      ma[0][0] = (float)(1.0/xs); ma[0][1] = 0; ma[0][2] = 0; ma[0][3] = 0;
      ma[1][0] = 0; ma[1][1] =-(float)(1.0/xs);ma[1][2] = 0; ma[1][3] = 0;
      ma[2][0] = 0; ma[2][1] = 0; ma[2][2] = 1; ma[2][3] = 0;
      ma[3][0] = 0; ma[3][1] = 0; ma[3][2] = 0; ma[3][3] = 1;


      /* matrix for expansion and contraction */
      mb[0][0] =zx; mb[0][1] = 0; mb[0][2] = 0; mb[0][3] = 0;
      mb[1][0] = 0; mb[1][1] =zy; mb[1][2] = 0; mb[1][3] = 0;
      mb[2][0] = 0; mb[2][1] = 0; mb[2][2] = 1; mb[2][3] = 0;
      mb[3][0] = 0; mb[3][1] = 0; mb[3][2] = 0; mb[3][3] = 1;
      Matrix(ma,mb,mc);


      /* matrix for shift */
      ma[0][0] = 1; ma[0][1] = 0; ma[0][2] = 0; ma[0][3] = 0;
      ma[1][0] = 0; ma[1][1] = 1; ma[1][2] = 0; ma[1][3] = 0;
      ma[2][0] = 0; ma[2][1] = 0; ma[2][2] = 1; ma[2][3] = 0;
      ma[3][0] = p_x; ma[3][1] = p_y; ma[3][2] = p_z; ma[3][3] = 1;
      Matrix(mc,ma,mb);


      /* matrix for rotation on z axis */
      mc[0][0] = (float)cos(w); mc[0][1] = (float)sin(w);mc[0][2] = 0; mc[0][3] = 0;
      mc[1][0] = -(float)sin(w); mc[1][1] = (float)cos(w);mc[1][2] = 0; mc[1][3] = 0;
      mc[2][0] = 0; mc[2][1] = 0; mc[2][2] = 1; mc[2][3] = 0;
      mc[3][0] = 0; mc[3][1] = 0; mc[3][2] = 0; mc[3][3] = 1;
      Matrix(mb,mc,ma);


      /* matrix for rotation on x axis */
      mb[0][0] = 1; mb[0][1] = 0; mb[0][2] = 0; mb[0][3] = 0;
      mb[1][0] = 0; mb[1][1] = (float)cos(u);mb[1][2]=(float)sin(u);mb[1][3] = 0;
      mb[2][0] = 0; mb[2][1] =-(float)sin(u);mb[2][2]=(float)cos(u);mb[2][3] = 0;
      mb[3][0] = 0; mb[3][1] = 0; mb[3][2] = 0; mb[3][3] = 1;
      Matrix(ma,mb,mc);


      /* matrix for rotation on y axis */
      ma[0][0] = (float)cos(v); ma[0][1] = 0; ma[0][2]=(float)sin(v);ma[0][3] = 0;
      ma[1][0] = 0; ma[1][1] = 1; ma[1][2] = 0; ma[1][3] = 0;
      ma[2][0] = -(float)sin(v); ma[2][1] = 0; ma[2][2]=(float)cos(v);ma[2][3] = 0;
      ma[3][0] = 0; ma[3][1] = 0; ma[3][2] = 0; ma[3][3] = 1;
      Matrix(mc,ma,mb);


      /* matrix to change view point */
      mc[0][0] = 1; mc[0][1] = 0; mc[0][2] = 0; mc[0][3] = 0;
      mc[1][0] = 0; mc[1][1] = 1; mc[1][2] = 0; mc[1][3] = 0;
      mc[2][0] = 0; mc[2][1] = 0; mc[2][2] = -1; mc[2][3] = 0;
      mc[3][0] = 0; mc[3][1] = 0; mc[3][2] =view; mc[3][3] = 1;
      Matrix(mb,mc,ma);


      /* matrix for perspect transform */
      mb[0][0] = 1; mb[0][1] = 0; mb[0][2] = 0; mb[0][3] = 0;
      mb[1][0] = 0; mb[1][1] = 1; mb[1][2]= 0; mb[1][3] = 0;
      mb[2][0] = 0; mb[2][1] = 0; mb[2][2]=1/screen; mb[2][3] = 1/screen;
      mb[3][0] = 0; mb[3][1] = 0; mb[3][2] = -1; mb[3][3] = 1;
      Matrix(ma,mb,mc);


      /* normaization matrix */
      ma[0][0] = (float)xs; ma[0][1] = 0; ma[0][2]= 0; ma[0][3] = 0;
      ma[1][0] = 0; ma[1][1] = -(float)xs; ma[1][2] = 0; ma[1][3] = 0;
      ma[2][0] = 0; ma[2][1] = 0; ma[2][2]= 1; ma[2][3] = 0;
      ma[3][0] = 0; ma[3][1] = 0; ma[3][2] = 0; ma[3][3] = 1;
      Matrix(mc,ma,mb);


      k1 = mb[0][3]; k2 = mb[1][3] ; k3 = mb[3][3];
      k4 = mb[0][0]; k5 = mb[1][0] ; k6 = mb[3][0];
      k7 = mb[0][1]; k8 = mb[1][1] ; k9 = mb[3][1];


      k[0] = k7*k2 - k8*k1; k[1] = k5*k1 - k4*k2; k[2] = k4*k8 - k7*k5;
      k[3] = k8*k3 - k9*k2; k[6] = k9*k1 - k7*k3; k[4] = k6*k2 - k5*k3;
      k[7] = k4*k3 - k6*k1; k[5] = k5*k9 - k8*k6; k[8] = k7*k6 - k4*k9;

}
 
 
 /******************************/
// function for matrix multipication
/******************************/
void CImageGeometry::Matrix(float ma[][4],float mb[][4],float mc[][4])
{
       int i,j,k;
       float p;
 
       for(i = 0; i < 4; i++) {
            for(j = 0; j < 4; j++) {
                 p = 0;
                 for(k = 0; k < 4; k++) p = p + ma[i][k]*mb[k][j]; // multiplication
                 mc[i][j] = p;
            }
      }
}

// RCER operation
void CImageGeometry::RCER()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if(m_nDepth == 8) RCERGray();
}

void CImageGeometry::RCERGray()
{
	RCERSearch(m_nWidth/2,m_nHeight/2);
}

void CImageGeometry::RCERSearch(int x, int y)
{
	static CPixelPtr m_ptr(*m_pImage);
	int z;

	m_ptr[y][x] = 0;

	if ((x<1) || (x>m_nWidth) || (y<1) || (y>m_nHeight))
		z = 0;

	if ((x>1) && (x<m_nWidth-1) && (y>1) && (y<m_nHeight-1))
	{
		if (m_ptr[y-1][x-1] <= RCERTH)
			RCERSearch(x-1,y-1);
		if (m_ptr[y][x-1] <= RCERTH)
			RCERSearch(x-1,y);
		if (m_ptr[y+1][x-1] <= RCERTH)
			RCERSearch(x-1,y+1);
		if (m_ptr[y-1][x] <= RCERTH)
			RCERSearch(x,y-1);
		if (m_ptr[y+1][x] <= RCERTH)
			RCERSearch(x,y+1);
		if (m_ptr[y-1][x+1] <= RCERTH)
			RCERSearch(x+1,y-1);
		if (m_ptr[y][x+1] <= RCERTH)
			RCERSearch(x+1,y);
		if (m_ptr[y+1][x+1] <= RCERTH)
			RCERSearch(x+1,y+1);
	}
}

// LP centering
void CImageGeometry::LPCenter()
{
	int offset = 1;
	CPixelPtr pSrc(*m_pImage);

	int i,j;
	int maxU, minU, maxV, minV, tmpV;
	int uhs, vhs;
//	char flag = 0;

	int *VPvec;

	BinaryImage A = CImgMisc::ImgConvc_B(*m_pImage);

	VPvec = A.proj(BinaryImage::X,offset,offset,A.w()-offset,A.h()-offset);

//	ProfileData H(m_nWidth,VPvec);

	minU = 65536;
	maxV = -1;
	// find minU, maxV
	for(j=offset;j<m_nWidth-offset;j++)
	{
		if (VPvec[j-offset]>0)
		{
			minU = j;
			for(i=offset;i<m_nHeight-offset;i++)
				if (A.getPixel(j,i)>0)
				{
					maxV = i;
					break;
				}
			break;
		}
	}

	maxU = -1;
	minV = 65536;
	// find maxU, minV
	for(j=m_nWidth-offset;j>minU;j--)
	{
		if (VPvec[j-offset]>0)
		{
			maxU = j;
			for(i=m_nHeight-offset;i>offset;i--)
				if (A.getPixel(j,i)>0)
				{
					minV = i;
					break;
				}
			break;
		}
	}

	if (maxV < minV)
	{
//		flag = 1;
		tmpV = maxV;
		maxV = minV;
		minV = tmpV;
	}

	uhs = vhs = 0;
	for(j=offset;j<m_nHeight-offset;j++)
		for(i=minU;i<=maxU;i++)
		{
			if (A.getPixel(i,j)>0)
			{
				if (j<m_nHeight/2)
					uhs += i-(maxU+minU)/2;
				else
					vhs += i-(maxU+minU)/2;
			}
		}

	char out[100];
	sprintf(out,"U = %03d,%03d, V = %03d,%03d, UHS = %04d, VHS = %04d", minU, maxU, minV, maxV, uhs, vhs);
	AfxMessageBox(out);

	for(j=offset;j<m_nHeight-offset;j++)
	{
/*		if (flag == 1)
		{
			pSrc[j+minV][minU] = 128;
			pSrc[j+maxV][maxU] = 128;
		}
		else
		{
*/			pSrc[j][minU] = 128;
			pSrc[j][maxU] = 128;
//		}
	}

	for(i=offset;i<m_nWidth-offset;i++)
	{
/*		if (flag == 1)
		{
			pSrc[minV][i+minU] = 128;
			pSrc[maxV][i+maxU] = 128;
		}
		else
		{
*/			pSrc[minV][i] = 128;
			pSrc[maxV][i] = 128;
//		}
	}

	delete []VPvec;
	VPvec = NULL;
}



// 20010309@1:15a@DJ
void CImageGeometry::HProj()
{
	CPixelPtr pSrc(m_pImage->GetUndoHandle());
	CmyImage image = CImgMisc::MakeImg(m_nWidth*2,m_nHeight,8);
	CPixelPtr pDest(image);

	int i,j, maxH;

	int *HPvec = new int[m_nHeight];

	CPixel p;
	GrayImage A = CImgMisc::ImgConvc_G(*m_pImage);

	for (j=0;j<m_nHeight;j++)
		HPvec[j] = 0;

	HPvec = A.proj(GrayImage::Y, 0, 0, A.w(), A.h());

	CFaceTool face;
	CRect rect1, rect2;
	rect1.left = rect2.left = 0;
	rect1.right = rect2.right = A.w()-1;

	face.eyesExtract(HPvec, m_nHeight, &rect1, &rect2);

	// plot projection result
	ProfileData H(m_nHeight,HPvec);
	maxH = H.getMax();

	// copy org. image
	for (j=0;j<m_nHeight;j++)
		memcpy(pDest[j], pSrc[j], m_nWidth);

	// draw projection line
	int HPmax;
	int color = 8;
	for (j=0;j<m_nHeight;j++)
	{
		if (HPvec[j] != 0 && HPvec[j-1] == 0)
			color += 8;

		HPmax = (int)ceil((HPvec[j]/(float)maxH)*m_nWidth);
		for (i=0;i<HPmax;i++)
		{
			pDest[j][i+m_nWidth] = 255;//color;
		}
	}

	/*
	// eyebrows
	for (j=rect1.left; j<rect1.right; j++)
	{
		pDest[rect1.top][j] = 128;
		pDest[rect1.bottom][j] = 128;
	}

	for (j=rect2.left; j<rect2.right; j++)
	{
		pDest[rect2.top][j] = 128;
		pDest[rect2.bottom][j] = 128;
	}

	for (i=rect1.top; i<rect1.bottom; i++)
	{
		pDest[i][rect1.left] = 128;
		pDest[i][rect1.right] = 128;
	}

	for (i=rect2.top; i<rect2.bottom; i++)
	{
		pDest[i][rect2.left] = 128;
		pDest[i][rect2.right] = 128;
	}
	*/
	
	//CImgMisc::drawRectgray(pDest, rect1.left, rect1.top, rect1.right, rect1.bottom, 128);
	//CImgMisc::drawRectgray(pDest, rect2.left, rect2.top, rect2.right, rect2.bottom, 128);

	// get each eye
	HPvec = A.proj(GrayImage::X, 0, rect2.top, A.w(), rect2.bottom);

	CRect eyerect1, eyerect2;
	eyerect1 = eyerect2 = rect2;
	face.eyeExtract2(HPvec, m_nWidth, &eyerect1, &eyerect2);
	
	// get each eyebrow
	HPvec = A.proj(GrayImage::X, 0, rect1.top, A.w(), rect1.bottom);

	CRect eyebrrect1, eyebrrect2;
	eyebrrect1 = eyebrrect2 = rect1;
	face.eyeExtract2(HPvec, m_nWidth, &eyebrrect1, &eyebrrect2);

	/*
	// eyebrow
	for (j=eyebrrect1.left; j<eyebrrect1.right; j++)
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

	// eye
	for (j=eyerect1.left; j<eyerect1.right; j++)
	{
		pDest[rect2.top][j] = 128;
		pDest[rect2.bottom][j] = 128;
	}

	for (j=eyerect2.left; j<eyerect2.right; j++)
	{
		pDest[rect2.top][j] = 128;
		pDest[rect2.bottom][j] = 128;
	}
	
	for (i=rect2.top; i<rect2.bottom; i++)
	{
		pDest[i][eyerect1.left] = 128;
		pDest[i][eyerect1.right] = 128;
		pDest[i][eyerect2.left] = 128;
		pDest[i][eyerect2.right] = 128;
	}
	*/
	
	delete []HPvec;

	CImgMisc::ShowImg(image);
}

void CImageGeometry::VProj()
{
	CPixelPtr pSrc(m_pImage->GetUndoHandle());
	CmyImage image = CImgMisc::MakeImg(m_nWidth,m_nHeight*2,8);
	CPixelPtr pDest(image);

	int i,j, maxV;

	int *VPvec = new int[m_nWidth];

	CPixel p;

	GrayImage A = CImgMisc::ImgConvc_G(*m_pImage);

	for (i=0;i<m_nWidth;i++)
		VPvec[i] = 0;

	VPvec = A.proj(GrayImage::X,0,0,A.w(),A.h());

	ProfileData V(m_nWidth,VPvec);

	maxV = V.getMax();

	for (j=0;j<m_nHeight;j++)
		memcpy(pDest[j], pSrc[j], m_nWidth);
		
	float VPmax;
	for (i=0;i<m_nWidth;i++)
	{
		VPmax = (float)ceil((VPvec[i]/(float)maxV)*m_nHeight);
		for (j=0;j<VPmax;j++)
		{
			// Hproj
			pDest[j+m_nHeight][i] = 255;
		}
	}

	delete []VPvec;
	VPvec = NULL;

	// image save
	image.Save("./VProj.bmp");
	CImgMisc::ShowImg(image);
}

void CImageGeometry::HVProj()
{
	CPixelPtr pSrc(m_pImage->GetUndoHandle());
	CmyImage image = CImgMisc::MakeImg(m_nWidth*2,m_nHeight*2,8);
	CPixelPtr pDest(image);

	int i,j, maxH, maxV;

	int *HPvec = new int[m_nHeight];
	int *VPvec = new int[m_nWidth];

	CPixel p;

	GrayImage A = CImgMisc::ImgConvc_G(*m_pImage);

	for (i=0;i<m_nWidth;i++)
		VPvec[i] = 0;
	for (j=0;j<m_nHeight;j++)
		HPvec[j] = 0;

	HPvec = A.proj(GrayImage::Y,0,0,A.w(),A.h());
	VPvec = A.proj(GrayImage::X,0,0,A.w(),A.h());

	ProfileData H(m_nHeight,HPvec);
	ProfileData V(m_nWidth,VPvec);

	maxH = H.getMax();
	maxV = V.getMax();

	for (j=0;j<m_nHeight;j++)
		memcpy(pDest[j], pSrc[j], m_nWidth);
		
	float HPmax;
	for (j=0;j<m_nHeight;j++)
	{
		HPmax = (float)ceil((HPvec[j]/(float)maxH)*m_nWidth);
		for (i=0;i<HPmax;i++)
		{
			// Hproj
			pDest[j][i+m_nWidth] = 255;
		}
	}

	float VPmax;
	for (i=0;i<m_nWidth;i++)
	{
		VPmax = (float)ceil((VPvec[i]/(float)maxV)*m_nHeight);
		for (j=0;j<VPmax;j++)
		{
			// Hproj
			pDest[j+m_nHeight][i] = 255;
		}
	}

	delete []HPvec;
	delete []VPvec;
	HPvec = VPvec = NULL;

	// image save & show
	image.Save("./HVProj.bmp");
	CImgMisc::ShowImg(image);
}

void CImageGeometry::Histogram()
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	if (m_nDepth == 8) HistogramGray();
	else HistogramColor();
}

void CImageGeometry::HistogramGray()
{
	int i,j;
	//int th;
	int th0, th1;
	
	GrayImage g = CImgMisc::ImgConvc_G(*m_pImage);
	int no_pixel = g.w()*g.h();

	unsigned char *gMat = new unsigned char[no_pixel];
	
	g.copyTo(gMat,NULL,NULL);
	Hist1d hist(65536,-1,256);
	hist.addtohist(gMat,no_pixel);

	//hist.DJotsu(&th);
	hist.DJotsu(&th0, &th1);

	int *hist1 = hist.gets();
	int histmax = hist.getMax();
	int histMean = (int)hist.getMean();

/*	float **histogram = get_histogramGray(m_pImage);
	float histmax = -1;
	int i,j;

	for(i=0;i<256;i++)
		histmax = __max(histmax,histogram[0][i]);
		*/

	CmyImage image = CImgMisc::MakeImg(266,138,8);
	CPixelPtr pDest(image);

	float VPmax;
	for (i=4;i<260;i++)
	{
		//VPmax = ceil((histogram[0][i-4]/histmax)*127);
		VPmax = (float)ceil(((float)hist1[i-4]/(float)histmax)*127);
		for(j=132;j>132-(int)VPmax;j--)
			pDest[j][i] = 128;

		if (i-4 == histMean)
			for(j=0;j<138;j++)
				pDest[j][i] = 128;

		if (i-4 == th0)
			for(j=0;j<138;j++)
				pDest[j][i] = 255;

		if (i-4 == th1)
			for(j=0;j<138;j++)
				pDest[j][i] = 255;

		pDest[132][i] = 255;
	}

	for(i=132;i>4;i--)
		pDest[i][4] = 255;

	CImgMisc::ShowImg(image);

//	delete [] hist1;
	delete [] gMat;

	char str[100];
	sprintf(str, "Max = %d Mean = %d th0 = %d th1 = %d", histmax, histMean, th0, th1);
	AfxMessageBox(str);

	//free(histogram);
}

void CImageGeometry::HistogramColor()
{
	ColorImage g = CImgMisc::ImgConvc_C(*m_pImage);
	int no_pixel = g.w()*g.h();

	int i,j;

	unsigned char *gMat[3];
	for(i=0;i<3;i++)
		gMat[i] = new unsigned char[no_pixel];
	
	g.copyTo(gMat[0],gMat[1],gMat[2]);
	
	Hist1d hist(65536,-1,256);
	hist.addtohist(gMat[0],no_pixel);
	
	int *hist1 = hist.gets();
	int histmax = hist.getMax();
	int histMean = (int)hist.getMean();

/*	float **histogram = get_histogramGray(m_pImage);
	float histmax = -1;
	int i,j;

	for(i=0;i<256;i++)
		histmax = __max(histmax,histogram[0][i]);
		*/

//	int i,j;
	
	CmyImage image = CImgMisc::MakeImg(266,138*3,8);
	CPixelPtr pDest(image);

	float VPmax;
	for (i=4;i<260;i++)
	{
		VPmax = (float)ceil(((float)hist1[i-4]/(float)histmax)*127);
		for(j=132;j>132-(int)VPmax;j--)
			pDest[j][i] = 128;

		if (i-4 == histMean)
			for(j=0;j<138;j++)
				pDest[j][i] = 128;

		pDest[132][i] = 255;
	}

	for(i=132;i>4;i--)
		pDest[i][4] = 255;

	CImgMisc::ShowImg(image);

	for(i=0;i<3;i++)
		delete [] gMat[i];

	CString str;
	str.Format("Max = %d, Mean = %f", histmax, histMean);
	AfxMessageBox(str);
}

float **CImageGeometry::get_histogramGray(CmyImage *imageP)
{
   	register BYTE *bP;
   	register int i,j,k;
   	int bands, rows,cols;
	unsigned levels=0;
	float **histogram;
   
   	bands = 1;//getNoOfBands_Image(imageP);
   	rows =  imageP->GetHeight();//getNoOfRows_Image(imageP);
   	cols =  imageP->GetWidth();//getNoOfCols_Image(imageP);

	CPixelPtr pDest(*imageP);

	histogram=(float**)calloc((bands+1),sizeof(float*));
	if(!histogram)
		{
		printf("memory allocation error for float **histogram\n");
		return (float **)NULL;
		}
	for(i=0;i<bands;i++)
		{
		histogram[i]=(float*)calloc(256,sizeof(float));
		if(!histogram[i])
			{
			printf("Insufficient memory for histogram[%d]\n",i);
		return (float **)NULL;
			}
		}
	for(i=0;i<bands;i++)
		for(k=0;k<256;k++)
			histogram[i][k]=0.0;
   	for(i=0; i < bands; i++)
      		for(j=0; j < rows; j++)
			{
         		//bP=(BYTE *)pDest[j];//getRow_Image(imageP, j, i);
         		for(k=0; k < cols; k++, bP++)
					histogram[i][pDest[j][k]]++;
			}
	for(i=0;i<bands;i++)
		for(k=0;k<256;k++)
			histogram[i][k]/=(float)(rows*cols);
#ifdef DEBUG
	printf("  r  histogram \n");			
	printf("---------------\n");			
	for(i=0;i<256;i++)				
		printf("%4d %8d\n",i,histogram[0][i]);
#endif	
	return histogram;
}

float **CImageGeometry::get_histogramColor(CmyImage *imageP)
{
   	register BYTE *bP;
   	register int i,j,k;
	int bands, rows, cols;
	unsigned levels=0;
	float **histogram;
   
   	bands = 3;//getNoOfBands_Image(imageP);
   	rows =  imageP->GetHeight();//getNoOfRows_Image(imageP);
   	cols =  imageP->GetWidth();//getNoOfCols_Image(imageP);

	CColorPixelPtr pDest(*imageP);

	histogram=(float**)calloc((bands+1),sizeof(float*));
	if(!histogram)
		{
		printf("memory allocation error for float **histogram\n");
		return (float **)NULL;
		}
	for(i=0;i<bands;i++)
		{
		histogram[i]=(float*)calloc(256,sizeof(float));
		if(!histogram[i])
			{
			printf("Insufficient memory for histogram[%d]\n",i);
		return (float **)NULL;
			}
		}
	for(i=0;i<bands;i++)
		for(k=0;k<256;k++)
			histogram[i][k]=0.0;
      	
		for(j=0; j < rows; j++)
		{
			//bP=(BYTE *)pDest[i][j];//getRow_Image(imageP, j, i);
			for(k=0; k < cols; k++, bP++)
			{
				histogram[0][pDest[j][k].R]++;
				histogram[1][pDest[j][k].G]++;
				histogram[2][pDest[j][k].B]++;
			}
		}
	for(i=0;i<bands;i++)
		for(k=0;k<256;k++)
			histogram[i][k]/=(float)(rows*cols);

		return histogram;
}

void CImageGeometry::Feature()
{
	int n = 256, m;
	GrayImage I1(n,n);	
	Moment	i1mt(&I1);

//	setCircle(&I1, 150, 150, 50.1);
	I1.save("circle.rgb");
/*
	unsigned char *p = I1.buf[0];
	for (int y = 0; y < n; y++)
		for (int x = 0; x < n; x++)
			*p++ = 255;
*/

//	I1.printx("stdout", 0, I1.height, I1.width);
	printf ("Image Moment:\n");
	printf ("normalized center = (%4.2f, %4.2f)", i1mt.getCenterX(), i1mt.getCenterY()); 
	printf (" = (%d, %d)\n", (int)(i1mt.getCenterX() * I1.width), (int)(i1mt.getCenterY() * I1.height)); 
	printf ("eigenvalues = (%4.2f, %4.2f)\n", i1mt.getEigenValues()[0], i1mt.getEigenValues()[1]); 
	printf ("orientation = %4.2f deg(%4.2f rad)\n", RAD2DEG(i1mt.getOriAngle()), i1mt.getOriAngle()); 
	printf ("aspectratio = %4.2f\n", i1mt.getAspectRatio()); 
	printf ("tan 2o = %4.2f\n", i1mt.getTan2O()); 

	printf ("Moment= \n");
	for (n = 0; n < 3; n++) {
		for (m = 0; m < 3; m++)
			printf ("m%d%d = %4.2f\n", m, n, i1mt.get(m,n));
	}

	printf ("Central Moment= \n");
	for (n = 0; n < 3; n++) {
		for (m = 0; m < 3; m++)
			printf ("u%d%d = %4.2f\n", m, n, i1mt.getC(m,n));
	}

/*
	printf ("%f %f\n", i1mt.getC(3,0), i1mt.get(3,0) - 3.0 *
		i1mt.getCenterY() * i1mt.get(2,0) + 2.0 * sqr(i1mt.getCenterY()) *
		i1mt.get(1,0));
*/
	printf ("%f %f\n", i1mt.getC(2,1), 
		i1mt.get(2,1) - 2.0 * i1mt.getUCenterY() * i1mt.get(1,1) 
		- i1mt.getUCenterX() * i1mt.get(2,0) 
		+ 2.0 * sqr(i1mt.getUCenterY()) * i1mt.get(0,1));
	printf ("moment invariant features:\n");
	for (n = 0; n < 7; n++) 
		printf ("h[%d] = %10.9e\n", n, i1mt.getInvariantFeature(n, n == 0 ? 1:0));
}


// Hough transform //
void CImageGeometry::hough(char *str, BYTE th)
{
	CmyImage houghImg = hough_transform(m_pImage,parse_string(str),180);
	CImgMisc::ShowImg(houghImg);

	CmyImage invImg = hough_inverse(&houghImg,m_pImage->GetHeight(),m_pImage->GetHeight(),th);
	CImgMisc::ShowImg(invImg);
}

/*only pixels of 255 will be scanned, here I mean binary image is composed of
  255 and 0, not other definition*/
CmyImage CImageGeometry::hough_transform(CmyImage* cvipImage,char *interpret,int num)
{
    long *image_o;

    double angle;
    int x, y,i,j, bands, rows, cols, arm_length;
    float arm;

    bands = cvipImage->GetBitCount()/8;   
    rows =  cvipImage->GetHeight();   
    cols =  cvipImage->GetWidth();
    
	arm = (float)sqrt((double)rows*rows+cols*cols);
    arm_length = (int)(arm + 0.5);

	CmyImage out1Image = CImgMisc::MakeImg(180, arm_length,bands*8);

	CPixelPtr image_i(*cvipImage);
	image_o = new long[180*arm_length];
	
	for(y=0;y<rows;y++){
	    for(x=0;x<cols;x++){
	    	if(image_i[y][x] == 255)
			{
				for(i=0;i<180;i++)
				{
					if(i<num && interpret[i] == 0)
						continue;
					angle = (double)i*M_PI/180;
					j = (int)((x*cos(angle) + y*sin(angle) + arm)/2 +0.5);
					if(j>=0 && j < arm_length)
						image_o[j*180+i]++;
				}
			}
		}
	}
    
	CPixelPtr image_t(out1Image);

	for(y=0;y<arm_length;y++)
	{
		for(x=0;x<180;x++)
		{
			if(image_o[y*180+x] > 255)
				image_t[y][x] = 255;
			else
				image_t[y][x] = (unsigned char)(image_o[y*180+x]);
				//image_t[y][x] = 0;
		}
	}

	delete [] image_o;
    return out1Image;
}

char *CImageGeometry::parse_string(char *input){
    int count,before,decision = 0,lower,upper;
    char *output;
    char *ptr,temp[10],original[180];

    output = (char*)calloc(180,sizeof(char));
    memset(original,0,sizeof(original));
    ptr = input;
    while(*ptr ==' ' || *ptr == '\t')ptr++;
    if(*ptr == '\n' || *ptr == NULL){
	memset(output,1,180*sizeof(char));
	return output;
    }
    while(*ptr != NULL){
	before = -1;
	if(*ptr == '-'){
	    before = decision;
	    ptr++;
	}
	while(*ptr ==' ' || *ptr == '\t')ptr++;
	count = 0;
	while(isdigit(*ptr))
	    temp[count++] = *ptr++;
	temp[count] = NULL;
  	if(count != 0){
	    sscanf(temp,"%d",&decision);
	    if(decision <= 179){
		if(before == -1)
		    original[decision] = 1;
		else{
		    lower = (before < decision)?before:decision;
		    upper = (before > decision)?before:decision;
		    memset(&original[lower],1,(upper-lower+1) 
				*sizeof(char));
		    before = -1;
		}
	    }
	    else{
		 decision = 179;
		 original[decision] = 1;
	    }
	}
        while(*ptr ==' ' || *ptr == '\t')ptr++;
	if(*ptr == NULL)
	    break;
    }
    if(before != -1)
	memset(&original[before],1,(180 - before)*sizeof(char));
    memcpy(output,&original[90],90*sizeof(char));
    memcpy(&output[90],original,90*sizeof(char));
    return output;
}

CmyImage CImageGeometry::hough_inverse(CmyImage *cvipImage,int height, int width,int threshold)
{
    double  angle,arm,r_cos,r_sin,test_c,test_s;
    int x, y, bands, rows,cols;
	int i,j;
 
    bands = cvipImage->GetBitCount()/8;   
    rows =  cvipImage->GetHeight();   
    cols =  cvipImage->GetWidth();

	CmyImage outImage = CImgMisc::MakeImg(width,height,bands*8);

    CPixelPtr image_i(*cvipImage);
	CPixelPtr image_o(outImage);

   	for(y=0;y<2*rows;y++)
	{
	    for(x=0;x<cols;x++)
		{
			if(image_i[y/2][x] >=(BYTE) threshold)
			{
				angle = (double)x*M_PI/180;
				arm = y - rows ;
				r_cos = cos(angle);
				r_sin = sin(angle);
				test_c = (r_cos > 0)? r_cos : -1*r_cos;
				test_s = (r_sin > 0)? r_sin : -1*r_sin;
				if(test_c < test_s)
				{
					for(i=0;i<width;i++)
					{
						j = (int)((arm - i*r_cos)/r_sin + 0.5);
						if(j>=0 && j < height)
							image_o[j][i] = 255;
					}
				}
				else
				{
					for(j=0;j<height;j++)
					{
						i = (int)((arm - j*r_sin)/r_cos + 0.5);
						if(i>=0 && i < width)
							image_o[j][i] = 255;
					}				
				}
			}
		}
	}

    return outImage;
}

/*
Image *hough_filter(Image *cvipImage, char *name, char *degree_string, 
	int threshold, int connection, int interactive){
    Image *outImage, *tempImage,*tempImage1,*houghImage;
    char temp_name[100], direct[150],specification[300], *interpret;
    byte **image_i, **image_o,**image_t;
    int x, y, bands, rows,cols,no_of_bands,i,j,arm_length,flag;
    float  r_cos,r_sin, test_c, test_s,angle,arm;
    int u_x, u_y, d_x, d_y,r_i,r_j;

    bands = getNoOfBands_Image(cvipImage);   
    rows =  getNoOfRows_Image(cvipImage);   
    cols =  getNoOfCols_Image(cvipImage);
    if(cvipImage->bandP[0]->data_type != NULL){
	tempImage = remap_Image(cvipImage,CVIP_BYTE,0,255);
	delete_Image(cvipImage);
	cvipImage = tempImage;
    	fprintf(stderr,"Input Image is remapped into CVIP_BYTE\n");
    }
    if(interactive){
	fflush(stdin);
	fprintf(stderr,"\n\nNow please specify the orientation which you are interested in x-y coordinates( the same as that of your input image).");
	fprintf(stderr,"The range is from 0 to 179 in degrees\n");
	fprintf(stderr,"\tAn example like \"-18 40-60 90 130-\", which means keep  those lines whose slope are in the range from 0 to 18, 40 to 60, 90, 130 to 179 degrees\n");
        gets(specification);
        interpret = parse_string(specification);
    }
    else{
	interpret = parse_string(degree_string);
    }
    tempImage = (Image*)duplicate_Image(cvipImage);
    cvipImage = hough_transform(cvipImage,interpret,180);
    free(interpret);
    if(*name != NULL)
	strcpy(temp_name,name);
    else
        strcpy(temp_name,"UNKNOWN");
    strcat(temp_name,"-HOUGH");
    fprintf(stderr,"Displaying  Hough transform image...\n");

    view_Image(cvipImage,temp_name);

    if(interactive){
	fflush(stdin);
	fprintf(stderr,"Enter the value for minimum number of pixels per line:");
	scanf("%d",&threshold);
	fprintf(stderr,"Enter the maximum connection distance(in pixels):");
	scanf("%d",&connection);
    }
    houghImage = duplicate_Image(cvipImage);
    cvipImage = hough_inverse(cvipImage,rows,cols,threshold);
    tempImage1 = duplicate_Image(cvipImage); 
    for(no_of_bands=0;no_of_bands<bands;no_of_bands++){
	image_i = getData_Image(cvipImage,no_of_bands);
        image_o = getData_Image(tempImage,no_of_bands);
	image_t = getData_Image(tempImage1,no_of_bands);
    	for(y=0;y<rows;y++){
	    for(x=0;x<cols;x++){
		image_t[y][x] = image_i[y][x] & image_o[y][x];
	    }
	}
    }
    delete_Image(cvipImage);
    delete_Image(tempImage);
    tempImage = duplicate_Image(tempImage1);
    arm_length = houghImage->bandP[0]->rows;
    for(no_of_bands=0;no_of_bands<bands;no_of_bands++){
	image_i = getData_Image(houghImage,no_of_bands);
	image_o = getData_Image(tempImage1,no_of_bands);
	image_t = getData_Image(tempImage,no_of_bands);
	for(y=0;y<rows;y++){
	    for(x=0;x<cols;x++){
		if(image_o[y][x] == 255){
		    int high_degree = houghImage->bandP[0]->cols;
		    u_x = x - connection;
		    u_y = y - connection;
		    d_x = x + connection;
		    d_y = y + connection;
		    u_x =   (u_x >= 0) ? u_x : 0;
		    u_y =   (u_y >= 0) ? u_y : 0;
		    d_x = (d_x < cols) ? d_x : cols-1;
		    d_y = (d_y < rows) ? d_y : rows-1;
		    for(i=0;i<high_degree;i++){
			angle = (double)i*PI/180;
			j = (x*cos(angle) + y*sin(angle) + arm_length)/2 + 0.5;
			if(j>=0&&j<arm_length&&image_i[j][i]>=(byte)threshold){
			    r_cos = cos(angle);
			    r_sin = sin(angle);
			    arm = x*r_cos + y*r_sin;
			    test_c = (r_cos > 0)? r_cos : -1*r_cos;
			    test_s = (r_sin > 0)? r_sin : -1*r_sin;
			    flag = 0;
			    if(test_c < test_s){
			        for(r_i=u_x;r_i<= x;r_i++){
				    r_j = (arm - r_i*r_cos)/r_sin + 0.5;
				    if(r_j>=u_y && r_j<=d_y ){
					if(flag == 1){
					    image_t[r_j][r_i] = 255;
					}
					else if(image_t[r_j][r_i] == 255 ||
						image_o[r_j][r_i] == 255){
					    flag = 1;
					}
				    }
			        }
				flag = 0;
			        for(r_i=d_x;r_i>=x;r_i--){
				    r_j = (arm - r_i*r_cos)/r_sin + 0.5;
				    if(r_j>=u_y && r_j<=d_y ){
					if(flag == 1){
					    image_t[r_j][r_i] = 255;
					}
					else if(image_t[r_j][r_i] == 255 ||
						image_o[r_j][r_i] == 255){
					    flag = 1;
					}
				    } 
			        }
			    }
			    else{
			        for(r_j=u_y;r_j<= y;r_j++){
				    r_i = (arm - r_j*r_sin)/r_cos + 0.5;
				    if(r_i>=u_x && r_i<=d_x ){
					if(flag == 1){
					    image_t[r_j][r_i] = 255;
					}
					else if(image_t[r_j][r_i] == 255 ||
						image_o[r_j][r_i] == 255){
					    flag = 1;
					}
				    } 
			        }
				flag = 0;
			        for(r_j=d_y;r_j>=y;r_j--){
				    r_i = (arm - r_j*r_sin)/r_cos + 0.5;
				    if(r_i>=u_x && r_i<=d_x ){
					if(flag == 1){
					    image_t[r_j][r_i] = 255;
					}
					else if(image_t[r_j][r_i] == 255 ||
						image_o[r_j][r_i] == 255){
					    flag = 1;
					}
				    } 
			        }
			    }
			}
		    }

		}
	    }
	}
    }
    delete_Image(houghImage);
    delete_Image(tempImage1);
    
    return tempImage;
}	    
      
			
Image* edge_link_filter(Image *cvipImage, int connection){
    Image *outImage;
    byte  **image_i, **image_o;
    int x,y,rows,cols,no_of_bands,bands,i,j,u_x,u_y,d_x,d_y;
    int y_inc,x_inc,x_abs,y_abs,r_i,r_j,direction;
    float slope;

    bands = getNoOfBands_Image(cvipImage);   
    rows =  getNoOfRows_Image(cvipImage);   
    cols =  getNoOfCols_Image(cvipImage);
    if(cvipImage->bandP[0]->data_type != CVIP_BYTE){
	fprintf(stderr,"Image will be remapped into CVIP_BYTE\n");
	outImage = remap_Image(cvipImage,CVIP_BYTE,0,255);
	delete_Image(cvipImage);
	cvipImage = duplicate_Image(outImage);
    }
    else
    	outImage = duplicate_Image(cvipImage);
    for(no_of_bands=0;no_of_bands<bands;no_of_bands++){
	image_i = getData_Image(cvipImage,no_of_bands);
	image_o = getData_Image(outImage,no_of_bands);
	for(y=0;y<rows;y++){
	    for(x=0;x<cols;x++){
		if(image_o[y][x] == 255){
		    u_x = x - connection;
		    u_y = y - connection;
		    d_x = x + connection;
		    d_y = y + connection;
		    u_x =   (u_x >= 0) ? u_x : 0;
		    u_y =   (u_y >= 0) ? u_y : 0;
		    d_x = (d_x < cols) ? d_x : cols-1;
		    d_y = (d_y < rows) ? d_y : rows-1;
		    for(j=u_y;j<=d_y;j++){
			for(i=u_x;i<=d_x;i++){
			    if(image_i[j][i] == 255 && !(i==x && j==y)){
				x_inc = x - i;
				y_inc = y - j;
				x_abs = abs(x_inc);
				y_abs = abs(y_inc);
				if(x_abs > y_abs){
				    direction = (x_inc > 0)? 1 : -1;
				    slope = (float)y_inc/x_inc;
				    for(r_i=i;r_i!=x;r_i+=direction){
					r_j=slope*(r_i-i)+j+0.5;
					image_o[r_j][r_i] = 255;
				    }
				}
				else{
				    direction = (y_inc > 0)? 1 : -1;
				    slope = (float)x_inc/y_inc;
				    for(r_j=j;r_j!=y;r_j+=direction){
					r_i=slope*(r_j-j)+i+0.5;
					image_o[r_j][r_i] = 255;
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    delete_Image(cvipImage);
    return outImage;
}
			
Image* link_Setup(Image *cvipImage){
    int length;
    fflush(stdin);
    printf("Enter the maximum linking distance:");
    scanf("%d",&length);
    return edge_link_filter(cvipImage, length);
}
*/

// weighted projection
// 07.11.01
// dir: 0(X), 1(Y), 2(Xw), 3(Yw)
long *CImageGeometry::weighted_projection(GrayImage A, int sizex, int sizey, int dir)
{
	int size, i, j;

	if (dir%2 == 0)
		size = sizex;
	else
		size = sizey;

	long *proj = new long[size];
	long sum;

	switch (dir)
	{
	case 0:
		proj = (long *)A.proj(GrayImage::X, 0, 0, A.w(), A.h());
		break;
	case 1:
		proj = (long *)A.proj(GrayImage::Y, 0, 0, A.w(), A.h());
		break;
	case 2:
		for (i=0; i<size; i++)
		{
			sum = 0;
			for (j=0; j<A.h(); j++)
				sum += A.getPixel(i, j) * (j*j+1);

			proj[i] = sum;
		}
		break;
	case 3:
		for (i=0; i<size; i++)
		{
			sum = 0;
			for (j=0; j<A.w(); j++)
				sum += A.getPixel(j, i) * (j*j+1);

			proj[i] = sum;
		}
		break;
	default:
		break;
	}

	return proj;
}

void CImageGeometry::projection(BOOL xdir, BOOL ydir, BOOL weight)
{
	ASSERT(m_pImage);
	ASSERT(m_pImage->GetHandle() != NULL);
	ASSERT(m_pImage->GetUndoHandle() != NULL);

	// error handling
	if ((m_pImage->GetBitCount() != 8) || ((xdir == 0) && (ydir == 0)))
		return;

	GrayImage A = CImgMisc::ImgConvc_G(*m_pImage);
	GrayImage B(A.w() + MY_PROJ_TOP, A.h() + MY_PROJ_TOP), *C; 

	// only X
	if ((xdir == 1) && (ydir == 0))
		C = B.getSubImage(0, 0, B.w() - MY_PROJ_TOP, B.h());
	// only Y
	else if ((xdir == 0) && (ydir == 1))
		C = B.getSubImage(0, 0, B.w(), B.h() - MY_PROJ_TOP);
	// X & Y
	else
		C = B.getSubImage(0, 0, B.w(), B.h());

	int i, j;
	for (j=0; j<A.h(); j++)
		for (i=0; i<A.w(); i++)
			C->setPixel(i, j, A.getPixel(i, j));

	long *proj, max;

	// x direction
	if (xdir == 1)
	{
		if (weight == 0)
			proj = weighted_projection(A, A.w(), A.h(), 0);
		else
			proj = weighted_projection(A, A.w(), A.h(), 2);

		max = -1;
		for (i=0; i<A.w(); i++)
			if (proj[i] > max)
				max = proj[i];

		for (i=0; i<A.w(); i++)
			for (j=0; j<MY_PROJ_TOP; j++)
			{
				if (j <= (int)(MY_PROJ_TOP*((float)proj[i]/(float)max))) 
					C->setPixel(i, j + A.h(), 255);
				else
					C->setPixel(i, j + A.h(), 0);
			}

		delete []proj;
	}

	// y direction
	if (ydir == 1)
	{
		if (weight == 0)
			proj = weighted_projection(A, A.w(), A.h(), 1);
		else
			proj = weighted_projection(A, A.w(), A.h(), 3);

		max = -1;
		for (i=0; i<A.h(); i++)
			if (proj[i] > max)
				max = proj[i];

		for (j=0; j<A.h(); j++)
			for (i=0; i<MY_PROJ_TOP; i++)
			{
				if (i <= (int)(MY_PROJ_TOP*((float)proj[j]/(float)max)))  
					C->setPixel(i + A.w(), j, 255);
				else
					C->setPixel(i + A.w(), j, 0);
			}


		delete []proj;
	}

	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(C));
}