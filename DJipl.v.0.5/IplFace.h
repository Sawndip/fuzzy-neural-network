// IplFace.h: interface for the CIplFace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPLFACE_H__87A9C804_A64B_4C49_A481_83F96421AEFA__INCLUDED_)
#define AFX_IPLFACE_H__87A9C804_A64B_4C49_A481_83F96421AEFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cv.h"
#include "ipl.h"
#include "myresource.h"

class iplFaceTemplate
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

	iplFaceTemplate() {x = y = dl = dr = dd = tu = td = 0; au = as = 0.0f; dist = 4; };
	iplFaceTemplate(int xv, int yv, int dlv, int drv, int ddv, int tuv, int tdv, float auv, float asv) 
	{x = xv; y = yv; dl = dlv; dr = drv; dd = ddv; tu = tuv, td = tdv; au = auv; as = asv; dist = 4; };

	void iplDTMset(int xv, int yv, int dlv, int drv, int ddv, int tuv, int tdv, float auv, float asv) 
	{x = xv; y = yv; dl = dlv; dr = drv; dd = ddv; tu = tuv, td = tdv; au = auv; as = asv; dist = 4; };
	void iplCorrEdge(IplImage *img, float *corr);
	void iplDTMshow(IplImage *img);
};



class iplFaceSubComponent
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
};

class iplFaceStructure
{
public:
	int emotions;

	iplFaceSubComponent face;

	iplFaceSubComponent left_eyebrow;
	iplFaceSubComponent right_eyebrow;
	iplFaceSubComponent left_eye;
	iplFaceSubComponent right_eye;
	iplFaceSubComponent nose;
	iplFaceSubComponent mouth;
	
	iplFaceSubComponent nasolabial_root;
	iplFaceSubComponent left_crow_feet;
	iplFaceSubComponent right_crow_feet;
	iplFaceSubComponent left_eye_feet;
	iplFaceSubComponent right_eye_feet;
	iplFaceSubComponent left_nasolabial_fold;
	iplFaceSubComponent right_nasolabial_fold;

	IplImage *mask;

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

	void SaveFeatures(const char *str);
};


class CIplFace  
{
public:
	CIplFace();
	virtual ~CIplFace();

	// My functions
	// 2002.2.19

	// processing
	void iplFaceLocate(IplImage *Simg, iplFaceStructure *face, IplImage *Dimg);
	void iplFaceComponent(IplImage *Simg, iplFaceStructure *face, IplImage *Dimg);
	void iplFaceFeatureExtract(IplImage *Simg, iplFaceStructure *face);
	void iplFaceExpression(IplImage *Simg, iplFaceStructure *face);

	void iplBlobMaxArea(IplImage *Simg, CvMemStorage *storage, CvSeq *contMayor);
	void ShowHistogram(IplImage *Simg, IplImage *Dimg, int start = 0, int method = 0);
	void iplProject(IplImage *Simg, CvMat proj, int dir, int method = 0);
	void ShowProjection(IplImage *Simg, IplImage *Dimg, int xdir, int ydir, int method);
	void iplOtsu1(IplImage *Simg, int *th0, int start = 0);
	void iplThreshold2(IplImage *Simg, IplImage *Dimg, int low = 0, int high = 255, int method = 0, float otsu = 1.0);
	void ShowFace(IplImage *Simg, IplImage *Dimg);
	void ShowSubFace(IplImage *Simg, IplImage *Dimg, CvRect rect, int method=0);
	void GGfeature(IplImage *Simg, float wave, float theta, float *feature);
	void iplNormalize(IplImage *Simg, IplImage *Dimg);
	void iplFaceSubFeature(IplImage *Simg, iplFaceSubComponent *comp, int method=0);
	void iplLPM(IplImage *Simg, IplImage *Dimg);
	void iplLPMbulge(IplImage *Simg, IplImage *Dimg, float *bulge);
	void iplLPMangle(IplImage *Simg, IplImage *Dimg, float *angle);
	void iplFaceDrawing(IplImage *Simg, iplFaceStructure face);
	void iplFaceDTM(IplImage *Simg, IplImage *Dimg, iplFaceStructure *face);
	void iplFaceCHAT(IplImage *Simg, iplFaceStructure *face, int *th);

	// 2002.4.11
	void eyeFeature(IplImage *Simg, CvRect rect, float feature);
	void rootFeature(IplImage *Simg, CvRect rect, float feature);
	void nlfFeature(IplImage *Simg, CvRect rect, float feature);
	void mouthFeature(IplImage *Simg, CvRect rect, float feature);
	void disteyeFeature(IplImage *Simg, CvRect rect1, CvRect rect2, float feature);

	// initialization
	void iplFaceAllocImage(IplImage *Simg);
	void iplFaceFreeImage();

	// Learning
	void iplLTanal(int rindex, int gindex, char pstr[]);
	void grTruthChk(IplImage *grImg, IplImage *segImg, double &Correct, double &SkinError, double &NonSkinError, double &Skin);

	// My variables
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
	IplConvKernelFP	*kernel[NUM_FILTERS][2];
	CvFont font;
	IplImage *xmap;
	IplImage *ymap;
	IplImage *xmap2;
	IplImage *ymap2;
	IplImage *procCPimg;
	CvMemStorage *storage;
	CvSeq *contMayor;
	
	clock_t mystart, myfinish;
	double duration;

	// My images
	IplImage *procCPBimg[NUM_BUFFERS];
	IplImage *procCPGimg[NUM_BUFFERS];
	IplImage *procCPSimg[NUM_BUFFERS];
	IplImage *procCPFimg[NUM_BUFFERS];
	IplImage *procCPCimg[NUM_BUFFERS];
	IplImage *procCPCSimg[NUM_BUFFERS];
	IplImage *procLPGimg[NUM_BUFFERS];
	IplImage *procLPCimg[NUM_BUFFERS];
	IplImage *procCHATmask;
	
	// My etc.
	int CheckSize(IplImage *Simg) {return ((Simg->width == CPsize.width) && (Simg->height = CPsize.height)); }

};

#endif // !defined(AFX_IPLFACE_H__87A9C804_A64B_4C49_A481_83F96421AEFA__INCLUDED_)
