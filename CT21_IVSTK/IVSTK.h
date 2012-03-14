// IVSTK.h: interface for the CCT21IVSTK class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IVSTK_H__6EA07B95_1EBC_4FA4_B429_2C943491FCAD__INCLUDED_)
#define AFX_IVSTK_H__6EA07B95_1EBC_4FA4_B429_2C943491FCAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define HAVE_IPL
#include <ipl.h>
#include <highgui.h>
#include <cv.h>

#include "ImageBaseView.h"
#include "LogView.h"

#include "myFNN.h"
#include "FMMNN.h"
#include "NNfunction.h"

#define	CE21	(2147483648)
#define NUM_WAVE	(4)
#define NUM_ANGLE	(4)

///////////////////////////////////////////////////////////////////////
// 20030501
class CIVSTKsegment
{
public:
	int start;
	int end;
	int length;
	long sum;
	float gauss;

	CIVSTKsegment() { start = end = length = 0; sum = 0; gauss = 0; }
};

class CIVSTKfacetemplate
{
public:
	int x;
	int y;
	int dl;
	int dr;
	int dd;
	float au;
	float as;
	int tu;
	int td;
	int dist;

	CIVSTKfacetemplate() {x = y = dl = dr = dd = tu = td = 0; au = as = 0.0f; dist = 4; };
	CIVSTKfacetemplate(int xv, int yv, int dlv, int drv, int ddv, int tuv, int tdv, float auv, float asv) 
	{x = xv; y = yv; dl = dlv; dr = drv; dd = ddv; tu = tuv, td = tdv; au = auv; as = asv; dist = 4; };

	void iplDTMset(int xv, int yv, int dlv, int drv, int ddv, int tuv, int tdv, float auv, float asv) 
	{x = xv; y = yv; dl = dlv; dr = drv; dd = ddv; tu = tuv, td = tdv; au = auv; as = asv; dist = 4; };
	//void iplCorrEdge(IplImage *img, float *corr);
	//void iplDTMshow(IplImage *img);
};



class CIVSTKsubcomp
{
public:
	CvRect rect;
	float feature;

	CvRect GetRect() {return (CvRect)rect; }
	int t() {return rect.y; }
	int b() {return rect.y + rect.height; }
	int r() {return rect.x + rect.width; }
	int l() {return rect.x; }
	CvPoint tl() {return (cvPoint(l(), t())); }
	CvPoint tr() {return (cvPoint(r(), t())); }
	CvPoint bl() {return (cvPoint(l(), b())); }
	CvPoint br() {return (cvPoint(r(), b())); }

	int GetArea() {return (int)(rect.width*rect.height); }
	float GetFeature() {return (float)feature; }
	
	inline void SetRect(CvPoint pt1, CvPoint pt2) 
	{rect.x = pt1.x; rect.y = pt1.y; rect.width = pt2.x - pt1.x; rect.height = pt2.y - pt1.y; }
	int GetCenterX() {return (int)(rect.x + rect.width/2); }
	int GetCenterY() {return (int)(rect.y + rect.height/2); }
	CvPoint GetCenter() {return (cvPoint(GetCenterX(), GetCenterY())); }
	int GetCDx() {return rect.width/2; }
	int GetCDy() {return rect.height/2; }
	int ValidRect() {return ((rect.width > 0) && (rect.height > 0) && (rect.x > 0) && (rect.y > 0) && (rect.x < 160) && (rect.y < 120)); }

	CIVSTKsubcomp() { rect.x = rect.y = rect.height = rect.width = 0; feature = 0.0f; }
};

class CIVSTKface
{
public:
	int emotions;

	CIVSTKsubcomp face;

	CIVSTKsubcomp left_eyebrow;
	CIVSTKsubcomp right_eyebrow;
	CIVSTKsubcomp left_eye;
	CIVSTKsubcomp right_eye;
	CIVSTKsubcomp nose;
	CIVSTKsubcomp mouth;
	
	CIVSTKsubcomp nasolabial_root;
	CIVSTKsubcomp left_crow_feet;
	CIVSTKsubcomp right_crow_feet;
	CIVSTKsubcomp left_eye_feet;
	CIVSTKsubcomp right_eye_feet;
	CIVSTKsubcomp left_nasolabial_fold;
	CIVSTKsubcomp right_nasolabial_fold;

	IplImage* mask;

	inline void EyeBrowFeat() 
	{ 
		left_eyebrow.feature = (float)(left_eye.GetCenterY() - left_eyebrow.GetCenterY());
		right_eyebrow.feature = (float)(right_eye.GetCenterY() - right_eyebrow.GetCenterY());
	}

	// global feature!
	inline void EyeOpennessG()
	{
		left_eye.feature = ((float)left_eye.rect.height/(float)face.rect.height);
		right_eye.feature = ((float)right_eye.rect.height/(float)face.rect.height);
	}

	inline void MouthOpennessG1() 
	{
		mouth.feature = ((float)mouth.rect.height/(float)face.rect.height);
	}

	inline void MouthOpennessG2() 
	{
		mouth.feature = (((float)mouth.rect.height*(float)mouth.rect.width)/((float)face.rect.width*(float)face.rect.height));
	}

	// functions
	void SetTransient() { 
		nasolabial_root.SetRect(cvPoint(__max(left_eyebrow.r(), left_eye.r()), left_eyebrow.t()),
			cvPoint(__min(right_eyebrow.l(), right_eye.l()), right_eye.b()));
		left_crow_feet.SetRect(cvPoint(left_eye.l() - left_eye.GetCDx(), left_eye.t()),
			cvPoint(left_eye.l(), left_eye.b()));
		right_crow_feet.SetRect(cvPoint(right_eye.r(), right_eye.t()),
			cvPoint(right_eye.r() + right_eye.GetCDx(), right_eye.b()));
		left_eye_feet.SetRect(cvPoint(left_eye.l(), left_eye.b()),
			cvPoint(left_eye.r(), left_eye.b() + left_eye.GetCDy()));
		right_eye_feet.SetRect(cvPoint(right_eye.l(), right_eye.b()),
			cvPoint(right_eye.r(), right_eye.b() + right_eye.GetCDy()));
		left_nasolabial_fold.SetRect(cvPoint(left_eye.l(), nose.t()),
			cvPoint(nose.l(), mouth.t()));
		right_nasolabial_fold.SetRect(cvPoint(nose.r(), nose.t()),
			cvPoint(right_eye.r(), mouth.t()));
	}

	void save(const char* str);
	void load(const char* str);
	void savefeature(const char* str);
	void draw(IPLIMAGE src);
};
///////////////////////////////////////////////////////////////////////

class CIVSTK  
{
public:
	CIVSTK();
	virtual ~CIVSTK();

	enum FeatureIndex{NLF, HEDGE, ADTH, CANNY, ICOPY, NLR, CNTNZ, EYEOPENLP, EYEOPENG, GG};
	enum ThresholdIndex{IB, OB, GT, LT, OTSU_H, OTSU_L};

	bool init_flag;

	CImageBaseView* imgview;
	CLogView* msgview;
	CImageBaseView* debug[4];

	IPLIMAGE colimg[10];
	IPLIMAGE colLPimg[10];
	IPLIMAGE grayimg[10];
	IPLIMAGE grayLPimg[10];
	IPLIMAGE floatimg[10];
	IPLIMAGE longimg[10];
	IPLIMAGE binimg[10];

	IPLIMAGE xmap;
	IPLIMAGE ymap;
	IPLIMAGE xmap2;
	IPLIMAGE ymap2;

	CvSize CPsize;
	CvSize LPsize;
	CvMat projX;
	CvMat projY;
	CvMat projXLP;
	CvMat projYLP;
	
	IplLUT *plut;
	IplLUT lut;
	IplROI *roi;
	IplColorTwist *mytwist, *twist_I1I2I3, *twist_I1I2I3N;
	CvHistogram *histG;
	IplConvKernelFP	*kernel[NUM_WAVE*NUM_ANGLE][2];
	CvFont font;
	CvMemStorage *storage;
	CvSeq *contMayor;

	CIVSTKface m_face;

	myFNN m_fnn;
	CFMMNN m_fmmnn;
	CNNfunction m_nn;

	void IVSTKinit(IPLIMAGE img);
	void IVSTKend();

	void IVSTKcolorconvert(IPLIMAGE src, IPLIMAGE dst, CString str);
	void IVSTKfiltering(IPLIMAGE src, IPLIMAGE dst, CString str, int minmax[2]);
	void IVSTKfourier(IPLIMAGE src, IPLIMAGE dst, CString str);
	void IVSTKtempmatch(IPLIMAGE src, IPLIMAGE dst, CString str, CString part, int degree);
	void IVSTKstatfeat(IPLIMAGE src, IPLIMAGE dst, CString str);
	void IVSTKblobanal(IPLIMAGE src, IPLIMAGE dst, CString str, CString morph, int num = 1);
	void IVSTKprojext(IPLIMAGE src, IPLIMAGE dst, CString str);
	void IVSTKgeofeat(IPLIMAGE src, IPLIMAGE dst, CString str, CString str2, CIVSTKface face);
	void IVSTKcoordtx(IPLIMAGE src, IPLIMAGE dst, CString str);
	void IVSTKalu(IPLIMAGE src, IPLIMAGE dst, CString str, BOOL flag[3], float Const);

	void IVSTKthreshold(IPLIMAGE src, IPLIMAGE dst, CString str, int th1, int th2);
	void IVSTKthreshold2(IPLIMAGE Simg, IPLIMAGE Dimg, int low, int high, ThresholdIndex method, float otsu);
	void IVSTKotsu(IPLIMAGE Simg, int *th0, int start);

	void IVSTKproject2(IPLIMAGE Simg, CvMat proj, int dir, int method = 0);
	void IVSTKproject(IplImage *Simg, IplImage *Dimg, int xdir, int ydir, int method);
	void IVSTKhistogram(IplImage *Simg, IplImage *Dimg, int offset);
	void IVSTKnormalize(IplImage *Simg, IplImage *Dimg);

	void IVSTKsubfeatext(IPLIMAGE Simg, CIVSTKsubcomp *comp, FeatureIndex method);
	float IVSTKgg(IPLIMAGE A);
	void IVSTKsubsearch(CvMat proj, CvPoint range, int mode, CIVSTKface& facecomp);
	void IVSTKcompext(IPLIMAGE A, CvMat proj, CvPoint range, CIVSTKface& facecomp);

	void IVSTKrecNN(CString str, float param[]);
	void IVSTKrecFNN(CString str, float param[]);
	void IVSTKrecFMMNN(CString str, float param[]);

	void IVSTKeigenobject(IPLIMAGE src, IPLIMAGE dst, CString str, float param[]);

	void IVSTKmotion(IPLIMAGE left, IPLIMAGE right, CString str);
	void IVSTKdepth(IPLIMAGE left, IPLIMAGE right, CString str);

	void IVSTKdrawflow(IPLIMAGE velx, IPLIMAGE vely, IPLIMAGE vel, int gridsize = 4);

	//void IVSTKfacefeatext(IPLIMAGE Simg, CIVSTKface *face);
};

#endif // !defined(AFX_CT21IVSTK_H__6EA07B95_1EBC_4FA4_B429_2C943491FCAD__INCLUDED_)
