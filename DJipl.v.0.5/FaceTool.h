// FaceTool.h: interface for the CFaceTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACETOOL_H__DCAEA1E0_3366_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_FACETOOL_H__DCAEA1E0_3366_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cimage.h"
#include "morph.h"
//#include "VisMatrix.h"

// face components
// 20010614@22:01@DJ
//
//
// leyebrow		reyebrow
//   leye		  reye
//
//		   exnose
//
//		   exmouth
//
//
class CFaceComponent
{
public:
	CRect exFace;
	CRect rEyeBrow;
	CRect lEyeBrow;
	CRect rEye;
	CRect lEye;
	CRect exNose;
	CRect exNosex;
	CRect exMouth;

	int distEyeBrow() {return (rEyeBrow.left - lEyeBrow.right);}
	int distEye() {return (rEye.left - lEye.right);}
	int distEyeEyeBrowR() {return (rEye.top - rEyeBrow.bottom);}
	int distEyeEyeBrowL() {return (lEye.top - lEyeBrow.bottom);}
	//int distEyeEyeBrowOut() {return (rEyeBrow.left - lEyeBrow.right);}

	int distNoseMouth() {return (rEyeBrow.left - lEyeBrow.right);}
	int distMouthH() {return (exMouth.Height());}
	int distMouthV() {return (exMouth.Width());}
};

class CFaceSegment
{
public:
	int start;
	int end;
	int length;
	long sum;
	float gauss;

	CFaceSegment() { start = end = length = 0; sum = 0; gauss = 0; }
};

class CFaceTool  
{
public:
	// variables
	int faceTh[2][2];
	int w, h;
	
	Morphology morph;

	ColorImage *colorimage;
	GrayImage CHAT_mask;

	CFaceComponent	facecomp;

	// functions
	// 20010614@17:23@DJ
	CFaceTool();
	CFaceTool(ColorImage *colimage);
	virtual ~CFaceTool();
	// degree of eye-openness
	float DJdegree_of_eye_openness(ColorImage *A);
	// degree of mouth-openness
	float DJdegree_of_mouth_openness(ColorImage *A);
	// degree of each emotional expressions
	//CVisDVector DJdegree_of_facial_expressions(GrayImage A);
	void DJdegree_of_facial_expressions(GrayImage A, float* degree);


	void DJfaceComponent(GrayImage A);

	unsigned char DJmotion(ColorImage *A, ColorImage *B, GrayImage *C, unsigned char mode);
	void DJfaceSegment(GrayImage *A, ColorImage *B, ColorImage *C);
	
	// 10.11.01
	// color segmentation for face detection
	void DJsimple_face_detection(GrayImage *A, ColorImage *B, int phase);

	// 20010307@10:36a@DJ
	void HistoAnal(int *YBufVal, int mode = 0, int setTh = 0);
	// 20010306@16:43p@DJ
	void eyesExtract(int *proj, int projNum, CRect *eyeRect, CRect *eyebrRect);
	void eyeExtract2(int *proj, int projNum, CRect *eyeRectr, CRect *eyeRectl);
	void compExtract(int *proj, int projNum, GrayImage A);
	void DJeyeExtract(int *proj, int projNum, int mode);

	void faceSegment2(int colorSpace, int rindex, int gindex, int range1Min, int range1Max, int range2Min, int range2Max);
	void faceColor();

	
	// from Qpark.
	int findsEyeBrowMouthY(BinaryImage &b);
	int findFace(BinaryImage &b, int *x1, int *x2, int *y1, int *y2);
	int projSave(char *filename, int *X, int n, BinaryImage *b = NULL, int start = 0);
	int findnpts(int *in, int n, int npts=4, int diff = 5);
	int findninterval(int *X, int n, int npts=4, int diff=0);
	void faceLocate();
	int inclusionTest(int *a1, int a1num, int *a2, int a2num);
	// 2000.10.11
	void findFaceDJ(BinaryImage b, int *x1, int *x2, int *y1, int *y2);
	int *findMouthDJ(BinaryImage b, int x1, int x2, int y1, int y2);
	int *findMouthDJ2(BinaryImage b, int x1, int x2, int y1, int y2);

	// DJ. 20000907
	// Find face.
	// Extract Mouth.
	float getL1WithLips();
	double getfG(GrayImage A, double wave);
	// Face Segment by YCrCb (or other C.S.)
	float FaceSegment();

	void grTruthChk(BinaryImage grImg, BinaryImage segImg, double &Correct, double &SkinError, double &NonSkinError, double &Skin);
	void GroundTruth();

	double ProbMf(double mr, double mg, double srr, double sgg, double r, double g);
	void getPDF();
	double *ImgPDF(double mr, double mg, double srr, double sgg, int rindex, int gindex);

	void PDFSegment(double mr, double mg, double srr, double sgg, int rindex, int gindex, double th);
	void PDFSegmentAnalysis(double mr, double mg, double srr, double sgg, int rindex, int gindex);
	void LTSegmentAnalysis(int rindex,int gindex);
	void LTSegmentAnalysis2(int rindex,int gindex);
	// multi-analysis of LT method
	void LTSegmentAnalysis3(int rindex, int gindex, char pstr[], ColorSpaceTransform::ColorSys sys);

	// 2000. 12. 5
	// DJ
	// color image -> binary image using given th[][] set.
	// sys -> color space
	// th[][2] -> flag for InBound, OutOfBound, UnUsed (0, 1, 2)
	void faceSegment(ColorImage sImg, BinaryImage *dImg, ColorSpaceTransform::ColorSys sys, int th[3][3], unsigned char oper);
};

#endif // !defined(AFX_FACETOOL_H__DCAEA1E0_3366_11D4_B764_00E0294BE14D__INCLUDED_)
