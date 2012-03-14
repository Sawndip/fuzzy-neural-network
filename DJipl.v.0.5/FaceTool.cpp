// FaceTool.cpp: implementation of the CFaceTool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "FaceTool.h"
#include "ImgMisc.h"
#include "GaborFilter.h"
#include "math.h"
#include "Algebra.h"
#include "histogram.h"
#include "ImageFiltering.h"
//#include "VisMatrix.h"
#include "myresource.h"

#define thisnum	256

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// parameters
float faceAspectRatio = (float)(135./95.);	// = h/w
float faceAspectRatioMargin1 =  (float)(25./95.);
float faceAspectRatioMargin2 =  (float)(10./95.);

int face_width = 0, face_height = 0;
int brow_x1, brow_x2, brow_x3, brow_x4;
int eye_x1, eye_x2, eye_x3, eye_x4;
int nostril_x1, nostril_x2, nostril_x3, nostril_x4;
int mouth_x1, mouth_x2;
int brow_y1, brow_y2;
int eye_y1, eye_y2;
int nostril_y1, nostril_y2;
int mouth_y1, mouth_y2;

int saveflag = 1;

//
// Parameters
// 
// static int skinTHs[6] = {60, 227, 50, 242, 37, 255};
//static int skinTHs[6] = {13, 230, 125, 195, 83, 170};
static int skinTHs[6] = {34, 222, 58, 236, 11, 255};
//static int skinTHs[6] = {30, 245, 20, 230, 82, 195};
static int eyeTHs[6] = {40, 212, 0, 255, 12, 198};
static int browTHs[6] = {0, 255, 100, 205, 0, 255}; // hair/brow/eye/lip corner
//static int browTHs[6] = {204, 233, 0, 60, 12, 255};
////////////////////////////////////////////////////////////////////////
GrayImage *edgeimage = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFaceTool::CFaceTool()
{
	faceTh[0][0] = 10;
	faceTh[0][1] = 128;
	faceTh[1][0] = 110;
	faceTh[1][1] = 130;

}

CFaceTool::CFaceTool(ColorImage *colimage)
{
	colorimage = colimage;
	w = colorimage->w();
	h = colorimage->h();

	faceTh[0][0] = 10;
	faceTh[0][1] = 128;
	faceTh[1][0] = 110;
	faceTh[1][1] = 130;

	GrayImage null(w, h);
	CHAT_mask = null.black(w, h, 1);

	/*
	double u = 1.0f, a = 1.0f;
	CImageFiltering filter;
	filter.GaussianFilterCo(u, a, kersize, kernel);
	*/
}

CFaceTool::~CFaceTool()
{
}

// HLS 변환을 통하여 얼굴 분할 및 눈, 머리 등을 구한다.
// 규태형이 작성한 고전 함수.
// 얼굴 찾는 부분은 ok. 하지만 얼굴 요소를 구하는 부분은...?
void CFaceTool::faceLocate()
{
	GrayImage hue, lum, sat;

	// 1. rgb2hls color space transform
	ColorImage ci(*colorimage);
	ColorSpaceTransform rgb2hls(ColorSpaceTransform::HLS);	
	rgb2hls.fromtransform(ci);

	ci.save("OUTPUTfacehls.bmp");
	ci.to3Gray(&hue, &lum, &sat);
	hue.save("facehue.bmp");	
	lum.save("facelum.bmp");	
	sat.save("facesat.bmp");	

	// 2. thresholding hls image
	// 2.1 skin color segmentation
	BinaryImage *skin_bin = ci.threshold(
					skinTHs[0], skinTHs[1], ColorImage::OutOfBound,// Hue
				  	skinTHs[2], skinTHs[3],ColorImage::InBound,	// Lum
				  	skinTHs[4], skinTHs[5],ColorImage::InBound);	// Sat

	// 2.2 eye color(white) segmentation
	BinaryImage *eye_bin = ci.threshold(
					eyeTHs[0], eyeTHs[1], ColorImage::InBound,// Hue
				  	eyeTHs[2], eyeTHs[3],ColorImage::InBound,	// Lum
				  	eyeTHs[4], eyeTHs[5],ColorImage::InBound);	// Sat

	// 2.3 hair/brow/mouth color(black) segmentation
	BinaryImage *brow_bin = ci.threshold(
					browTHs[0], browTHs[1], ColorImage::InBound,// Hue
				  	browTHs[2], browTHs[3],ColorImage::OutOfBound,	// Lum
				  	browTHs[4], browTHs[5],ColorImage::InBound);	// Sat
	skin_bin->save("skin1.bmp");
	eye_bin->save("eye1.bmp");
	brow_bin->save("brow1.bmp");

	printf ("finding face location...\n");
	int x1, x2, y1, y2;
	skin_bin->setROI(0,0,skin_bin->w()-1,skin_bin->h()-1);
	findFace(*skin_bin, &x1, &x2, &y1, &y2);
	face_width = x2 - x1;
	face_height = y2 - y1;
	printf ("face region: (%d, %d) (%d, %d)\n", x1, y1, x2, y2);
	printf ("face area : %d \n", abs(face_width) * abs(face_height));

/*
	BinaryImage eb_bin = (*eye_bin) + (*brow_bin);
	eb_bin.save("faceAddBrowEye.bin");

	if (verbose) printf ("horizontal edge detecting and thresholding ...\n");
	int hor_edge_kernel[] = {1, 0, -1,
							2, 0, -2,
							1, 0, -1};
	GrayImage *hor_edge_gry = lum.convolution(3,3, hor_edge_kernel, 8, 128);
	edgeimage = new GrayImage(hor_edge_gry);
	hor_edge_gry->save("faceHEdge.gry");
	hor_edge_gry->thresholdSelf(128-30,128+30, GrayImage::OutOfBound),
	hor_edge_gry->save("faceHEdgeAbs.gry");
	BinaryImage hor_edge_bin(hor_edge_gry);
	hor_edge_bin.save("faceHEdgeAbs.bin");
	eb_bin += hor_edge_bin;
	eb_bin.save("faceAddEdge.bin");

	// Morphology: thickening
	if (verbose) printf ("morphology operation ...\n");
	unsigned char *sebuf = new unsigned char[9];
	for (int x = 0; x < 9; x++) sebuf[x] = 255;
	Morphology morph(3,3, sebuf);
	BinaryImage morph_bin(&eb_bin);
	morph_bin.setROI(1,1,morph_bin.w()-1,morph_bin.h()-1);
*/
/*
	morph.dilation(eb_bin, morph_bin);
	morph_bin.save("faceDil.bin");
	morph.erosion(eb_bin, morph_bin);
	morph_bin.save("faceEro.bin");
	morph.closing(eb_bin, morph_bin);
	morph_bin.save("faceClose.bin");
*/
//	morph.opening(eb_bin, morph_bin);
//	morph_bin.save("faceOpen.bin");
/*
	morph.thickening(*hair_rejected_bin, morph_bin, 3);
	morph_bin.save("faceThick.bin");
	morph.thinning(*hair_rejected_bin, morph_bin, 3);
	morph_bin.save("faceThin.bin");
*/

//	ImageLabeling label(&eb_bin);
/*
	ImageLabeling label(&morph_bin);
	GrayImage *label_bin = label.labeling();
	label_bin->save("lebel1.bin");
	ProfileData area_prof(label.getLength(),label.getAreas());
	int ns[256], n;
	area_prof.getMax(ns, n);
	if (n != 1) {
		printf ("warning: max area too many: %d\n", n);
	}
	BinaryImage *hair_rejected_bin = label.select(1, (int)label.getArea(ns[0]) - 1, 
									ImageLabeling::Area, 
									ImageLabeling::InBound);

	hair_rejected_bin->save("fparts.bin");
	system("display fparts.bin&");
*/

//	drawscene2();
	// 4. Projection
/*
	printf ("locating done\n");
	if (abs(face_width) * abs(face_height) < 2500) {
		printf (" image face region : two small\n");
	} else {
		hair_rejected_bin->setROI(x1, y1, x2, y2);
		findsEyeBrowMouthY(*hair_rejected_bin);
	}
*/
//	drawflush();

//	delete label_bin;
//	delete labsel;
	delete skin_bin;
	delete eye_bin;
	delete brow_bin;
//	delete hair_rejected_bin;
//	delete hor_edge_gry;
	printf ("Locating face done\n");
}

/*
void faceLocateOrg()
{
	GrayImage hue, lum, sat;

	// 1. rgb2hls color space transform
	ColorImage ci(*colorimage);
	ColorSpaceTransform rgb2hls(ColorSpaceTransform::HLS);
	rgb2hls.fromtransform(ci);
		ci.save("face.hls");
		ci.to3Gray(&hue, &lum, &sat);
		hue.save("face.hue");	
		lum.save("face.lum");	
		sat.save("face.sat");	

	// 2. thresholding hls image
	// 2.1 skin color segmentation
	BinaryImage *skin_bin = ci.threshold(
					60, 227, ColorImage::OutOfBound,// Hue
				  	50,242,ColorImage::InBound,	// Lum
				  	37,255,ColorImage::InBound);	// Sat

	// 2.2 eye color(white) segmentation
	BinaryImage *eye_bin = ci.threshold(
					40, 212, ColorImage::InBound,// Hue
				  	0,255,ColorImage::InBound,	// Lum
				  	12,198,ColorImage::InBound);	// Sat

	// 2.3 hair/brow/mouth color(black) segmentation
	BinaryImage *brow_bin = ci.threshold(
					204, 233, ColorImage::InBound,// Hue
				  	0,60,ColorImage::InBound,	// Lum
				  	12,255,ColorImage::InBound);	// Sat
	skin_bin->save("skin1.bin");
	eye_bin->save("eye1.bin");
	brow_bin->save("brow1.bin");

	BinaryImage eb_bin = (*eye_bin) + (*brow_bin);
	eb_bin.save("faceAddBrowEye.bin");

	// 4. Projection
	printf ("finding face location...\n");
	int x1, x2, y1, y2;
	skin_bin->setROI(0,0,skin_bin->w()-1,skin_bin->h()-1);
	findFace(*skin_bin, &x1, &x2, &y1, &y2);
	face_width = x2 - x1;
	face_height = y2 - y1;
	printf ("face region: (%d, %d) (%d, %d)\n", x1, y1, x2, y2);
	printf ("face area : %d \n", abs(face_width) * abs(face_height));

	int hor_edge_kernel[] = {1, 0, -1,
							2, 0, -2,
							1, 0, -1};

	GrayImage *hor_edge_gry = lum.convolution(3,3, hor_edge_kernel, 8, 128);
	edgeimage = new GrayImage(hor_edge_gry);
	hor_edge_gry->save("faceHEdge.gry");
	hor_edge_gry->thresholdSelf(128-30,128+30, GrayImage::OutOfBound),
	hor_edge_gry->save("faceHEdgeAbs.gry");
	BinaryImage hor_edge_bin(hor_edge_gry);
	hor_edge_bin.save("faceHEdgeAbs.bin");
	eb_bin += hor_edge_bin;
	eb_bin.save("faceAddEdge.bin");

	// Morphology: thickening
	printf ("morphology test\n");
	unsigned char *sebuf = new unsigned char[9];
	for (int x = 0; x < 9; x++) sebuf[x] = 255;
	Morphology morph(3,3, sebuf);
	BinaryImage morph_bin(&eb_bin);
	morph_bin.setROI(1,1,morph_bin.w()-1,morph_bin.h()-1);
/*
	morph.dilation(eb_bin, morph_bin);
	morph_bin.save("faceDil.bin");
	morph.erosion(eb_bin, morph_bin);
	morph_bin.save("faceEro.bin");
	morph.closing(eb_bin, morph_bin);
	morph_bin.save("faceClose.bin");
*/

/*
	morph.opening(eb_bin, morph_bin);
	morph_bin.save("faceOpen.bin");
*/
	
	/*
	morph.thickening(*hair_rejected_bin, morph_bin, 3);
	morph_bin.save("faceThick.bin");
	morph.thinning(*hair_rejected_bin, morph_bin, 3);
	morph_bin.save("faceThin.bin");
*/

//	ImageLabeling label(&eb_bin);

/*
	ImageLabeling label(&morph_bin);
	GrayImage *label_bin = label.labeling();
	label_bin->save("lebel1.bin");
	ProfileData area_prof(label.getLength(),label.getAreas());
	int ns[256], n;
	area_prof.getMax(ns, n);
	if (n != 1) {
		printf ("warning: max area too many: %d\n", n);
	}
	BinaryImage *hair_rejected_bin = label.select(1, (int)label.getArea(ns[0]) - 1, 
									ImageLabeling::Area, 
									ImageLabeling::InBound);

	hair_rejected_bin->save("fparts.bin");
	system("display fparts.bin&");

	drawscene2();
*/
  
	// 4. Projection
/*
	if (abs(face_width) * abs(face_height) < 2500) {
		printf (" image face region : two small\n");
	} else {
		hair_rejected_bin->setROI(x1, y1, x2, y2);
		findsEyeBrowMouthY(*hair_rejected_bin);
	}
*/

/*
	drawflush();

	delete label_bin;
//	delete labsel;
	delete skin_bin;
	delete eye_bin;
	delete brow_bin;
	delete hair_rejected_bin;
	delete hor_edge_gry;
}
*/

// 아래에 보면 알겠지만 inclusion test 라고 되어 있다.
// 규태형이 작성한 것.
int CFaceTool::inclusionTest(int *a1, int a1num, int *a2, int a2num)
{
	// whether a2 interval is included in a1 intervals
	int i, j, k = 0;
if (debug) { // interprete
	printf ("a1[] = ");
	for (i = 0; i < a1num/2; i++) {
		printf ("[%d, %d], ", a1[2*i], a1[2*i+1]);
	}
	printf ("\n");
	printf ("a2[] = ");
	for (i = 0; i < a2num/2; i++) {
		printf ("[%d, %d], ", a2[2*i], a2[2*i+1]);
	}
	printf ("\n");
	
}
	for (i = 0; i < a2num/2; i++) {
		int done = 0;
		for (j = k; j < a1num/2 && !done; j++) {
			if ((a2[2*i] >= a1[2*j]) && (a2[2*i+1] <= a1[2*j+1])) {
				done = 1;
			}
		}
		if (done) k++;
		else {
			for (j = k; j < (a1num/2-1); j++) {
				a1[2*j] = a1[2*(j+1)];
				a1[2*j+1] = a1[2*(j+1)+1];
			}
			a1num--; a1num--;
		}
	}
	if (debug) { // interprete
		printf ("a1[] = ");
		for (i = 0; i < a1num/2; i++) {
			printf ("[%d, %d], ", a1[2*i], a1[2*i+1]);
		}
		printf ("\n");
		if (k*2 == a2num) {
			printf ("all interval included\n");
		}

	}
	return 2*k;
}

// 규태형이 만든 함수.
// 눈, 눈썹, 입 등을 찾아내는 함수일 것이다.
// 면밀히 검토바람.
int CFaceTool::findsEyeBrowMouthY(BinaryImage &b)
{
	int x, y;	
	int w;

	int *projs_y = b.proj(BinaryImage::Y);
	if (saveflag) 
		projSave("faceholey.dat", projs_y, b.getROIHeight(), &b);
	ProfileData profy(b.getROIHeight(), projs_y);
	
	float thresh[] = { 0.1f, 0.30f, 0.8f};
	int yns[512], yyns[512];
	int ynum, yynum;
	int y_max = profy.getMax();
	float th;
debug = 1;
	printf ("face_width = %d \n", face_width);
	th =  thresh[0]*(float)face_width/(float)profy.getMax();
	yynum = profy.getPulse(yyns, th);
	ynum = profy.selectPulse(yns, yyns, yynum, 2, -1);	
	for (y = 0; y < ynum; y++) 
		printf ("%d ", b.height - (yns[y] + b.getROIy1()));
	printf ("\n");
	th =  thresh[1]*(float)face_width/(float)profy.getMax();
	yynum = profy.getPulse(yyns, th);
	for (y = 0; y < yynum; y++) 
		printf ("%d ", b.height - (yyns[y] + b.getROIy1()));
	printf ("\n");
	// eye & brow inclusion check
	printf ("check inclusion for eye & brow:");
	if (yyns[yynum-1] <= yns[ynum-1] && yyns[yynum-4]>= yns[ynum-2]) {
		printf ("included\n");
		yns[ynum+1] = yns[ynum-1];
		yns[ynum] = yns[ynum-1] = (yyns[yynum-2] + yyns[yynum-3])/2;
		ynum++; ynum++;
	}
	if (ynum < 8) {
		printf ("Warning: may miss y coordinate\n");
		printf ("         found %d points\n", ynum);
	}
debug = 0;

	printf ("ROI: (%d, %d), (%d, %d)\n", 
					b.getROIx1(),b.getROIy1(),b.getROIx2(),b.getROIy2());
	printf ("%d percent \n", (int)(thresh[0] * 100.0));
	for (y = 0; y < ynum; y++) 
		printf ("%d ", b.height - (yns[y] + b.getROIy1()));
	printf ("\n");
	
	int j = ynum-1;
	if (j > 0) {
		brow_y2 = yns[j] + b.getROIy1(); j--;
		brow_y1 = yns[j] + b.getROIy1(); j--;
	}
	if (j > 0) {
		eye_y2 = yns[j] + b.getROIy1(); j--;
		eye_y1 = yns[j] + b.getROIy1(); j--;
	}
	if (j > 0) {
		nostril_y2 = yns[j] + b.getROIy1(); j--;
		nostril_y1 = yns[j] + b.getROIy1(); j--;
	}
	if (j > 0) {
		mouth_y2 = yns[j] + b.getROIy1(); j--;
		mouth_y1 = yns[j] + b.getROIy1(); j--;
	} else {
		printf ("Error: j  = %d\n");
	}

	printf ("brows : %d %d\n", b.height - brow_y2, b.height - brow_y1);
	printf ("eyes : %d %d\n", b.height - eye_y2, b.height - eye_y1);
	printf ("nostril : %d %d\n", b.height - nostril_y2, b.height - nostril_y1);
	printf ("mouth : %d %d\n", b.height - mouth_y2, b.height - mouth_y1);

	//////////////////////////////////////////////////////////////////////////
	// find x coords
	printf ("\n");
	printf ("finding brow x-coordinate\n");
	b.setROI(0,brow_y1,b.w()-1,brow_y2);
	int *projs_x = b.proj(BinaryImage::X);
	if (saveflag)
		projSave("brows.dat", projs_x, b.getROIWidth(), NULL, b.getROIx1());

	ProfileData profx(b.getROIWidth(), projs_x);
	
	int xns[512], xxns[512];
	int xnum, xxnum;
debug = 0;
	w = abs(brow_y1 - brow_y2);
	th = (float)(0.5 * (float)w/(float)profx.getMax());
	xxnum = profx.getPulse(xxns, th);	
	xnum = profx.selectPulse(xns, xxns, xxnum, 2, -1);	
debug = 0;
	if (xnum > 4) {
		printf ("Warning: %d pts detected(>4)\n", xnum);
		xxnum = xnum;
		xnum = findninterval(xns, xxnum, 4);
	}
	for (x = 0; x < xnum; x++) {
		printf ("%d ", xns[x] + b.getROIx1());
	}
	printf ("\n");

	j = 0;
	if (j < xnum-1) {
		brow_x1 = xns[j] + b.getROIx1(); j++;
		brow_x2 = xns[j] + b.getROIx1(); j++;
	}
	if (j < xnum-1) {
		brow_x3 = xns[j] + b.getROIx1(); j++;
		brow_x4 = xns[j] + b.getROIx1(); j++;
	} else {
		printf ("Error: j  = %d\n");
	}

	delete projs_x;

	printf ("Brows:  R(%d, %d), (%d, %d) L(%d, %d), (%d, %d)\n",
		brow_x1, b.height - brow_y2, brow_x2, b.height - brow_y1, 
		brow_x3, b.height - brow_y2, brow_x4, b.height - brow_y1); 
	printf ("Brows:  R(%d, %d), (%d, %d) L(%d, %d), (%d, %d)\n",
		brow_x1, brow_y1, brow_x2, brow_y2, 
		brow_x3, brow_y1, brow_x4, brow_y2); 

	// eyes
	printf ("\n");
	printf ("finding eyes x-coordinate\n");

	b.setROI(0,eye_y1,b.w()-1,eye_y2);
	projs_x = b.proj(BinaryImage::X);
	if (saveflag)
		projSave("eyes.dat", projs_x, b.getROIWidth(), NULL, b.getROIx1());
	profx.set(b.getROIWidth(), projs_x);
	
debug = 0;
	w = abs(eye_y1 - eye_y2);
	th = (float)(0.5 * (float)w /(float)profx.getMax());
	xxnum = profx.getPulse(xxns, th);	
	xnum = profx.selectPulse(xns, xxns, xxnum, 2, -1);	
debug = 0;
	if (xnum > 4) {
		printf ("Warning: %d pts detected(>4)\n", xnum);
		xxnum = xnum;
		xnum = findninterval(xns, xxnum, 4);
	}
	for (x = 0; x < xnum; x++) {
		printf ("%d ", xns[x] + b.getROIx1());
	}
	printf ("\n");

	j = 0;
	if (j < xnum-1) {
		eye_x1 = xns[j] + b.getROIx1(); j++;
		eye_x2 = xns[j] + b.getROIx1(); j++;
	}
	if (j < xnum-1) {
		eye_x3 = xns[j] + b.getROIx1(); j++;
		eye_x4 = xns[j] + b.getROIx1(); j++;
	} else {
		printf ("Error: j  = %d\n");
	}
	delete projs_x;

	printf ("eyes:  R(%d, %d), (%d, %d) L(%d, %d), (%d, %d)\n",
		eye_x1, b.height - eye_y2, eye_x2, b.height - eye_y1, 
		eye_x3, b.height - eye_y2, eye_x4, b.height - eye_y1); 
	delete projs_x;
	printf ("eyes:  R(%d, %d), (%d, %d) L(%d, %d), (%d, %d)\n",
		eye_x1, eye_y1, eye_x2, eye_y2, 
		eye_x3, eye_y1, eye_x4, eye_y2); 

	// nostril
	printf ("\n");
	printf ("finding nostril x-coordinate\n");
	b.setROI(MIN(brow_x1,eye_x1),nostril_y1,MAX(brow_x4,eye_x4),nostril_y2);
	projs_x = b.proj(BinaryImage::X);
	profx.set(b.getROIWidth(), projs_x);
	if (saveflag)
		projSave("nostrils.dat", projs_x, b.getROIWidth(), NULL, b.getROIx1());
debug = 1;
	w = abs(nostril_y1 - nostril_y2);
	th = (float)(0.5 * (float)((float)w /(float)profx.getMax()));
	xxnum = profx.getPulse(xxns, th);	
	xnum = profx.selectPulse(xns, xxns, xxnum, 2, -1);	
debug = 0;
	if (xnum > 4) {
		printf ("Warning: %d pts detected(>4)\n", xnum);
		xxnum = xnum;
		xnum = findninterval(xns, xxnum, 4);
	}
	for (x = 0; x < xnum; x++) {
		printf ("%d ", xns[x] + b.getROIx1());
	}
	printf ("\n");

	j = 0;
	if (j < xnum-1) {
		nostril_x1 = xns[j] + b.getROIx1(); j++;
		nostril_x2 = xns[j] + b.getROIx1(); j++;
	}
	if (j < xnum-1) {
		nostril_x3 = xns[j] + b.getROIx1(); j++;
		nostril_x4 = xns[j] + b.getROIx1(); j++;
	} else {
		printf ("Error: j  = %d\n");
	}

	delete projs_x;

	printf ("nostrils:  R(%d, %d), (%d, %d) L(%d, %d), (%d, %d) \n",
	nostril_x1, b.height - nostril_y2, nostril_x2, b.height - nostril_y1, 
	nostril_x3, b.height - nostril_y2, nostril_x4, b.height - nostril_y1); 

	printf ("nostrils:  R(%d, %d), (%d, %d) L(%d, %d), (%d, %d) \n",
		nostril_x1, nostril_y1, nostril_x2, nostril_y2,
		nostril_x3, nostril_y1, nostril_x4, nostril_y2);

	// mouth
	printf ("\n");
	printf ("finding mouth x-coordinate\n");
	b.setROI(eye_x1,mouth_y1,eye_x4,mouth_y2);
	projs_x = b.proj(BinaryImage::X);
	if (saveflag)
		projSave("mouth.dat", projs_x, b.getROIWidth(), NULL, b.getROIx1());

	profx.set(b.getROIWidth(), projs_x);
	
debug = 0;
	w = abs(mouth_y1 - mouth_y2);
	th = (float)(0.5 * (float)w /(float)profx.getMax());
	xxnum = profx.getPulse(xxns, th);	
	xnum = profx.selectPulse(xns, xxns, xxnum, 2, -1);	
debug = 0;
	if (xnum > 2) {
		printf ("Warning: %d pts detected(>2)\n", xnum);
		xxnum = xnum;
		xnum = findninterval(xns, xxnum, 2);
	}
	for (x = 0; x < xnum; x++) {
		printf ("%d ", xns[x] + b.getROIx1());
	}
	printf ("\n");

	j = 0;
	if (j < xnum-1) {
		mouth_x1 = xns[j] + b.getROIx1(); j++;
		mouth_x2 = xns[j] + b.getROIx1(); j++;
	} else {
		printf ("Error: j  = %d\n");
	}

	delete projs_x;

	printf ("mouth:  (%d, %d), (%d, %d)\n",
		mouth_x1, b.height - mouth_y2, mouth_x2, b.height - mouth_y1);
	printf ("mouth:  (%d, %d), (%d, %d) \n",
		mouth_x1, mouth_y1, mouth_x2, mouth_y2) ;

/*	drawRec (brow_x1, brow_y1, brow_x2, brow_y2);
	drawRec (brow_x3, brow_y1, brow_x4, brow_y2);
	drawRec (eye_x1, eye_y1, eye_x2, eye_y2);
	drawRec (eye_x3, eye_y1, eye_x4, eye_y2);
	drawRec (nostril_x1, nostril_y1, nostril_x2, nostril_y2);
	drawRec (nostril_x3, nostril_y1, nostril_x4, nostril_y2);
	drawRec (mouth_x1, mouth_y1, mouth_x2, mouth_y2);
*/
	delete projs_y;
	return 0;
}

// 분할된 얼굴 영상에 대하여 사각형 테두리를 구한다.
// 이 때, 수직 및 수평 투영을 이용하여 사각형 테두리를 구한다.
// 역시 고전적인 규태형의 함수.
int CFaceTool::findFace(BinaryImage &b, int *x1, int *x2, int *y1, int *y2)
{
	int *projs_y = b.proj(BinaryImage::Y);
	int *projs_x = b.proj(BinaryImage::X);

	int x, y;	
	FILE *foutx, *fouty;

	if((foutx = fopen("facex.dat", "w")) == NULL) {
		fprintf (stderr,"cannot open file: facex.dat\n");
		return -1;
	}
	if((fouty = fopen("facey.dat", "w")) == NULL) {
		fprintf (stderr,"cannot open file: facey.dat\n");
		return -1;
	}

	for (x = 0; x < b.getROIWidth(); x++) 
		fprintf (foutx, "%d %d\n", x + b.getROIx1(), projs_x[x]);

	for (y = 0; y < b.getROIHeight(); y++) 
		fprintf (fouty, "%d %d\n", b.height - (y + b.getROIy1()), projs_y[y]);

	fclose(foutx);
	fclose(fouty);

	ProfileData profx(b.getROIWidth(), projs_x);
	ProfileData profy(b.getROIHeight(), projs_y);
	
	printf ("face region: projection x, 45 percent threshold\n");

	int xns[thisnum], yns[thisnum];
	int xxns[thisnum], yyns[thisnum];
	int xnum, ynum;
	int xxnum, yynum;

	xxnum = profx.getPulse(xxns, 0.4f);	
	xnum = profx.selectPulse(xns, xxns, xxnum, 5, -1);	
debug = 0;
	if (xnum > 2) {
		printf ("Warning: %d pts detected(>2)\n", xnum);
		xxnum = xnum;
		xnum = findninterval(xns, xxnum, 2);
	}
	for (x = 0; x < xnum; x++) 
		printf ("%d ", xns[x] + b.getROIx1());
	printf ("\n");
	yynum = profy.getPulse(yyns, 0.3f);
	ynum = profy.selectPulse(yns, yyns, yynum, 5, -1);	
	if (ynum > 2) {
		printf ("Warning: %d pts detected(>2)\n", ynum);
		yynum = ynum;
		ynum = findninterval(yns, yynum, 2);
	}
	for (y = 0; y < ynum; y++) 
		printf ("%d ", b.height - (yns[y] + b.getROIy1()));
	printf ("\n");
/*
	int xnum = profx.getPercent(xns, 0.40, 0.0030);
	printf ("profile x(%d) ", xnum);
	for (x = 0; x < xnum; x++) 
		printf ("%d ", xns[x]);
	printf ("\n");
	printf ("max of proj y: %d \n", profy.getMax());
	printf ("face region: projection y, 40 percent threshold\n");
	int ynum = profy.getPercent(yns, 0.30, 0.0040);
	for (y = 0; y < ynum; y++) 
		printf ("%d ", b.height - yns[y]);
	printf ("\n");
	
	int xxnum = xnum;
	for (x = 0; x < xnum; x++) 
		if (xns[xnum - x - 1] == xns[xnum - x-2] + 1) {
			for (int j = xnum-x-1; j < xxnum-1; j++)
				xns[j] = xns[j+1];
			xxnum --;
		}
	for (x = 0; x < xxnum; x++) {
		printf ("%d ", xns[x]);
	}
	printf ("\n");

	if (ynum == 1) {
		ynum = 2;
		yns[1] = yns[0];
		yns[0] = b.getROIy1();
	}
	int yynum = ynum;
	for (y = 0; y < ynum; y++) 
		if (yns[ynum - y - 1] == yns[ynum - y-2]+1) {
			for (int j = ynum-y-1; j < yynum-1; j++)
				yns[j] = yns[j+1];
			yynum --;
		}
	for (y = 0; y < yynum; y++) {
		printf ("%d ", yns[y]);
	}
	printf ("\n");
*/

	if(debug)
		for (y = 0; y < ynum; y++) 
			if (yns[y] == yns[y-1]) xns[y] = -1;

	int th1 = (int)((faceAspectRatio - faceAspectRatioMargin1) * (float)(xns[1] - xns[0]));
	int th2 = (int)((faceAspectRatio + faceAspectRatioMargin2) * (float)(xns[1] - xns[0]));
	int yy1, yy2;
	int cnt = 0;
	printf ("th : %d, %d\n", th1, th2);
	for (y = 0; y < ynum; y++) 
		for (int j = y+1; j < ynum; j++)
			if (abs(yns[y] - yns[j]) >= th1 && abs(yns[y] - yns[j]) <= th2) {
				yy1 = yns[y];
				yy2 = yns[j];
				printf ("satisfied region %d\n", cnt++);
			}
	if (cnt) {
		yns[0] = yy1;
		yns[1] = yy2;
	} else {
		printf ("satisfied region not found\n");
		yns[1] = yns[ynum-1];
	}
	printf ("face region verification\n");
	float as = (float)(yns[1] - yns[0]) / (float)(xns[1] - xns[0]);
	printf ("aspect ratio = %f(reference = [%f, %f, %f])\n", as, 
				faceAspectRatio - faceAspectRatioMargin1, 
				faceAspectRatio, faceAspectRatio + faceAspectRatioMargin2 );
	xns[0] += b.getROIx1(); yns[0] += b.getROIy1();
	xns[1] += b.getROIx1(); yns[1] += b.getROIy1();
//	drawRec (xns[0], yns[0], xns[1], yns[1]);

/*
	if (xxnum == 2 && yynum == 2) {
//		drawscene2();
		drawRec (xns[0], yns[0], xns[1], yns[1]);
//		drawflush();
	} else {
		*x1 = xns[0]; *y1 = yns[0];
		*x2 = xns[1]; *y2 = yns[1];
		return -1;
	}
*/
	*x1 = xns[0]; *y1 = yns[0];
	*x2 = xns[1]; *y2 = yns[1];
	
	delete [] projs_y;
	delete [] projs_x;

	return 0;
}

// 투영된 값을 저장한다.
int CFaceTool::projSave(char *filename, int *X, int n, BinaryImage *b, int start)
{
	FILE *fouty;

	if((fouty = fopen(filename, "w")) == NULL) {
		fprintf (stderr,"cannot open file: %s\n", filename);
		return -1;
	} 
	if (b) {
		for (int y = 0; y < n; y++) 
			fprintf (fouty, "%d %d\n", 
							b->h() - (y + b->getROIy1()), X[y]);
	} else {
		for (int x = 0; x < n; x++) 
			fprintf (fouty, "%d %d\n", x + start, X[x]);

	}
	fclose(fouty);
	return 0;
}

// 음... findninterval과 유사한 것으로 보임.
// 전자가 interval을 찾는다면, 후자는 pts를 찾는 듯... -.-
int CFaceTool::findnpts(int *X, int n, int npts, int diff)
{
	int m = n;

if(debug){
	for (int i = 0; i < n; i++) 
		printf ("%d ", X[i]);
	printf ("\n");
}

	for (int k = 1; k < diff; k++) {
		if(debug)
			printf ("k = %d \n", k);
		for (int i = 0; i < n-1; i++) {
			if (abs(X[n - i - 1] - X[n -i-2]) < k) {
				if (n-i-3 >=0) {
					if (abs(X[n-i-1] - X[n-i]) < abs(X[n-i-2] - X[n-i-3])) {
						for (int j = n-i-1; j < m-1; j++)
							X[j] = X[j+1];
						m--;

if(debug) {
	printf ("0, i=%d \n", i);
	for (int a = 0; a < m; a++) 
		printf ("%d ", X[a]);
	printf ("\n");
}
						if (m == npts) { return m;}
					} else {
						for (int j = n-i-2; j < m-1; j++)
							X[j] = X[j+1];
						m--;
if(debug){
	printf ("1, i=%d \n", i);
	for (int a = 0; a < m; a++) 
		printf ("%d ", X[a]);
	printf ("\n");
}
						if (m == npts) { return m;}
					}
				} else {
					for (int j = n-i-2; j < m-1; j++)
						X[j] = X[j+1];
					m--;
if(debug){
	printf ("2, i=%d \n", i);
	for (int a = 0; a < m; a++) 
		printf ("%d ", X[a]);
	printf ("\n");
}
					if (m == npts) { return m;}
				}
			}
		}
		n = m;
	}
	return m;
}

// 아직 완전히 파악이 되지 않음.
// 아마도 투영된 결과 값에 대하여 몇 개의 그룹이 존재하는지
// 찾아내는 함수인 듯함.
// eyeExtract2, eyesExtract 등의 함수를 참조할 것.
int CFaceTool::findninterval(int *X, int n, int npts, int diff)
{
	int i, j;
	int dist[512], num = n/2;
	int tmp;
	int tmp1, tmp2;

	for (i = 0; i < num; i++) 
		dist[i] = abs(X[2*i] - X[2*i+1]);
	
if (debug){
	for (i = 0; i < num; i++) 
		printf ("[%d, %d] ", X[2*i], X[2*i+1]);
	printf ("\n");
}
	for (i = 0; i < num; i++) {
		for (j = i; j < num; j++) 
			if (dist[i] < dist[j]) {
				tmp = dist[i]; dist[i] = dist[j]; dist[j] = tmp;
				tmp1 = X[2*i]; X[2*i] = X[2*j]; X[2*j] = tmp1;
				tmp2 = X[2*i+1]; X[2*i+1] = X[2*j+1]; X[2*j+1] = tmp2;
			}
	}

if (debug){
	for (i = 0; i < num; i++) 
		printf ("[%d, %d] ", X[2*i], X[2*i+1]);
	printf ("\n");
}
	for (i = 0; i < npts/2; i++) {
		for (j = i; j < npts/2; j++) 
			if (X[2*i] > X[2*j]) {
				tmp1 = X[2*i]; X[2*i] = X[2*j]; X[2*j] = tmp1;
				tmp2 = X[2*i+1]; X[2*i+1] = X[2*j+1]; X[2*j+1] = tmp2;
			}
	}
if (debug){
	for (i = 0; i < num; i++) 
		printf ("[%d, %d] ", X[2*i], X[2*i+1]);
	printf ("\n");
}
	return npts;
}
/*
void ptsInit(matrix &v, int x1, int y1, int x2, int y2);
int gflg = 0;
void snakeCB(Widget, XtPointer, XtPointer)
{
	int g_n = 5;
	int g_std = 1;	
	// edge detecting
	GrayImage *edgeI = edgeimage;
	if (gflg) {	// gaussian filtering
        printf ("-> gaussian filtering\n"); fflush(stdout);
        int f_size = g_n;
        float std = g_std;
        float *gfil = getGaussianFilter2f(std, std, f_size);
        GrayImage gaussianKernalI(f_size,f_size);
		GrayImage tmp_img = *edgeI;
        edgeI = convol2f(&tmp_img, gfil, f_size);
        edgeI->save("pI.rgb");
    } 
	ColorImage cimg(edgeI->w(), edgeI->h(), 3);
    cimg.from3Gray(edgeI,edgeI,edgeI);

	// brow[l,r], eye[l,r], mouth
	double threshold1[] = {0.25, 0.25, 0.25, 0.25, 0.25};
	int threshold2[] = {100, 100, 100, 100, 100};
	int threshold3[] = {3, 3, 3, 3, 3};
	int x1y1x2y2[5][4];
	x1y1x2y2[0][0] = brow_x1; x1y1x2y2[0][1] = brow_y1;
	x1y1x2y2[0][2] = brow_x2; x1y1x2y2[0][3] = brow_y2;
	x1y1x2y2[1][0] = brow_x3; x1y1x2y2[1][1] = brow_y1;
	x1y1x2y2[1][2] = brow_x4; x1y1x2y2[1][3] = brow_y2;
	x1y1x2y2[2][0] = eye_x1; x1y1x2y2[2][1] = eye_y1;
	x1y1x2y2[2][2] = eye_x2; x1y1x2y2[2][3] = eye_y2;
	x1y1x2y2[3][0] = eye_x3; x1y1x2y2[3][1] = eye_y1;
	x1y1x2y2[3][2] = eye_x4; x1y1x2y2[3][3] = eye_y2;
	x1y1x2y2[4][0] = mouth_x1; x1y1x2y2[4][1] = mouth_y1;
	x1y1x2y2[4][2] = mouth_x2; x1y1x2y2[4][3] = mouth_y2;

	display_flag = 0;
	filesave_flag = 0;
	print_flag = 0;
	matrix *v[5];
	for (int n = 0; n < 5; n++) {
		int pts_num = ((2*abs(x1y1x2y2[n][0] - x1y1x2y2[n][2])) + 2*abs(x1y1x2y2[n][1] - x1y1x2y2[n][3]))/4;
	   	v[n] = new matrix (2, pts_num);
		ptsInit(*v[n], x1y1x2y2[n][0], x1y1x2y2[n][1], x1y1x2y2[n][2], x1y1x2y2[n][3]);
	}
	for (n = 0; n < 5; n++) {
		SnakeGreedy(*edgeI, *v[n], threshold1[n], threshold2[n], threshold3[n]);
	}
	char rgbname[20], buf[200];
	for (n = 0; n < 5; n++) {
	    putPelImage(*edgeI, *v[n], n);
		sprintf (rgbname,"d%03d.rgb",n);
		sprintf (buf, "display %s &",rgbname);
		system(buf);
	}
}

void ptsInit(matrix &v, int x1, int y1, int x2, int y2)
{
	int n = v.n();
	int m = v.m();
	printf ("matrix; %d x %d\n", m, n);
	int i, j = 0;
	for (i = 0; i < (x2 - x1)/4 ; i++, j++) {
		v[0][j] = x1 + i * 4;
		v[1][j] = y1;
	}
	for (i = 0; i < (y2 - y1)/4 ; i++, j++) {
		v[0][j] = x2; 
		v[1][j] = y1 + i * 4;
	}
	for (i = 0; i < (x2 - x1)/4 ; i++, j++) {
		v[0][j] = x2 - i * 4;
		v[1][j] = y2;
	}
	for (i = 0; i < (y2 - y1)/4 ; i++, j++) {
		v[0][j] = x1; 
		v[1][j] = y2 - i * 4;
	}
	if (j != v.n()) {
		printf ("ptsInit error\n");
		for (i = 0;j< v.n(); j++, i++) {
			v[0][j] = x1; 
			v[1][j] = y2 + i;
		}
	}
	if (debug) 
		for (i = 0; i < v.n(); i++) 
			printf ("%.0f %.0f\n", v[0][i], v[1][i]);
}
/*/

// 2000. 12. 5
// DJ
// find face by I1I2I3 color space
// I1I2I3N 컬러공간을 이용하여 얼굴 분할을 행하고
// GG 특징값까지 얻어내는 함수.
float CFaceTool::getL1WithLips()
{
	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	int i;//,j;
	CString str;
	float val = 0.0;
	//int IRG_Th1[3] = {0,40,70};
	//int IRG_Th2[3] = {0,70,140};
	//int IRG_Th1[3] = {0,120,133};
	//int IRG_Th2[3] = {0,135,141};
	//unsigned char IRG_Th2[3][3] = {{0, 0, 2}, {120, 135, 0}, {133, 141, 2}};
	int IRG_Th2[3][3] = {{0, 0, 2}, {150, 167, 0}, {127, 137, 0}};

	ColorImage ci(*colorimage);
	GrayImage *ii = ci.getLumImg();
	BinaryImage face_region;
	
	// face segmentation
	// I1I2I3 color space
	faceSegment(ci, &face_region, ColorSpaceTransform::I1I2I3N, IRG_Th2, '&');

	//CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&face_region));

/*
	// get 'real' R-B
	// is it useful or not?
	GrayImage R1,G1,B1;
	ci.to3Gray(&R1, &G1, &B1);	

	GrayImage ir(R1);
	for(i=0;i<ir.w();i++)
		for(j=0;j<ir.h();j++)
			ir.setPixel(i,j,R1.getPixel(i,j)-B1.getPixel(i,j));				// R-B
*/

	/* segmentation by PDF
	double *ImgPDF = new double[w*h];

	int k=0;
	for(j=0;j<h;j++)
		for(i=0;i<w;i++,k++)
		{
			ImgPDF[k] = ProbMf(158.0,127.0,22.63,3.16,G1.getPixel(i,j),B1.getPixel(i,j));
			if (ImgPDF[k] >= 0.05)
				irb.setPixel(i,j,255);
			else
				irb.setPixel(i,j,0);
		}


	delete []ImgPDF;
	*/

	// morphological operation
	int bufNum = 3;
	unsigned char *buf = new unsigned char[bufNum*bufNum];
	for(int l=0;l<bufNum*bufNum;l++) buf[l] = 255;
	Morphology morph(bufNum,bufNum,buf);
	delete []buf;
	
	BinaryImage irb(&face_region);
	irb.setROI(1,1,irb.w()-1,irb.h()-1);
	
	morph.opening(face_region,irb);

	//CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&irb));

	/*
	// Yellow books
	// modified morphological operation
	GrayImage densmap(&irb);
	morph.denseMap(irb,densmap,1,1,4,4);

	GrayImage densmap3(&densmap);
	morph.erosion2(densmap,densmap3,16,3,3,1);

	GrayImage densmap4(&densmap3);
	morph.dilation2(densmap3,densmap4,16,3,3,2);

	irb = densmap4.threshold(16,16);

	morph.erosion3(irb,irb,255,3,3,4);

	morph.dilation3(irb,irb,255,3,3,5);
	*/

	// find face by DJ's method.
	int x1,x2,y1,y2;//,exit;
	findFaceDJ(irb,&x1,&x2,&y1,&y2);

	int x4,y4,x5,y5;
	int *co;

	CHECK_TIME_START;

	co = findMouthDJ2(irb,x1,x2,y1,y2);

	CHECK_TIME_END(Time1, err);
	
	x4 = co[0];
	y4 = co[1];
	x5 = co[2];
	y5 = co[3];
	delete []co;

	GrayImage result(ii);
	// face candidate
	for(i=x1;i<x2;i++)
	{
		result.setPixel(i,y1,0);
		result.setPixel(i,y2,0);
	}
	for(i=y1;i<y2;i++)
	{
		result.setPixel(x1,i,0);
		result.setPixel(x2,i,0);
	}
	// lip candidate
	for(i=x4;i<x5;i++)
	{
		result.setPixel(i,y4,255);
		result.setPixel(i,y5,255);
	}
	for(i=y4;i<y5;i++)
	{
		result.setPixel(x4,i,255);
		result.setPixel(x5,i,255);
	}
	result.save("face_result.bmp",OUTPUT);
	
	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&result));

	int y6,y7, margin = (y2-y1)/20;
	y6 = y4-margin;
	y7 = y5+margin;

	double fG, hRatio, aRatio;

	GrayImage *gabor = ii->getSubImage(x4,y6,x5,y7);

	CHECK_TIME_START;

	fG = getfG(*gabor, 4.0);
	hRatio = (double)(y5-y4)/(double)(y2-y1);
	aRatio = (double)((x5-x4)*(y5-y4))/(double)((x2-x1)*(y2-y1));

	CHECK_TIME_END(Time2, err);

	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\timeMouthEx_GG.txt", "at");
	fprintf(fp, "%d %d %f %f\n", gabor->w(), gabor->h(), Time1, Time2);
	fclose(fp);

	gabor->save("face_gabor_lip.bmp",OUTPUT);
	delete gabor;

	str.Format("Height ratio: %.4f, Area ratio: %.4f, fG: %.2f", hRatio, aRatio, fG);
	AfxMessageBox(str);

/*
	GrayImage ir2 = ir.threshold(40,70,GrayImage::InBound);
	
	ImageLabeling irbl(&ir2);
	GrayImage *irbl2 = irbl.labeling();
	irbl2->save("face_label.bmp");

	ProfileData area_prof(irbl.getLength(),irbl.getAreas());
	int ns[256], n;
	area_prof.getMax(ns, &n);

	BinaryImage *hair_rejected_bin = irbl.select(1, (int)irbl.getArea(ns[0]) - 1, 
									ImageLabeling::Area, 
									ImageLabeling::InBound);

	hair_rejected_bin->save("face_pure.bmp");

	delete hair_rejected_bin;
	delete irbl2;
*/

	delete ii;

	return val;
}

//void CFaceTool::findFace(BinaryImage &b, int *x1, int *x2, int *y1, int *y2)
// IEEE Tr. on CirSysVideo, (9), 4, Jun 1999, Chai and Ngan.
// 상기 참조 문헌의 방법을 구현한 것이다. YCrCb 컬러 공간을 이용하며,
// 다단계 morphological 과정을 통하여 얼굴 분할을 행한다.
float CFaceTool::FaceSegment()
{
	float val = 0.0;
	int i,j;

	// Seg. Th.
	//int IRG_Th1[3] = {0,133,77};
	//int IRG_Th2[3] = {0,173,127};
	int IRG_Th1[3] = {0,120,133};
	int IRG_Th2[3] = {0,135,141};

	ColorImage ci(*colorimage);

	// YCrCb Conversion.
	ColorSpaceTransform rgb2ycrcb(ColorSpaceTransform::YCRCB);
	rgb2ycrcb.fromtransform(ci);
	GrayImage Y,CR,CB;
	ci.to3Gray(&Y, &CR, &CB);

	// SubSampling
	GrayImage CRs(CR.w()/2,CB.h()/2);
	GrayImage CBs = CRs;
	for(i=0;i<CRs.w();i++)
		for(j=0;j<CRs.h();j++)
		{
			CRs.setPixel(i,j,CR.getPixel(i*2,j*2));
			CBs.setPixel(i,j,CB.getPixel(i*2,j*2));
		}

	// Step 1.
	// Color Segmentation
	BinaryImage o1(&CRs);
	BinaryImage *o1_1,*o1_2;
	o1_1 = CRs.threshold(IRG_Th1[1],IRG_Th2[1]);
	o1_2 = CBs.threshold(IRG_Th1[2],IRG_Th2[2]);
	o1 = *o1_1 & *o1_2;
	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&o1));

	// Step 2.
	// Density Regularization
	// Step 2.1
	// Density Map
	int densSum = 0;
	int k,l;
	GrayImage densmap(CRs.w()/4,CRs.h()/4);
	morph.denseMap(o1,densmap,4,4,4,4);

	// classfying for display the density map
	int densClass;
	GrayImage densmap2(&densmap);
	for(i=0;i<densmap.w();i++)
		for(j=0;j<densmap.h();j++)
		{
			if (densmap2.getPixel(i,j) == 16)
				densClass = 255;
			else if (densmap2.getPixel(i,j) == 0)
				densClass = 0;
			else
				densClass = 128;

			densmap2.setPixel(i,j,densClass);
		}

	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&densmap2));

	// Step 2.2.
	// density regularization
	// Step 2.2.1
	// discard all points at the edge of the density map
	for(i=0;i<densmap.w();i++)
	{
		densmap.setPixel(i,0,0);
		densmap.setPixel(i,densmap.h()-1,0);
	}
	for(j=0;j<densmap.h();j++)
	{
		densmap.setPixel(0,j,0);
		densmap.setPixel(densmap.w()-1,j,0);
	}
	// Step 2.2.2
	// erode any full-density point
	GrayImage densmap3(&densmap);
	morph.erosion2(densmap,densmap3,16,3,3,5);

	// Step 2.2.3
	// dilate any point of either zero or intermediate density
	GrayImage densmap4(&densmap3);
	morph.dilation2(densmap3,densmap4,16,3,3,2);

	BinaryImage o2(&densmap4);
	o2 = o2.threshold(16,16);
	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&o2));

	// Step 3.
	// Luminance Regularization
	double Wsum, Wsumsq;
	double *sigma = new double[o2.w()*o2.h()];
	BinaryImage o3(&o2);
	for(i=0;i<o2.w();i++)
		for(j=0;j<o2.h();j++)
		{
			if (o2.getPixel(i,j) == 255)
			{
			Wsum = Wsumsq = 0.0;
			for(k=0;k<8;k++)
				for(l=0;l<8;l++)
				{
					Wsum += Y.getPixel(i+k,j+l);
				}
			Wsum /= 64.0;
			for(k=0;k<9;k++)
				for(l=0;l<9;l++)
				{
					Wsumsq += ((Y.getPixel(i+k,j+l)-Wsum)*(Y.getPixel(i+k,j+l)-Wsum));
				}

			sigma[i*o2.h()+j] = sqrt(Wsumsq/63.0);

			if (sigma[i*o2.h()+j] >= 2.0)
				o3.setPixel(i,j,255);
			else
				o3.setPixel(i,j,0);
			}
			else
				o3.setPixel(i,j,0);
		}
	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&o3));

	// Step 4.
	// Geometric Correction
	// Step 4.1
	// Preprocess for noise reduction
	// search detected pixel
	BinaryImage o3_1(&o3);
	morph.erosion3(o3,o3_1,255,3,3,4);

	// Step 4.2
	// dilate any point of either zero or intermediate density
	morph.dilation3(o3,o3_1,255,3,3,5);

	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&o3_1));

	// Step 4.3
	// delete short segment
	// Under consideration...

	// Step 5.
	// Extract facial region
	// Oversampling
	ColorImage face(CBs.w(),CBs.h(),3);
	for(i=0;i<face.w();i++)
		for(j=0;j<face.h();j++)
		{
			if (o3_1.getPixel(i/4,j/4) == 255)
			{
				face.setPixel(i,j,colorimage->getPixel(i*2,j*2,0),colorimage->getPixel(i*2,j*2,1),colorimage->getPixel(i*2,j*2,2));
			}
		}

	CImgMisc::ShowImg(CImgMisc::ImgConvC_c(&face));

	delete [] sigma;
	delete o1_1, o1_2;

	return val;
}

// 주어진 비교 영상과 ground 영상을 대조하여 C, SE, NSE, S를
// 직접 구하는 함수.
void CFaceTool::grTruthChk(BinaryImage grImg, BinaryImage segImg, double &Correct, double &SkinError, double &NonSkinError, double &Skin)
{
	// pixel number for total, skin, non-skin
	double tPix, sPix = 0, nsPix = 0;
	tPix = grImg.w()*grImg.h();

	double sPixHit = 0, nsPixHit = 0;
	double sePixHit = 0, nsePixHit = 0;
	
	int k;

	for (k=0; k<tPix; k++)
	{
		if (grImg.buf[0][k] == 255)
		{
			sPix++;

			if (segImg.buf[0][k] == 255)
				sPixHit++;
			else
				sePixHit++;
		}
		else
		{
			nsPix++;
			if (segImg.buf[0][k] == 0)
				nsPixHit++;
			else
				nsePixHit++;
		}
	}

	nsPix = tPix - sPix;

	Correct = (sPixHit+nsPixHit)/tPix;
	SkinError = sePixHit/tPix;
	NonSkinError = nsePixHit/tPix;
	Skin = sPixHit/sPix;
}

// ground 영상과 원영상을 이용하여
// 성능평가지수 C, SE, NSE, S 등을 구하는 함수.
void CFaceTool::GroundTruth()
{
	double Cr,SEr,NSEr,Sr;
	char str[100];

	BinaryImage grImg, segImg;
	segImg.load("cfacea000.bmp", TRAINING);
	grImg.load("gfacea000.bmp", TRAINING);

	grTruthChk(grImg, segImg, Cr,SEr,NSEr,Sr);

	sprintf(str,"C=%.2f, SE=%.2f, NSE =%.2f, S=%.2f",Cr,SEr,NSEr,Sr);
	AfxMessageBox(str);
}

// Membership Fuction 값을 구하는 함수.
// 2차원 gaussian 분포에 대하여 주어진 평균, 분산 및 값을 이용해 
// Mf를 구한다.
double CFaceTool::ProbMf(double mr, double mg, double srr, double sgg, double r, double g)
{
	double value;
	// 2D Gaussian distribution
	// value : [0,1]
	value = exp(-((r-mr)*(r-mr)/srr+(g-mg)*(g-mg)/sgg)/2.0);

	return value;
}

// PDF 분포 함수를 얻는 함수.
// 특정 컬러 영상(얼굴 혹은 입술 등)을 입력으로 하여
// 평균, 분산 등을 구하는 함수이다.
void CFaceTool::getPDF()
{
	int i,j,k;
	double sum[3];
	double mean[3];
	double sig[3];

	for(i=0;i<3;i++)
		sum[i] = mean[i] = sig[i] = 0.0;

	for(i=0;i<w;i++)
		for(j=0;j<h;j++)
			for (k=0;k<3;k++)
				sum[k] += colorimage->getPixel(i,j,k);

	// Mean for each color space.
	for (i=0;i<3;i++)
		mean[i] = sum[i]/(w*h-1);

	for(i=0;i<3;i++)
		sum[i] = 0.0;

	for(i=0;i<w;i++)
		for(j=0;j<h;j++)
			for (k=0;k<3;k++)
				sum[k] += ((colorimage->getPixel(i,j,k)-mean[k])*(colorimage->getPixel(i,j,k)-mean[k]));

	// Covariance for each color space.
	for (i=0;i<3;i++)
		sig[i] = sum[i]/(w*h-1);

	char str[100];
	sprintf(str,"m1=%.0f, s1=%.2f, m2=%.0f, s2=%.2f, m3=%.0f, s3=%.2f", mean[0],sig[0],mean[1],sig[1],mean[2],sig[2]);
	AfxMessageBox(str);
}

// 주어진 영상을 PDF 영상으로 변환한다.
// 이때 주어진 평균, 분산 및 해당 컬러부공간 rindex, gindex를
// 이용한다.
double *CFaceTool::ImgPDF(double mr, double mg, double srr, double sgg, int rindex, int gindex)
{
	double *ImgPDFMat = new double[w*h];

	int i,j,k=0;
	for(j=0;j<h;j++)
		for(i=0;i<w;i++,k++)
		{
			ImgPDFMat[k] = ProbMf(mr,mg,srr,sgg,colorimage->getPixel(i,j,rindex),colorimage->getPixel(i,j,gindex));
		}

	GrayImage A(w,h);
	k=0;
	for(j=0;j<h;j++)
		for(i=0;i<w;i++,k++)
		{
			A.setPixel(i,j,(unsigned char)(ImgPDFMat[k]*256.0));
		}

//	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&A));

	return ImgPDFMat;
}

// LT가 아니라 PDF 값을 주어진 평균, 분산 및 컬러부공간에 대하여
// 구하고, 이것을 주어진 th 값으로 이진화하는 함수.
void CFaceTool::PDFSegment(double mr, double mg, double srr, double sgg, int rindex, int gindex, double th)
{
	double *ImgP;
	int i,j,k;
	BinaryImage A(w,h);

	ImgP = ImgPDF(mr,mg,srr,sgg,rindex,gindex);

	k=0;
	for(i=0;i<w;i++)
		for(j=0;j<h;j++,k++)
		{
			if (ImgP[k] >= th)
				A.setPixel(i,j,255);
			else
				A.setPixel(i,j,0);
		}

	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&A));

	delete []ImgP;
}

// LTSegmentAnalysis2 와 유사한 대용량(?) 영상 분석 함수.
// 실제로는 LT를 사용하는 것에 비해 그다지 좋은 것 같지 않다.
void CFaceTool::PDFSegmentAnalysis(double mr, double mg, double srr, double sgg, int rindex, int gindex)
{
	char str[100];
	double *ImgP;
	double th_tmp,Cr,SEr,NSEr,Sr;
	int i,j,k,ii;
	BinaryImage A(w,h);
	BinaryImage grImg;
	ColorImage Z;

//	grImg.load("gcolor_face_lip_close.bmp",TRAINING);

	// PDF ...
	// 2000.10.12
	double sum[3], sumMean[3], sumSig[3];
	double mean[3];
	double sig[3];

	double tPix = 0.0;
	for(i=0;i<3;i++)
		sumMean[i] = sumSig[i] = 0.0;

	for(ii=0;ii<10;ii++)
	{
		for(i=0;i<3;i++)
			sum[i] = mean[i] = sig[i] = 0.0;

		tPix = 0.0;
		// load color image
		sprintf(str,"sfacea%03d.bmp", ii*2);
		Z.load(str,TRAINING);
		ColorSpaceTransform rgb;
		rgb.setcolorsys(ColorSpaceTransform::rgb);
		rgb.fromtransform(Z);

		// load gr image
		sprintf(str,"gfacea%03d.bmp", ii*2);
		grImg.load(str,TRAINING);

		for(j=0;j<h;j++)
			for(i=0;i<w;i++)
			{
				if (grImg.getPixel(i,j) == 255)
				{
					tPix++;
					for (k=0;k<3;k++)
						sum[k] += Z.getPixel(i,j,k);
				}
			}

		// Mean for each color space.
		for (i=0;i<3;i++)
		{
			mean[i] = sum[i]/(tPix-1);
			sumMean[i] += mean[i];
		}

		for (i=0;i<3;i++)
			sum[i] = 0.0;

		for(j=0;j<h;j++)
			for(i=0;i<w;i++)
			{
				if (grImg.getPixel(i,j) == 255)
				{
					for (k=0;k<3;k++)
						sum[k] += ((Z.getPixel(i,j,k)-mean[k])*(Z.getPixel(i,j,k)-mean[k]));
				}
			}
		
		// Covariance for each color space.
		for (i=0;i<3;i++)
		{
			sig[i] = sum[i]/(tPix-1);
			sumSig[i] += sig[i];
		}
	}

	// Mean and Covariance for each color space.
	for (i=0;i<3;i++)
	{
		mean[i] = sumMean[i]/10.0;
		sig[i] = sumSig[i]/10.0;
	}

	/////////////////////


	FILE *fp;
	fp = fopen("d:\\matlabr11\\work\\pdfanal.dat","aw");

	//ImgP = ImgPDF(mr2,mg2,srr2,sgg2,rindex,gindex);
	ImgP = ImgPDF(mean[rindex],mean[gindex],sig[rindex],sig[gindex],rindex,gindex);

	th_tmp = 0.0;

	do {
		
		k=0;
		for(j=0;j<h;j++)
			for(i=0;i<w;i++,k++)
			{
				if (ImgP[k] >= th_tmp)
					A.setPixel(i,j,255);
				else
					A.setPixel(i,j,0);
			}

		grTruthChk(grImg,A,Cr,SEr,NSEr,Sr);

		fprintf(fp,"%f %f %f %f %f\n",th_tmp,Cr,SEr,NSEr,Sr);
		
		th_tmp += 0.01;
	} while (th_tmp <= 1.0);

	fclose(fp);

	delete []ImgP;
}

// GG 특징을 얻어내는 함수.
// 특정 영상(입 영상 등)이 주어질 경우,
// gabor 필터링을 행하고 GG 특징을 구한다.
double CFaceTool::getfG(GrayImage A,double wave)
{
	int i;
	double mG, sG, fG;
	double wSum, wGSum;
	int th;

	GaborFilter gabor;
	double *gaborImg = gabor.gaborCo(A,wave,90.0);

	//CImgMisc::Normalize(gaborImg,A.w(),A.h());

	double max = -1;
	for(i=0;i<A.w()*A.h();i++)
		if (gaborImg[i] > max)
			max = gaborImg[i];

	Hist1d hist(65536, -1, 256);

	hist.addtohist(A.buf[0], A.w()*A.h());
	//hist.DJotsu(&th0, &th1);
	hist.DJotsu(&th);

	//double th = max*(0.2);
	for(i=0;i<A.w()*A.h();i++)
	{
		if (gaborImg[i] > th)
			gaborImg[i] = 1.0;
		else
			gaborImg[i] = 0.0;
	}

	//CImgMisc::Normalize(gaborImg,A.w(),A.h());

	double *proj = gabor.projCo(gaborImg,A.w(),A.h(),1);

	/*
	FILE *fp;
	fp = fopen("proj1.dat","aw");

	for(i=0;i<A.h();i++)
		fprintf(fp, "%f ", proj[i]);

	fclose(fp);
	*/

	double *dproj = new double[A.h()];
	for(i=1;i<A.h();i++)
		dproj[i] = fabs(proj[i]-proj[i-1]);

	double *wG = new double[A.h()];
	mG = (A.h()/2.0);
	sG = (A.h()/4.0)*(A.h()/4.0);
	wSum = wGSum = 0.0;
	for(i=1;i<A.h();i++)
	{
		wG[i] = exp(-(i-mG)/sG);
		wSum += wG[i];
		wGSum += wG[i]*dproj[i];
	}

	fG = wGSum/wSum;

	/*
	CString str;
	str.Format("fG = %.2f", fG);
	AfxMessageBox(str);
	*/

	delete [] wG;
	delete [] dproj;
	delete [] proj;
	delete [] gaborImg;

	return fG;
}

// 2000.10.11
// findFace와 유사한 함수.
// findFace가 뜻대로 동작하지 않아서 작성한 함수로 추정됨.
void CFaceTool::findFaceDJ(BinaryImage irb, int *x1, int *x2, int *y1, int *y2)
{
	int i;

	// face region finding...
	// x,y projection based...
	int margin = 10;
	double skin_th = 0.2;

	int *XProj = irb.proj(BinaryImage::X,margin,margin,irb.w()-margin,irb.h()-margin);
	int *YProj = irb.proj(BinaryImage::Y,margin,margin,irb.w()-margin,irb.h()-margin);

	ProfileData XData(irb.w()-2*margin+1,XProj);
	ProfileData YData(irb.h()-2*margin+1,YProj);

	int XMax = (int)(XData.getMax()*(skin_th));
	int YMax = (int)(YData.getMax()*(skin_th));

	int exit = 0;

	exit = 0;

	for(i=margin;i<irb.w()-margin;i++)
		if (XProj[i-margin] > XMax)
		{
			*x1 = i;
			exit = 1;
			break;
		}

	if (exit ==0)
		*x1 = margin;

	exit = 0;
	for(i=irb.w()-margin;i>*x1;i--)
		if (XProj[i-margin] > XMax)
		{
			*x2 = i;
			exit = 1;
			break;
		}

	if(exit ==0)
		*x2 = irb.w()-margin;

	exit = 0;
	for(i=margin;i<irb.h()-margin;i++)
		if (YProj[i-margin] > YMax)
		{
			*y1 = i;
			exit = 1;
			break;
		}

	if (exit ==0)
		*y1 = margin;

	exit = 0;
	for(i=irb.h()-margin;i>*y1;i--)
		if (YProj[i-margin] > YMax)
		{
			*y2 = i;
			exit = 1;
			break;
		}

	if(exit ==0)
		*y2 = irb.h()-margin;

	delete [] XProj;
	delete [] YProj;
}

// 아마도 최초로 만든 입술 찾기 함수인 것으로 추정.
// 피부색이 아닌 영역을 이용하여 입술을 찾는 함수.
// 그다지 신통치 않은 듯...
// 아무래도 수평 에지를 이용해야 할 것 같다.
int *CFaceTool::findMouthDJ(BinaryImage irb, int x1, int x2, int y1, int y2)
{
	int *co = new int[4],i,j;

	int *XProj, *YProj;
	int XMax, YMax, exit;

	// center point for x,y
	int x3 = (x1+x2)/2;
	int y3 = (y1+y2)/2;

	// lip region finding...
	// x,y projection based...

	// first, y projection used for finding height of lip
	YProj = irb.proj(BinaryImage::Y,x1,y3,x2,y2);
	ProfileData YData2(y2-y3+1,YProj);
	YMax = (int)(YData2.getMax()*(0.6));

	int x4,x5,y4,y5;//,k;
	int yc = (y3+y2)/2;

	exit = 0;
	for(i=y3;i<y2;i++)
		if (YProj[i-y3]>YMax)
		{
			for(j=i+1;j<y2;j++)
				if (YProj[j-y3]<YMax)
				{
					y4 = j;
					exit = 1;
					break;
				}
			if (exit == 1)
				break;
		}

	if (exit == 0)
		y4 = (int)(yc - (yc-y3)*0.2);

	exit = 0;
	for(i=y4;i<y2;i++)
		if (YProj[i-y3]>YMax)
		{
			y5 = i;
			exit = 1;
			break;
		}

	// there are no lip???
	if (exit == 0)
	{
		AfxMessageBox("Can't find lip height");
		y5 = (int)(yc + (yc-y3)*0.2);
		
	}

	// x projection
	XProj = irb.proj(BinaryImage::X,x1,y4,x2,y5);
	ProfileData XData2(x2-x1+1,XProj);
	XMax = (int)(XData2.getMax()*(0.5));

	exit = 0;
	for(i=x1;i<x3;i++)
		if (XProj[i-x1]>XMax)
		{
			for(j=i+1;j<x3;j++)
				if (XProj[j-x1]<XMax)
				{
					x4 = j;
					exit = 1;
					break;
				}
			if (exit == 1)
				break;
		}

	// can't find left of lip
	if (exit == 0)
		x4 = (int)(x1+(int)(x3-x1)*(0.7));

	exit = 0;
					
	for(i=2*x3-x4;i<x2;i++)
		if (XProj[i-x1]>XMax)
		{
			x5 = i;
			exit = 1;
			break;
		}

	if (exit == 0)
	{
		AfxMessageBox("Can't find lip length");
		x5 = (int)(x3+(int)(x2-x3)*(0.3));
		//return -1;
	}

	co[0] = x4;
	co[1] = y4;
	co[2] = x5;
	co[3] = y5;

	delete [] XProj;
	delete [] YProj;

	return co;
}

// findMouthDJ를 개선(?)시켜 만든 것으로 추정.
// 무슨 차이가 있는지?
// 역시 개선의 여지가 다분함.
int *CFaceTool::findMouthDJ2(BinaryImage irb, int x1, int x2, int y1, int y2)
{
	int *co = new int[4],i,j,margin = 10;
	double mouthYth = 0.7;
	double mouthXth = 0.7;

	int *XProj, *YProj;
	int XMax, YMax, exit;

	// center point for x,y
	int x3 = (x1+x2)/2;
	int y3 = (y1+y2)/2;

	/*
	for(j=0;j<irb.h();j++)
		for(i=0;i<irb.w();i++)
			if (irb.getPixel(i,j))
				irb.setPixel(i,j,0);
			else
				irb.setPixel(i,j,255);

			CImgMisc::ShowImg(CImgMisc::ImgConvB_c(&irb));
*/
	// lip region finding...
	// x,y projection based...

	// first, y projection used for finding height of lip
	YProj = irb.proj(BinaryImage::Y,x1,y3,x2,y2);
	ProfileData YData2(y2-y3+1,YProj);
	YMax = (int)(YData2.getMax()*(mouthYth));

	int x4,x5,y4,y5;//,k;
	int yc = (y3+y2)/2;

	exit = 0;
	for(i=y3;i<y2;i++)
		if (YProj[i-y3]>YMax)
		{
			for(j=i+1;j<y2;j++)
				if (YProj[j-y3]<YMax)
				{
					y4 = j;
					exit = 1;
					break;
				}
			if (exit == 1)
				break;
		}

	if (exit == 0)
		y4 = (int)(yc - (yc-y3)*0.2);

	exit = 0;
	for(i=y4+margin;i<y2;i++)
		if (YProj[i-y3]>YMax)
		{
			y5 = i;
			exit = 1;
			break;
		}

	// there are no lip???
	if (exit == 0)
	{
		AfxMessageBox("Can't find lip height");
		y5 = (int)(yc + (yc-y3)*0.2);
		
	}

	// x projection
	XProj = irb.proj(BinaryImage::X,x1,y4,x2,y5);
	ProfileData XData2(x2-x1+1,XProj);
	XMax = (int)(XData2.getMax()*(mouthXth));

	exit = 0;
	for(i=x1;i<x3;i++)
		if (XProj[i-x1]>XMax)
		{
			for(j=i+1;j<x3;j++)
				if (XProj[j-x1]<XMax)
				{
					x4 = j;
					exit = 1;
					break;
				}
			if (exit == 1)
				break;
		}

	// can't find left of lip
	if (exit == 0)
		x4 = (int)(x1+(int)(x3-x1)*(0.7));

	exit = 0;
					
	//for(i=2*x3-x4;i<x2;i++)
	for(i=x2;i>x3;i--)
		if (XProj[i-x1]>XMax)
		{
			for(j=i-1;j>x3;j--)
				if (XProj[j-x1]<XMax)
				{
					x5 = j;
					exit = 1;
					break;
				}
			if (exit == 1)
				break;
		}

	if (exit == 0)
	{
		AfxMessageBox("Can't find lip length");
		x5 = (int)(x3+(int)(x2-x3)*(0.3));
		//return -1;
	}

	co[0] = x4;
	co[1] = y4;
	co[2] = x5;
	co[3] = y5;

	delete [] XProj;
	delete [] YProj;

	return co;
}

// 컬러 분할을 위한 Lookup Table 을 구성하고,
// 그 LT를 이용하여 실제 테스트 영상에 대한 컬러 분할을 행한다.
// 이 경우 LT는 다시 [0.0 - 1.0] 으로 평준화된다.
// 이 때, 임계치를 0.1 단위로 바꾸어 가면서 컬러 분할을 행함.
void CFaceTool::LTSegmentAnalysis(int rindex,int gindex)
{
	int i,j,k;
	double max = -1;

	// Training stage.
	// load ground truth image
	BinaryImage A;
	A.load("gcolor_face_lip_close.bmp", TRAINING);

	double LT[256][256];
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			LT[i][j] = 0.0;

	// make LT. using color information.
	for(j=0;j<h;j++)
		for(i=0;i<w;i++)
		{
			if (A.getPixel(i,j) == 255)
			{
				LT[colorimage->getPixel(i,j,rindex)][colorimage->getPixel(i,j,gindex)]++;
				if (LT[colorimage->getPixel(i,j,rindex)][colorimage->getPixel(i,j,gindex)] >= max)
					max = LT[colorimage->getPixel(i,j,rindex)][colorimage->getPixel(i,j,gindex)];
			}
		}

	// Normalize LT
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			LT[i][j] /= max;
	
	GrayImage B(256,256);
	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			B.setPixel(i,j,(unsigned char)(LT[i][j]*255.0));

	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&B));

	// Testing stage.
	// skin detection by LT.
	double *CLT = new double[w*h];
	k=0;
	for(j=0;j<h;j++)
		for(i=0;i<w;i++,k++)
		{
			CLT[k] = LT[colorimage->getPixel(i,j,rindex)][colorimage->getPixel(i,j,gindex)];
		}

		CImgMisc::Normalize(CLT,w,h);

	FILE *fp;
	fp = fopen("d:\\matlabr11\\work\\ltanal.dat","aw");

	double th_tmp = 0.0;
	BinaryImage C(w,h);
	double Cr,SEr,NSEr,Sr;

	do {
		
		k=0;
		for(j=0;j<h;j++)
			for(i=0;i<w;i++,k++)
			{
				if (CLT[k] >= th_tmp)
					C.setPixel(i,j,255);
				else
					C.setPixel(i,j,0);
			}

		grTruthChk(A,C,Cr,SEr,NSEr,Sr);

		fprintf(fp,"%f %f %f %f %f\n",th_tmp,Cr,SEr,NSEr,Sr);
		
		th_tmp += 0.01;
	} while (th_tmp <= 1.0);

	fclose(fp);

	/*
	double PSkin, PNonSkin;
	double sumPSkin, sumPNonSkin;
	double sumSkin=0.0, sumNonSkin=0.0;

	for(j=0;j<h;j++)
		for(i=0;i<w;i++)
		{
			PSkin = 0.0;
			PNonSkin = 0.0;
			if (A.getPixel(i,j) == 255)
			{
				sumSkin++;

			}
			else
			{
				sumNonSkin++;
			}
		}
		*/

	delete []CLT;
}

// LTSegmentAnalysis 의 대용량 영상(?) 버전.
// 설정된 영상에 대하여 분석을 행한다.
// 컬러공간 비교를 위해서 필수적인 함수.
// 조만간 CRL_SKIN 영상을 갖고 한번 테스트해 볼 필요가 있음.
// 20010610@19:48@DJ
void CFaceTool::LTSegmentAnalysis2(int rindex,int gindex)
{
	LTSegmentAnalysis3(0, 1, "rgb", ColorSpaceTransform::rgb);
	LTSegmentAnalysis3(0, 2, "HLS", ColorSpaceTransform::HLS);
	LTSegmentAnalysis3(1, 2, "I1I2I3", ColorSpaceTransform::I1I2I3);
	LTSegmentAnalysis3(1, 2, "YCrCb", ColorSpaceTransform::YCRCB);
	LTSegmentAnalysis3(1, 2, "I1I2I3NN", ColorSpaceTransform::I1I2I3NN);
	
	// 20040217
	// r-b space
	LTSegmentAnalysis3(0, 2, "rb", ColorSpaceTransform::rb);

	// r-g space
	LTSegmentAnalysis3(0, 1, "rg", ColorSpaceTransform::rg);

	// a-b space
	//LTSegmentAnalysis3(0, 1, "ab", ColorSpaceTransform::ab);
	

	//LTSegmentAnalysis3(1, 2, "I1I2I3N7", ColorSpaceTransform::I1I2I3N);
	//LTSegmentAnalysis3(rindex, gindex, "I1I2I3N7", ColorSpaceTransform::I1I2I3N);
	//LTSegmentAnalysis3(rindex, gindex, "I1I2I3NN7", ColorSpaceTransform::I1I2I3NN);
	//LTSegmentAnalysis3(rindex, gindex, "I1I2I3N37", ColorSpaceTransform::I1I2I3N3);
}

// LTSegmentAnalysis 의 대용량 영상(?) 버전.
// 설정된 영상에 대하여 분석을 행한다.
// 컬러공간 비교를 위해서 필수적인 함수.
// 조만간 CRL_SKIN 영상을 갖고 한번 테스트해 볼 필요가 있음.
// 20010610@19:48@DJ
// speed-up!
// 20020524 IPL version by DJ
// 20040217
void CFaceTool::LTSegmentAnalysis3(int rindex,int gindex, char pstr[], ColorSpaceTransform::ColorSys sys)
{
	int i,j,k,ii, l;
	// 20040217
	int sample_no = 26;
	int test_no = 13;
	int test_offset = 13;

	double max = -1;
	char str[100];
	BinaryImage A;
	ColorImage Z;
	double LT[256][256];
	long imgSize;

	double *CLT;
	BinaryImage C;
	double th_tmp = 0.0;
	double Cr,SEr,NSEr,Sr;

	FILE *fp;
	sprintf(str, "c:\\matlabr11\\work\\ferec\\lt_%s.dat", pstr);
	fp = fopen(str,"aw");
	//FILE *fp2;
	//fp2 = fopen("K:\\User\\research\\faceDB\\training\\myCS\\skin_list.dat", "rt");
	ColorSpaceTransform rgb, rgb2;
	rgb.setcolorsys(sys);
	rgb2.setcolorsys(sys);

	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			LT[i][j] = 0.0;

	// Training stage.
	for(ii=0; ii<6; ii++)
	{
		l = ii*2;

		//fscanf(fp2, "%s", str);

		// load training image.
		//if (strstr(str, "jpg"))
			sprintf(str,"K:\\User\\research\\faceDB\\training\\myCS\\sfacea%03d.bmp", l);
		//else if (strstr(str, "gif"))
		//	sprintf(str,"K:\\User\\research\\faceDB\\training\\myCS\\skin%04d.gif", l);
		Z.load(str, 100);

		// load ground truth image.
		sprintf(str,"K:\\User\\research\\faceDB\\training\\myCS\\gfacea%03d.bmp", l);
		A.load(str, 100);

		imgSize = Z.w()*Z.h();

//		ColorSpaceTransform rgb;
//		rgb.setcolorsys(ColorSpaceTransform::I1I2I3N);
		rgb2.fromtransform(Z);

		// make LT. using color information.
		for (k=0; k<imgSize; k++)
		{
			if (A.buf[0][k] == 255)
			{
				LT[Z.buf[rindex][k]][Z.buf[gindex][k]]++;

				if (LT[Z.buf[rindex][k]][Z.buf[gindex][k]] >= max)
					max = LT[Z.buf[rindex][k]][Z.buf[gindex][k]];
			}
		}
	}

	//fclose(fp2);

	//FILE *fp3;
	//sprintf(str, "c:\\matlabr11\\work\\lt_%s.dat", pstr);
	//fp3 = fopen(str,"aw");

	// Normalized LT
	for(i=0;i<256;i++)
	{
		for(j=0;j<256;j++)
		{
			//LT[i][j] /= max;
			if (LT[i][j] > 1)
				LT[i][j] = (float)log(LT[i][j])/(float)log(max);
			else
				LT[i][j] = 0.0;

			//fprintf(fp3, "%.3f ", LT[i][j]);
		}

		//fprintf(fp3, "\n");
	}

	//fclose(fp3);

	// Test stage.
	//fp2 = fopen("K:\\User\\research\\faceDB\\training\\myCS\\skin_list.dat", "rt");

	//for (i=0; i<test_offset; i++)
	//	fscanf(fp2, "%s", str);

	for(ii=0; ii<20; ii++)
	{
		l = 12 + ii*2;

		//fscanf(fp2, "%s", str);

		// load training image.
		//if (strstr(str, "jpg"))
			sprintf(str,"K:\\User\\research\\faceDB\\training\\myCS\\sfacea%03d.bmp", l);
		//else if (strstr(str, "gif"))
		//	sprintf(str,"e:\\facedb\\crl_skin_db\\skin-images\\skin%04d.gif", l);
		Z.load(str, 100);

		// load ground truth image.
		sprintf(str,"K:\\User\\research\\faceDB\\training\\myCS\\gfacea%03d.bmp", l);
		A.load(str, 100);

		imgSize = Z.w()*Z.h();
		CLT = new double[imgSize];
		C = A;

//		ColorSpaceTransform rgb;
//		rgb.setcolorsys(ColorSpaceTransform::I1I2I3N);
		rgb.fromtransform(Z);

		for (k=0; k<imgSize; k++)
			CLT[k] = LT[Z.buf[rindex][k]][Z.buf[gindex][k]];

		//CImgMisc::Normalize(CLT,Z.w(),Z.h());

		th_tmp = 0.0;

		do {
			
			for (k=0; k<imgSize; k++)
			{
				if (CLT[k] >= th_tmp)
					C.buf[0][k] = 255;
				else
					C.buf[0][k] = 0;
			}

			grTruthChk(A, C, Cr, SEr, NSEr, Sr);

			fprintf(fp,"%.3f %.3f %.3f %.3f %.3f\n", th_tmp, Cr, SEr, NSEr, Sr);
			
			th_tmp += 0.01;
		} while (th_tmp <= 1.0);

		delete []CLT;
	}

	fclose(fp);
	//fclose(fp2);
}

// 얼굴 영상에 대하여 PCA 과정을 적용하고,
// 변환 계수를 구하는 함수이다.
// 먼저 주어진 얼굴 영상에 대하여 ground 영상을
// AND시킨 다음, 평균 및 분산 등을 구한다.
// 20010610@18:48@DJ
// speed-up!
// sample_no 값으로 training시킬 영상의 수를 결정할 수 있다.
// PCA 계수를 normalize하는 부분이 필요하다.
void CFaceTool::faceColor()
{
	int i,j,ii, k, l;
	double max = -1;
	const int sample_no = 100;
	char str[100];
	long imgSize;
	
	// A: ground image
	BinaryImage A;
	// Z: raw image
	ColorImage Z;
	// segImg: segmented image
	//ColorImage segImg;
	
	// mean
	double mR, mG, mB, m[sample_no][3];
	// covariance
	double dRR, dRG, dRB, dGR, dGG, dGB, dBR, dBG, dBB, d[sample_no][9];
	// no. of skin pixel
	double noSkin = 0;
	// diff. of mean and sample
	double diffR, diffG, diffB;

	mR = mG = mB = dRR = dRG = dRB = dGR = dGG = dGB = dBR = dBG = dBB = 0.0;

	for (i=0; i<sample_no; i++)
	{
		for (j=0; j<3; j++)
			m[i][j] = 0.0;
		for (j=0; j<9; j++)
			d[i][j] = 0.0;
	}

	FILE *fp;
	fp = fopen("f:\\facedb\\crl_skin_db\\skin-list-good-dj", "rt");

	// Training stage.
	for (ii=0; ii<sample_no; ii++)
	{
		l = ii + 1;

		noSkin = 0.0;

		fscanf(fp, "%s", str);
		// load training image.
		if (strstr(str, "jpg"))
			sprintf(str,"f:\\facedb\\crl_skin_db\\skin-images\\skin%04d.jpg", l);
		else if (strstr(str, "gif"))
			sprintf(str,"f:\\facedb\\crl_skin_db\\skin-images\\skin%04d.gif", l);
		Z.load(str, 100);

		// load ground truth image.
		sprintf(str,"f:\\facedb\\crl_skin_db\\masks\\mask%04d.tif", l);
		A.load(str, 100);

		imgSize = Z.w()*Z.h();

		// get mean for R,G,B
		k = 0;
		while (k < imgSize)	{
			if (A.buf[0][k] == 255)
			{
				noSkin += 1.0;
				m[ii][0] += Z.buf[0][k];
				m[ii][1] += Z.buf[1][k];
				m[ii][2] += Z.buf[2][k];
			}
			k++;
		}

		// real mean for R,G,B
		m[ii][0] /= noSkin;
		m[ii][1] /= noSkin;
		m[ii][2] /= noSkin;

		// get covariance for R,G,B
		k = 0;
		while (k < imgSize)	{

			if (A.buf[0][k] == 255)
			{
				diffR = (Z.buf[0][k]-m[ii][0]);
				diffG = (Z.buf[1][k]-m[ii][1]);
				diffB = (Z.buf[2][k]-m[ii][2]);

				d[ii][0] += (diffR*diffR);
				d[ii][1] += (diffR*diffG);
				d[ii][2] += (diffR*diffB);
				d[ii][3] += (diffG*diffR);
				d[ii][4] += (diffG*diffG);
				d[ii][5] += (diffG*diffB);
				d[ii][6] += (diffB*diffR);
				d[ii][7] += (diffB*diffG);
				d[ii][8] += (diffB*diffB);
			}

			k++;
		}

		// real cov. for R,G,B
		d[ii][0] /= noSkin;
		d[ii][1] /= noSkin;
		d[ii][2] /= noSkin;
		d[ii][3] /= noSkin;
		d[ii][4] /= noSkin;
		d[ii][5] /= noSkin;
		d[ii][6] /= noSkin;
		d[ii][7] /= noSkin;
		d[ii][8] /= noSkin;

	}

	fclose(fp);

	for(i=0; i<sample_no; i++)
	{
		mR += m[i][0];
		mG += m[i][1];
		mB += m[i][2];

		dRR += d[i][0];
		dRG += d[i][1];
		dRB += d[i][2];
		dGR += d[i][3];
		dGG += d[i][4];
		dGB += d[i][5];
		dBR += d[i][6];
		dBG += d[i][7];
		dBB += d[i][8];
	}

	// final mean for RGB
	mR /= (float)sample_no;
	mG /= (float)sample_no;
	mB /= (float)sample_no;

	// final cov. for RGB
	dRR /= (float)sample_no;
	dRG /= (float)sample_no;
	dRB /= (float)sample_no;
	dGR /= (float)sample_no;
	dGG /= (float)sample_no;
	dGB /= (float)sample_no;
	dBR /= (float)sample_no;
	dBG /= (float)sample_no;
	dBB /= (float)sample_no;

	//CAlgebra::
	float **a, *r, **v;
	int nrot;

	int n = 3;
	a = CAlgebra::mat_new_forRecipe(n,n);
	v = CAlgebra::mat_new_forRecipe(n,n);
	r = CAlgebra::vec_new_forRecipe(n);

	a[1][1] = (float)dRR;
	a[1][2] = (float)dRG;
	a[1][3] = (float)dRB;
	a[2][1] = (float)dGR;
	a[2][2] = (float)dGG;
	a[2][3] = (float)dGB;
	a[3][1] = (float)dBR;
	a[3][2] = (float)dBG;
	a[3][3] = (float)dBB;

	CAlgebra::jacobi(a,n,r,v,&nrot);
	CAlgebra::eigsrt(r,v,n);

	double sum[3] = {0.0, 0.0, 0.0};

	for (i=0; i<3; i++)
	{
		sum[0] += fabs(v[i+1][1]);
		sum[1] += fabs(v[i+1][2]);
		sum[2] += fabs(v[i+1][3]);
	}

	for (i=0; i<3; i++)
	{
		v[i+1][1] /= (float)sum[0];
		v[i+1][2] /= (float)sum[1];
		v[i+1][3] /= (float)sum[2];
	}

	CString str2;
	str2.Format("%5.3f,%5.3f,%5.3f,%5.3f,%5.3f,%5.3f,%5.3f,%5.3f,%5.3f",v[1][1],v[2][1],v[3][1],v[1][2],v[2][2],v[3][2],v[1][3],v[2][3],v[3][3]);
	AfxMessageBox(str2);

	//CAlgebra::
}

// 얼굴 분할을 위한 함수이다.
// faceSegment와 유사하게 컬러공간 colorSpace에 대하여
// 주어진 rindex, gindex에 해당하는 컬러부공간을 각 임계치로
// 이진화한 다음, AND 연산을 통해 얼굴 분할을 행한다.
void CFaceTool::faceSegment2(int colorSpace, int rindex, int gindex, int range1Min, int range1Max, int range2Min, int range2Max)
{
	ColorSpaceTransform rgb;

	ColorImage color(*colorimage);

	switch(colorSpace)
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
		default:
			break;
	}
		
	rgb.fromtransform(color);

	GrayImage A[3];
	color.to3Gray(&A[0], &A[1], &A[2]);

	BinaryImage B[2];

	//B[0] = A[rindex].threshold(160,205);
	//B[1] = A[gindex].threshold(104,131);

	//B = CR.threshold(137,188);
	//C = CB.threshold(104,134);

	B[0] = A[rindex].threshold(range1Min,range1Max);
	B[1] = A[gindex].threshold(range2Min,range2Max);

	BinaryImage C(B[0]);

	C = B[0] & B[1];

	ImageLabeling label(&C);

	GrayImage *labeled;
	labeled = label.labeling();

	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(labeled));

	ProfileData area_prof(label.getLength(),label.getAreas());
	int ns[256], n;
	area_prof.getMax(ns, &n);
	if (n != 1) {
		printf ("warning: max area too many: %d\n", n);
	}
	BinaryImage *hair_rejected_bin = label.select(0, (int)label.getArea(ns[0]),//1, (int)label.getArea(ns[0]),// - 1, 
									ImageLabeling::Area, 
									ImageLabeling::OutOfBound);

	CImgMisc::ShowImg(CImgMisc::ImgConvB_c(hair_rejected_bin));

	//delete labeled;

	for(int j=0;j<C.h();j++)
		for(int i=0;i<C.w();i++)
		{
			if (hair_rejected_bin->getPixel(i,j) == 255)
				color.setPixel(i,j,colorimage->getPixel(i,j,0),colorimage->getPixel(i,j,1),colorimage->getPixel(i,j,2));
			else
				color.setPixel(i,j,0,0,0);
		}

	CImgMisc::ShowImg(CImgMisc::ImgConvC_c(&color));
}

// 2000. 12. 5
// DJ
// func : colorSegment
// sys : any pre-defined color space in ColorSpaceTransform
// th[][0] - th[][1] : threshold for each color space
// th[][2] : threshold condition (InBound, OutOfBound, UnUsed)
// oper : operation between each image of color space images ('&', '|')
// 주어진 컬러영상 sImg에 대하여 주어진 컬러공간 sys로 변환을 하고,
// 주어진 th값을 이용하여 이진화를 한 다음, 주어진 oper을 이용하여
// 연산을 행한 결과값을 dImg로 돌려준다.
// 왜 만들었을까?... -.-
void CFaceTool::faceSegment(ColorImage sImg, BinaryImage *dImg, ColorSpaceTransform::ColorSys sys, int th[3][3], unsigned char oper)
{
	GrayImage A[3];
	ColorImage B(sImg);

	ColorSpaceTransform colorTransform(sys);	
	colorTransform.fromtransform(B);

	B.to3Gray(&A[0], &A[1], &A[2]);

	BinaryImage *C = new BinaryImage[3];
	for(int i=0; i<3; i++)
		if (th[i][2] != 2)
		{
			if (th[i][2] == 0)
				C[i] = A[i].threshold(th[i][0], th[i][1], GrayImage::InBound);
			else if (th[i][2] == 1)
				C[i] = A[i].threshold(th[i][0], th[i][1], GrayImage::OutOfBound);
						
		}

	BinaryImage D(B.w(), B.h());

	for(int j=0; j<D.h(); j++)
		for(i=0; i<D.w(); i++)
		{
			if (oper == '&')
				D.setPixel(i, j, 255);
			else if (oper == '|')
				D.setPixel(i, j, 0);
		}

	for(i=0; i<3; i++)
		if (th[i][2] != 2)
		{
			if (oper == '&')
				D = D & C[i];
			else if (oper == '|')
				D = D | C[i];
		}

	delete []C;

	*dImg = D;
}

// 20010306@19:58p@DJ
// 눈이랑 눈썹의 수직 좌표를 구하는 함수이다.
// eyeExtract2 와 혼동할 염려가 있다.
// est_num : project된 blob의 수를 의미함.
// blob의 수가 1인 경우... 눈만 찾은 것으로 판단.
void CFaceTool::eyesExtract(int *proj, int projNum, CRect *eyeRect, CRect *eyebrRect)
{
	int i, j;
	int est_num = 0, est_num2 = 0;
	int flageye = 0, flageyebrow = 0;

	CFaceSegment *segment;
//	CString str;

	// first scan
	for (i=0; i<projNum; i++)
	{
		if (proj[i] != 0 && i == 0)
			est_num++;
		else if (i != 0)
		{
			if (proj[i] != 0 && proj[i-1] == 0)
				est_num++;
		}
	}

	segment = new CFaceSegment[est_num];

	// second scan
	for (i=0; i<projNum; i++)
	{
		if (proj[i] != 0 && i == 0)
		{
			segment[est_num2].start = i;
			segment[est_num2].sum += proj[i];
		}
		else if (i != 0)
		{
			if (proj[i] != 0 && proj[i-1] == 0)
			{
				segment[est_num2].sum += proj[i];
				segment[est_num2].start = i;
			}
			else if (proj[i-1] != 0 && proj[i] == 0)
			{
				segment[est_num2].end = i-1;
				est_num2++;
			}
			else if (proj[i] != 0)
			{
				segment[est_num2].sum += proj[i];
			}
		}
	}

	if (proj[i-1] != 0)
		segment[est_num2].end = i-1;

	for (i=0; i<est_num; i++)
	{
		segment[i].length = segment[i].end - segment[i].start + 1;
//		str.Format("%d %d %d %d", segment[i].start, segment[i].end, segment[i].length, segment[i].sum);
//		AfxMessageBox(str);
	}	

	for (i=0; i<est_num; i++)
	{
		if (segment[i].length > projNum/10)
		{
			eyebrRect->top = segment[i].start;
			eyebrRect->bottom = segment[i].end;

			flageye = 1;

			if (est_num > 1)
			{
				for (j=i+1; j<est_num; j++)
				{
					if (segment[j].length > projNum/10)
					{
						eyeRect->top = segment[j].start;
						eyeRect->bottom = segment[j].end;

						flageyebrow = 1;

						break;
					}
				}
			}

			break;
		}
	}

	if (flageye == 0)
	{
		eyebrRect->top = 0;
		eyebrRect->bottom = projNum/4;
	}
	
	if (flageyebrow == 0)
	{
		eyeRect->top = projNum*3/4;
		eyeRect->top = projNum;
	}

	delete []segment;
}

// 20010309@0:57p@DJ
// 눈 위치를 추정해 내는 프로그램이다.
// 얼굴 상위 영역에 대하여 수평 투영을 행한 결과를 갖고,
// grouping 과정을 통하여 눈의 좌우 정보 등을 얻는다.
void CFaceTool::eyeExtract2(int *proj, int projNum, CRect *eyeRectr, CRect *eyeRectl)
{
	int i, j;
	int est_num = 0, est_num2 = 0;
	CFaceSegment *segment;
//	CString str;

	// first scan
	for (i=0; i<projNum; i++)
	{
		if (proj[i] != 0 && i == 0)
			est_num++;
		else if (i != 0)
		{
			if (proj[i] != 0 && proj[i-1] == 0)
				est_num++;
		}
	}

	segment = new CFaceSegment[est_num];

	// second scan
	for (i=0; i<projNum; i++)
	{
		if (proj[i] != 0 && i == 0)
		{
			segment[est_num2].start = i;
			segment[est_num2].sum += proj[i];
		}
		else if (i != 0)
		{
			if (proj[i] != 0 && proj[i-1] == 0)
			{
				segment[est_num2].sum += proj[i];
				segment[est_num2].start = i;
			}
			else if (proj[i-1] != 0 && proj[i] == 0)
			{
				segment[est_num2].end = i-1;
				est_num2++;
			}
			else if (proj[i] != 0)
			{
				segment[est_num2].sum += proj[i];
			}
		}
	}

	if (proj[i-1] != 0)
		segment[est_num2].end = i-1;

	for (i=0; i<est_num; i++)
	{
		segment[i].length = segment[i].end - segment[i].start + 1;
//		str.Format("%d %d %d %d", segment[i].start, segment[i].end, segment[i].length, segment[i].sum);
//		AfxMessageBox(str);
	}	

	for (i=0; i<est_num; i++)
	{
		if (segment[i].length > projNum/10)
		{
			eyeRectl->left = segment[i].start;
			eyeRectl->right = segment[i].end;

			for (j=i+1; j<est_num; j++)
			{
				if (segment[j].length > projNum/10)
				{
					eyeRectr->left = segment[j].start;
					eyeRectr->right = segment[j].end;

					break;
				}
			}

			break;
		}
	}

	delete []segment;
}

// 10.11.01
// Bremen, Germany
// phase: 0(find facial blob), 1(find facial component)
void CFaceTool::DJsimple_face_detection(GrayImage *A, ColorImage *B, int phase)
{
	ColorImage color(*B);
	ColorSpaceTransform rgb;
	rgb.setcolorsys(ColorSpaceTransform::I1I2I3N);
	rgb.fromtransform(color);

	GrayImage colorBand[3];
	color.to3Gray(&colorBand[0], &colorBand[1], &colorBand[2]);

	if (phase == 0)
	{
		colorBand[1].thresholdSelf(10, 64, GrayImage::InBound);

		*A = colorBand[1];
		return;
	}

	/*
	/////////////////////////////////////////////////////////////////////////////

	int i, j;
	int pnFilter[9] = {-2, -2, -2, 0, 0, 0, 2, 2, 2};
	unsigned char *histo = new unsigned char[A.w()*A.h()];
	int th0;//, th1;
	int div = 0;

	for(i=0 ; i<8 ; i++) div += pnFilter[i];

	GrayImage *C = A.convolution(3, 3, pnFilter, div);

	Hist1d hist(65536, -1, 256);

	hist.addtohist(C->buf[0], A.w()*A.h());
	hist.DJotsu(&th0);

	delete []histo;

	C->thresholdSelf(th0, 255);

	A = *C;
	delete C;

	if (phase == 1)
		return;

	int *HPvec = new int[A.h()];

	for (j=0; j<A.h(); j++)		HPvec[j] = 0;

	HPvec = C->proj(GrayImage::Y, 0, 0, A.w(), A.h());

	compExtract(HPvec, A.h(), *C);

	A.drawRect(facecomp.rEyeBrow.left, facecomp.rEyeBrow.top, 
		facecomp.rEyeBrow.right, facecomp.rEyeBrow.bottom, 255);
	A.drawRect(facecomp.lEyeBrow.left, facecomp.lEyeBrow.top, 
		facecomp.lEyeBrow.right, facecomp.lEyeBrow.bottom, 255);
	A.drawRect(facecomp.rEye.left, facecomp.rEye.top, 
		facecomp.rEye.right, facecomp.rEye.bottom, 255);
	A.drawRect(facecomp.lEye.left, facecomp.lEye.top, 
		facecomp.lEye.right, facecomp.lEye.bottom, 255);
	A.drawRect(facecomp.exMouth.left, facecomp.exMouth.top, 
		facecomp.exMouth.right, facecomp.exMouth.bottom, 255);
	
	//CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&A));
	A = *C;

	delete []HPvec;
	*/
}

// 20010307@1:27a@DJ
// 20010516@0520a
// A : return image
// B : current frame
// C : previous frame
// 현재 입력되는 B frame, 이전에 입력된 C frame을 이용하여
// 얼굴의 분할 영상을 얻는 프로그램이다.
// I1I2I3N 컬러 공간을 이용하며 motion 정보는 B, C frame의
// 밝기 정보를 이용하여 얻는다. 
// CHAT_mask 의 갱신 등에 주목하여 보강을 해야 할 듯... -.-
void CFaceTool::DJfaceSegment(GrayImage *A, ColorImage *B, ColorImage *C)
{
	int kernel[9] = {0, 1, 0, 1, 1, 1, 0, 1, 0};
	unsigned char flag; 
//	unsigned char *gMat = new unsigned char[B->w()*B->h()];
	int *hist1;
	int ns[256], n;

	ColorSpaceTransform rgb;
	ColorImage color(*B);
	GrayImage colorBand[3], D, E, F(B->w(), B->h());
	GrayImage CHAT_sub[2];
	//GrayImage *G1, *G2;
	BinaryImage binBand[2];
	BinaryImage *hair_rejected_bin;
	Hist1d hist(65536,-1,256);

	rgb.setcolorsys(ColorSpaceTransform::I1I2I3N);

	// motion detection
	//flag = DJmotion(B, C, &F, 0);

	*A = CHAT_mask;

	/*
	switch (flag)
	{
		// just return
	case 0:
		return;
		break;
		// init. CHAT procedure
	case 1:
	case 2:
	case 3:
		break;
	}
	*/

	unsigned char *gMat = new unsigned char[B->w()*B->h()];

		rgb.fromtransform(color);
		color.to3Gray(&colorBand[0], &colorBand[1], &colorBand[2]);
		binBand[0] = colorBand[1].threshold(faceTh[0][0], faceTh[0][1], GrayImage::OutOfBound);
		binBand[1] = colorBand[2].threshold(faceTh[1][0], faceTh[1][1]);

		D = binBand[0] & binBand[1];

		// delete boundary
		D.drawRect(0, 0, D.w()-1, D.h()-1, 0);

		// blob analysis
		ImageLabeling label(&D);

		GrayImage *labeled;
		labeled = label.labeling();

		ProfileData area_prof(label.getLength(),label.getAreas());

		area_prof.getMax(ns, &n);
		
		hair_rejected_bin = label.select(0, (int)label.getArea(ns[0]),//1, (int)label.getArea(ns[0]),// - 1, 
										ImageLabeling::Area, 
										ImageLabeling::OutOfBound);

		D = *hair_rejected_bin;

		// CHAT_mask
		CHAT_mask = CHAT_mask | D;

		CHAT_sub[0] = CHAT_mask & colorBand[1];
		CHAT_sub[1] = CHAT_mask & colorBand[2];

		CHAT_sub[0].copyTo(gMat, NULL, NULL);
		hist.addtohist(gMat, B->w()*B->h());
		hist1 = hist.gets();
		HistoAnal(hist1, 1, 0);

		CHAT_sub[1].copyTo(gMat, NULL, NULL);
		hist.addtohist(gMat, B->w()*B->h());
		hist1 = hist.gets();
		HistoAnal(hist1, 1, 1);

		delete []gMat;

		*A = D;

	/*
	// 1. color cues
	// color conversion
	ColorSpaceTransform rgb;
	ColorImage color(*B);
	rgb.setcolorsys(ColorSpaceTransform::I1I2I3N);
	rgb.fromtransform(color);
	// split channels
	color.to3Gray(&AB[0], &AB[1], &AB[2]);
	// threshold for each splitted channel
	BB[0] = AB[1].threshold(130, 190, GrayImage::OutOfBound);
	BB[1] = AB[2].threshold(110, 135);
	// logical AND
	D = BB[0] & BB[1];

	// delete boundary
	D.drawRect(0, 0, D.w()-1, D.h()-1, 0);

	// blob analysis
	ImageLabeling label(&D);

	GrayImage *labeled;
	labeled = label.labeling();

	ProfileData area_prof(label.getLength(),label.getAreas());

	int ns[256], n;
	area_prof.getMax(ns, &n);
	
	BinaryImage *hair_rejected_bin = label.select(0, (int)label.getArea(ns[0]),//1, (int)label.getArea(ns[0]),// - 1, 
									ImageLabeling::Area, 
									ImageLabeling::OutOfBound);

	D = *hair_rejected_bin;
	*/

	/*
	// CHAT_mask
	CHAT_mask = D;

	CHAT_sub[0] = CHAT_mask & AB[1];
	CHAT_sub[1] = CHAT_mask & AB[2];

	CHAT_sub[0].copyTo(gMat, NULL, NULL);
	hist.addtohist(gMat, B->w()*B->h());
	hist1 = hist.gets();
	HistoAnal(hist1, 1, 0);

	CHAT_sub[1].copyTo(gMat, NULL, NULL);
	hist.addtohist(gMat, B->w()*B->h());
	hist1 = hist.gets();
	HistoAnal(hist1, 1, 1);

	// 2. motion cues
	// LPF of each grabbed image
	/*
	if (C != NULL)
		G2 = C->getLumImg();
	else
		G2 = 0;
		*/
	//G2 = C->getLumImg();

	//DJmotion(B, C, &F, 0);
	
	//F = AB[0] - *G2;
	// Gaussian Filtering of diff. image
	//G1 = F.convolutionf(3, 3, (float *)kernel, 1.0f);
	// threshold for difference image
	//F.thresholdSelf(196);

//	delete []gMat;
	//delete []G1;
	//delete []G2;

	// return image
	//GrayImage *G3 = B->getLumImg();
	//*A = *G3;//*A = D;// & F;
}

// main code for CHAT
// 20010207 @ 0:43a
// CHAT 과정에서 필수적인 Histogram 분석을 하는 함수.
// 아마도 Otsu 방법 등을 통하여 임계치를 손쉽게 구할 수 있는
// 방법을 도입해야 할 것으로 추정됨.
void CFaceTool::HistoAnal(int *YBufVal, int mode, int setTh)
{
	// th. update by histogram analysis
	int i;

	long ThInc = 1L;
	//double HistoTh = 0.001; //?
	double HistoTh = 0.9;
	double MaxHist;
	double MaxVal = -1, MinVal = 1e6;
	int Th_Low = faceTh[setTh][0];
	int Th_High = faceTh[setTh][1];
	int Th_Tmp;

	for (i=0; i<256; i++)
	{
		if (YBufVal[i] > MaxVal)
			MaxVal = YBufVal[i];
		if (YBufVal[i] < MinVal)
			MinVal = YBufVal[i];
	}

	MaxHist = (double)((MaxVal-MinVal)*HistoTh + MinVal);

	// Find COG from Histogram
	double sum = 0.0;
	double wsum = 0.0;
	double center;
	for (i=0; i<256; i++)
	{
		sum += (i+1);
		wsum += (YBufVal[i]*(i+1));
	}
	
	center = (double)(wsum/sum - 1.0);

	int mid = (int)center;

	// IM_IN_RANGE
	if (YBufVal[mid] > MinVal)
	{
		for (i=mid; i>0; i--)
			if (YBufVal[i] < (long)MaxHist)
			{
				if (mode == 0)
				{
					if (i>(Th_Low))
						(Th_Low) += ThInc;
					else if (i<(Th_Low))
						(Th_Low) -= ThInc;
				}
				else
					(Th_Low) = i;
				break;
			}

		for(i=mid; i<256; i++)
			if (YBufVal[i] < (long)MaxHist)
			{
				if (mode == 0)
				{
					if (i>(Th_High))
						(Th_High) += ThInc;
					else if (i<(Th_High))
						(Th_High) -= ThInc;
				}
				else
					(Th_High) = i;
				break;
			}
	}
	// IM_OUT_RANGE
	else
	{
		for (i=mid; i>0; i--)
			if (YBufVal[i] > (long)MaxHist)
			{
				if (mode == 0)
				{
					if (i>(Th_Low))
						(Th_Low) += ThInc;
					else if (i<(Th_Low))
						(Th_Low) -= ThInc;
				}
				else
					(Th_Low) = i;
				break;
			}

		for(i=mid; i<256; i++)
			if (YBufVal[i] > (long)MaxHist)
			{
				if (mode == 0)
				{
					if (i>(Th_High))
						(Th_High) += ThInc;
					else if (i<(Th_High))
						(Th_High) -= ThInc;
				}
				else
					(Th_High) = i;
				break;
			}
	}

	if (Th_Low > Th_High)
	{
		Th_Tmp = Th_High;
		Th_High = Th_Low;
		Th_Low = Th_High;
	}

	if (abs(Th_Low - Th_High) < 10)
	{
		Th_Low = (100);
		Th_High = (190);
	}

	faceTh[setTh][0] = Th_Low;
	faceTh[setTh][1] = Th_High;
}

// 20010411@21:32p@DJ
// CHAT_mask motion_mask(*C) | return
// 0 0 | 0
// 1 0 | 1
// 0 1 | 2
// 1 1 | 3
// 두 개의 컬러 영상(A, B)을 받아 들여서 모션 정보를 구한다.
// 모션 정보는 각 컬러 영상에 대한 흑백 영상 간의 차이로 구하며,
// 필요에 따라서 CHAT_mask를 갱신하기도 한다.
unsigned char CFaceTool::DJmotion(ColorImage *A, ColorImage *B, GrayImage *C, unsigned char mode)
{
	unsigned char rst = 0;
	unsigned char v;
	int i;//, j;
	int motionTh = 128;
	int sumCHAT, sumC;
	double sumThfac = 0.01;
	int sumTh = (int)((A->w()*A->h())*(sumThfac));
	GrayImage *gA, *gB;

	gA = A->getLumImg();
	gB = B->getLumImg();

	sumCHAT = sumC = 0;

	// 20010411@20:52p
	for (i=0; i<A->w()*A->h(); i++)
	{
		if ((abs(*(gA->buf[0]+i) - *(gB->buf[0]+i)) < motionTh)
			&& (abs(*(gA->buf[0]+i) - *(gB->buf[0]+i)) > motionTh/2))
			v = 255;
		else
			v = 0;

		*(C->buf[0]+i) = v;

		sumCHAT += (*(CHAT_mask.buf[0]+i) == 255) ? 1 : 0;
		sumC += (v == 255) ? 1 : 0;
	}

	// chat_mask & motion
	if ((sumCHAT > sumTh) && (sumC > sumTh))
	{
		rst = 3;
		CHAT_mask = CHAT_mask ^ (*C);
	}
	// chat_mask & ~motion
	else if ((sumCHAT > sumTh) && (sumC <= sumTh))
	{
		rst = 1;
	}
	// ~chat_mask & motion
	else if ((sumCHAT <= sumTh) && (sumC > sumTh))
	{
		rst = 2;
		CHAT_mask = *C;
	}
	// ~chat_mask & ~motion
	else
		rst = 0;

	return rst;
}

// 16.11.01
// breman, GERMANY
void CFaceTool::DJfaceComponent(GrayImage A)
{
	if (A.d() != 1)
		return;

	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	int i, j;
	int pnFilter[9] = {-2, -2, -2, 0, 0, 0, 2, 2, 2};
	unsigned char *histo = new unsigned char[A.w()*A.h()];
	int th0;//, th1;
	int div = 0;
	GrayImage *B;

	for(i=0 ; i<8 ; i++) div += pnFilter[i];
	Hist1d hist(65536, -1, 256);

	int *HPvec = new int[A.h()];
	for (j=0; j<A.h(); j++)		HPvec[j] = 0;

	CHECK_TIME_START;

	B = A.convolution(3, 3, pnFilter, div);

	hist.addtohist(B->buf[0], A.w()*A.h());
	//hist.DJotsu(&th0, &th1);
	hist.DJotsu(&th0);

	//B->thresholdSelf(min(th0, th1), 255);
	B->thresholdSelf(th0, 255);
	//CImgMisc::ShowImg(CImgMisc::ImgConvG_c(B));

	hist.addtohist(A.buf[0], A.w()*A.h());
	//hist.DJotsu(&th0, &th1);
	hist.DJotsu(&th0);

	A.thresholdSelf(0, (int)((float)th0*0.5));
	//CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&A));

	HPvec = B->proj(GrayImage::Y, 0, 0, A.w(), A.h());
	compExtract(HPvec, A.h(), *B);

	CHECK_TIME_END(Time1, err);

	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\timeCompEx.txt", "at");
	fprintf(fp, "%f \n", Time1);
	fclose(fp);

	/*
	facecomp.rEyeBrow.left = facecomp.lEyeBrow.left = facecomp.lEye.left = facecomp.rEye.left = 0;
	facecomp.rEyeBrow.right = facecomp.lEyeBrow.right = facecomp.lEye.right = facecomp.rEye.right = A.w() - 1;
	facecomp.exNose.left = facecomp.exMouth.left = 0;
	facecomp.exNose.right = facecomp.exMouth.right = A.w()-1;
	*/

	/*
	A.drawRect(facecomp.rEyeBrow.left, facecomp.rEyeBrow.top, 
		facecomp.rEyeBrow.right, facecomp.rEyeBrow.bottom, 255);
	A.drawRect(facecomp.lEyeBrow.left, facecomp.lEyeBrow.top, 
		facecomp.lEyeBrow.right, facecomp.lEyeBrow.bottom, 255);
	A.drawRect(facecomp.rEye.left, facecomp.rEye.top, 
		facecomp.rEye.right, facecomp.rEye.bottom, 255);
	A.drawRect(facecomp.lEye.left, facecomp.lEye.top, 
		facecomp.lEye.right, facecomp.lEye.bottom, 255);
	/*
	A.drawRect(facecomp.exNose.left, facecomp.exNose.top, 
		facecomp.exNose.right, facecomp.exNose.bottom, 128);
	A.drawRect(facecomp.exNosex.left, facecomp.exNosex.top, 
		facecomp.exNosex.right, facecomp.exNosex.bottom, 128);
		*/
	/*
	A.drawRect(facecomp.exMouth.left, facecomp.exMouth.top, 
		facecomp.exMouth.right, facecomp.exMouth.bottom, 255);
	*/
	
	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&A));

	delete []HPvec;
	delete B;
	delete []histo;
}

// 20010614@22:25@DJ
// face component extraction.
// 16.11.01
// idea: projective grouping w/ varying lower limit (from 0 to some level)
// criteria: change lower limit w.r.t. group number
//
// 16.11.01
// CPtrList 동작 불능.
// 왜 ?
void CFaceTool::compExtract(int *proj, int projNum, GrayImage A)
{
	int i, j;
	int est_num = 0, est_num2 = 0;
	int flageye = 0, flageyebrow = 0;
	double diff, sigma;
	int *VPvec = new int[A.w()];
	int ref_Length = (int)((projNum)*(0.05));
	CString str;
	CFaceSegment *segment;
	int lower_limit = 0;
	int flag = 0;
	int start, end, length;
	int pre_est_num = 0;

	for (j=0; j<A.w(); j++)		VPvec[j] = 0;

	while (flag == 0)
	{
		start = end = length = 0;
		est_num = 0;

		// first scan
		for (i=0; i<projNum; i++)
		{
			if (proj[i] > lower_limit && i == 0)
			{
				start = i;
				est_num++;
			}
			else if (i != 0)
			{
				if (proj[i] > lower_limit && proj[i-1] <= lower_limit)
				{
					start = i;
					est_num++;
				}
				else if (proj[i-1] > lower_limit && proj[i] <= lower_limit)
				{
					end = i-1;
					length = end - start + 1;

					if (length <= ref_Length)
					{
						est_num--;
					}
				}
			}
		}

		/*
		if ((est_num <= 5) && (est_num >= 3))
			flag = 1;
		else
		{
			lower_limit += 5;
			pre_est_num = est_num;
		}
		*/

		flag = 1;
	}

	str.Format("lower limit = %d, est_num = %d", lower_limit, est_num);
	CImgMisc::DjMessageBox(str, 4);

	segment = new CFaceSegment[est_num];

	// second scan
	for (i=0; i<projNum; i++)
	{
		if (proj[i] > lower_limit && i == 0)
		{
			segment[est_num2].start = i;
			segment[est_num2].sum += proj[i];
		}
		else if (i != 0)
		{
			if (proj[i] > lower_limit && proj[i-1] <= lower_limit)
			{
				segment[est_num2].start = i;
				segment[est_num2].sum += proj[i];
			}
			else if (proj[i-1] > lower_limit && proj[i] <= lower_limit)
			{
				segment[est_num2].end = i-1;
				segment[est_num2].length = segment[est_num2].end - segment[est_num2].start + 1;

				if (segment[est_num2].length > ref_Length)
				{
					est_num2++;				
					str.Format("%d, %d", segment[est_num2].start, segment[est_num2].end);
					CImgMisc::DjMessageBox(str, 4);
				}
			}
			else if (proj[i] > lower_limit)
			{
				segment[est_num2].sum += proj[i];
			}
		}
	}

	if (proj[i-1] > lower_limit)
	{
		segment[est_num2].end = i-1;
		segment[est_num2].length = segment[est_num2].end - segment[est_num2].start + 1;
		est_num2++;		
	}

	if (est_num != est_num2)
	{
		CImgMisc::DjMessageBox("Grouping Error!", 4);
		return;
	}

	// gaussian feature extraction for each group
	for (i=0; i<est_num2; i++)
	{
		for (j = segment[i].start; j<segment[i].end; j++)
		{
			diff = (double)((proj[j] - segment[i].length/2)*(proj[j] - segment[i].length/2));
			sigma = (double)((segment[i].length/4)*(segment[i].length/4));
			segment[i].gauss += (float)(proj[j]*exp(-(diff/sigma)));
		}
	}

	double gaussmax = -1;
	int face_index[2] = {0, 0};

	for (i=0; i<est_num2; i++)
	{
		if (segment[i].end < projNum/2)
		{
			face_index[0] = i;

			for (j=i+1; j<est_num2; j++)
			{
				if (segment[j].start > projNum/2)
				{
					face_index[1] = j;
					break;
				}
			}
		}
		else
			break;
	}

	str.Format("face_index = [%d, %d]", face_index[0], face_index[1]);
	CImgMisc::DjMessageBox(str, 3);

	// Nose is located under the face_index[1].
	if (segment[face_index[1]].start - segment[face_index[0]].end < projNum/10)
	{
		for (i=0; i<face_index[0]; i++)
		{
			if (segment[i].length > projNum/20)
			{
				(facecomp.lEyeBrow).top = (facecomp.rEyeBrow).top = segment[i].start;
				(facecomp.lEyeBrow).bottom = (facecomp.rEyeBrow).bottom = segment[i].end;

				for (j=i+1; j<est_num; j++)
				{
					if (segment[j].length > projNum/20)
					{
						(facecomp.lEye).top = (facecomp.rEye).top = segment[j].start;
						(facecomp.lEye).bottom = (facecomp.rEye).bottom = segment[j].end;

						break;
					}
				}

				if (j != est_num)
					break;
				// fail to find eyes or eyebrows.
				else
				{
					(facecomp.lEye).top = (facecomp.rEye).top = (facecomp.lEyeBrow).bottom + 20;
					(facecomp.lEye).bottom = (facecomp.rEye).bottom = (facecomp.lEye).top + 20;
				}
			}
		}

		gaussmax = -1;
		for (i=face_index[1]+1; i<est_num; i++)
		{
			if ((segment[i].length > projNum/20) && (segment[i].gauss > gaussmax))
			{
				gaussmax = segment[i].gauss;

				(facecomp.exNose).top = segment[i].start;
				(facecomp.exNose).bottom = segment[i].end;

				break;
			}
		}

		for (j=i+1; j<est_num; j++)
		{
			if ((segment[j].length > projNum/20))
			{
				(facecomp.exMouth).top = segment[j].start;
				(facecomp.exMouth).bottom = segment[j].end;

				break;
			}
		}

		CImgMisc::DjMessageBox("Lower", 1);
	}
	// Nose is located in the middle of face_index[1] and face_index[0].
	else
	{
		for (i=0; i<face_index[0]; i++)
		{
			if (segment[i].length > projNum/20)
			{
				(facecomp.lEyeBrow).top = (facecomp.rEyeBrow).top = segment[i].start;
				(facecomp.lEyeBrow).bottom = (facecomp.rEyeBrow).bottom = segment[i].end;

				for (j=i+1; j<est_num; j++)
				{
					if (segment[j].length > projNum/20)
					{
						(facecomp.lEye).top = (facecomp.rEye).top = segment[j].start;
						(facecomp.lEye).bottom = (facecomp.rEye).bottom = segment[j].end;

						break;
					}
				}

				if (j != est_num)
					break;
				// fail to find eyes or eyebrows.
				else
				{
					(facecomp.lEye).top = (facecomp.rEye).top = (facecomp.lEyeBrow).bottom + 20;
					(facecomp.lEye).bottom = (facecomp.rEye).bottom = (facecomp.lEye).top + 20;
				}
			}
		}
		
		gaussmax = -1;
		for (i=face_index[0]+1; i<face_index[1]; i++)
		{
			if ((segment[i].length > projNum/20) && (segment[i].gauss > gaussmax))
			{
				gaussmax = segment[i].gauss;

				(facecomp.exNose).top = segment[i].start;
				(facecomp.exNose).bottom = segment[i].end;

				break;
			}
		}

		for (i=face_index[1]+1; i<est_num; i++)
		{
			if ((segment[i].length > projNum/20))
			{
				(facecomp.exMouth).top = segment[i].start;
				(facecomp.exMouth).bottom = segment[i].end;

				break;
			}
		}

		CImgMisc::DjMessageBox("Middle", 1);
	}

	// eyebrows
	if (facecomp.lEyeBrow.Height() > 0)
	{
		VPvec = A.proj(GrayImage::X, 0, facecomp.lEyeBrow.top, A.w(), facecomp.lEyeBrow.bottom);
		DJeyeExtract(VPvec, A.w(), 0);
	}

	// eyes
	if (facecomp.lEye.Height() > 0)
	{
		VPvec = A.proj(GrayImage::X, 0, facecomp.lEye.top, A.w(), facecomp.lEye.bottom);
		DJeyeExtract(VPvec, A.w(), 1);
	}

	// nose
	if (facecomp.exNose.Height() > 0)
	{
		VPvec = A.proj(GrayImage::X, 0, facecomp.exNose.top, A.w(), facecomp.exNose.bottom);
		DJeyeExtract(VPvec, A.w(), 2);
	}

	// mouth
	if (facecomp.exMouth.Height() > 0)
	{
		VPvec = A.proj(GrayImage::X, 0, facecomp.exMouth.top, A.w(), facecomp.exMouth.bottom);
		DJeyeExtract(VPvec, A.w(), 3);
	}

	delete []segment;
	delete []VPvec;
}

// 20010615@05:09@DJ
// eyes and eyebrows.
// even mouth and nose.
void CFaceTool::DJeyeExtract(int *proj, int projNum, int mode)
{
	int i, j;
	int est_num = 0, est_num2 = 0;
	CFaceSegment *segment;

	int max_proj = 0;
	for (i=0; i<projNum; i++)
	{
		if (proj[i] > max_proj)
			max_proj = proj[i];
	}

	int th = (int)max_proj*(0.01);

	// first scan
	for (i=0; i<projNum; i++)
	{
		//if (proj[i] != 0 && i == 0)
		if (proj[i] > th && i == 0)
			est_num++;
		else if (i != 0)
		{
			if (proj[i] != 0 && proj[i-1] == 0)
				est_num++;
		}
	}

	segment = new CFaceSegment[est_num];

	// second scan
	for (i=0; i<projNum; i++)
	{
		//if (proj[i] != 0 && i == 0)
		if (proj[i] > th && i == 0)
		{
			segment[est_num2].start = i;
			segment[est_num2].sum += proj[i];
		}
		else if (i != 0)
		{
			//if (proj[i] != 0 && proj[i-1] == 0)
			if (proj[i] > th && proj[i-1] <= th)
			{
				segment[est_num2].sum += proj[i];
				segment[est_num2].start = i;
			}
			//else if (proj[i-1] != 0 && proj[i] == 0)
			else if (proj[i-1] > th && proj[i] <= th)
			{
				segment[est_num2].end = i-1;
				est_num2++;
			}
			//else if (proj[i] != 0)
			else if (proj[i] > th)
			{
				segment[est_num2].sum += proj[i];
			}
		}
	}

	//if (proj[i-1] != 0)
	if (proj[i-1] > th)
		segment[est_num2].end = i-1;

	for (i=0; i<est_num; i++)
	{
		segment[i].length = segment[i].end - segment[i].start + 1;
	}	

	for (i=0; i<est_num; i++)
	{
		if (segment[i].length > projNum/10)
		{
			// eyebrows
			if (mode == 0)
			{
				facecomp.lEyeBrow.left = segment[i].start;
				facecomp.lEyeBrow.right = segment[i].end;
			}
			// eyes
			else if (mode == 1)
			{
				facecomp.lEye.left = segment[i].start;
				facecomp.lEye.right = segment[i].end;
			}
			// nose
			else if (mode == 2)
			{
				facecomp.exNose.left = segment[i].start;
				facecomp.exNose.right = segment[i].end;
			}
			// mouth
			else if (mode == 3)
			{
				facecomp.exMouth.left = segment[i].start;
				facecomp.exMouth.right = segment[i].end;
			}

			// only for eyes and eyebrows.
			// and nose...
			if (mode < 3)
			{
				for (j=i+1; j<est_num; j++)
				{
					if (segment[j].length > projNum/10)				
					{
						// eyebrows
						if (mode == 0)
						{
							facecomp.rEyeBrow.left = segment[j].start;
							facecomp.rEyeBrow.right = segment[j].end;
						}
						// eyes
						else if (mode == 1)
						{
							facecomp.rEye.left = segment[j].start;
							facecomp.rEye.right = segment[j].end;
						}
						else if (mode == 2)
						{
							facecomp.exNosex.left = segment[j].start;
							facecomp.exNosex.right = segment[j].end;
						}

						break;
					}
				}
			}

			break;
		}
	}

	delete []segment;
}

float CFaceTool::DJdegree_of_mouth_openness(ColorImage *A)
{
	float degree = 0.0f;

	return (degree);
}

float CFaceTool::DJdegree_of_eye_openness(ColorImage *A)
{
	float degree = 0.0f;

	return (degree);
}

// 20030501 -> CFRTK
//CVisDVector CFaceTool::DJdegree_of_facial_expressions(GrayImage A)
void CFaceTool::DJdegree_of_facial_expressions(GrayImage A, float* degree)
{
	int i, j;
	int pt_num;
	const int wave_num = 2;
	const int theta_num = 4;

	double wave[wave_num] = {4.5, 6.0};
	double theta[theta_num] = {0.0, 45.0, 90.0, 135.0};
	Filter_mask real_part[wave_num][theta_num], img_part[wave_num][theta_num];

	GrayImage B, D;
	D = B = A;

	CPoint xy[10];
	CPoint myxy[12];

	double mag;

	FILE *fp, *fp2, *fp3;

	fp = fopen("C:\\MATLABR11\\work\\FEERS\\gabor\\point.dat", "rt");
	fp2 = fopen("C:\\MATLABR11\\work\\FEERS\\gabor\\pt_gf.dat", "awt");
	fp3 = fopen("C:\\MATLABR11\\work\\FEERS\\gabor\\mouth_si.dat", "awt");

	fscanf(fp, "%d ", &pt_num);

	for (i=0; i<pt_num; i++)
	{
		fscanf(fp, "%d %d ", (int *)(&xy[i].x), (int *)(&xy[i].y));
	}

	// point conversion (8 -> 12)
	
	// eyebrows
	myxy[0] = xy[0];
	myxy[1] = xy[1];
	
	// eyes (left)
	myxy[2].x = xy[2].x;	myxy[2].y = (xy[2].y + xy[3].y)/2;
	myxy[3].x = xy[3].x;	myxy[3].y = myxy[2].y;
	
	// nose wrinkle
	myxy[4].x = (xy[3].x + xy[4].x)/2;	myxy[4].y = (xy[3].y + xy[4].y)/2;

	// eyes (right)
	myxy[5].x = xy[4].x;	myxy[5].y = (xy[4].y + xy[5].y)/2;
	myxy[6].x = xy[5].x;	myxy[6].y = myxy[5].y;

	// nasal root (left)
	myxy[7].x = xy[6].x;	myxy[7].y = (xy[3].y + xy[6].y)/2;

	// nasal root (right)
	myxy[8].x = xy[7].x;	myxy[8].y = (xy[5].y + xy[6].y)/2;

	// mouth (left)
	myxy[9].x = xy[6].x;	myxy[9].y = (xy[6].y + xy[7].y)/2;

	// mouth (center)
	myxy[10].x = (xy[6].x + xy[7].x)/2;	myxy[10].y = myxy[9].y;

	// mouth (right)
	myxy[11].x = xy[7].x;	myxy[11].y = myxy[9].y;

	GaborFilter gabor;

	// gabor filtering
	for (j=0; j<theta_num; j++)
		for (i=0; i<wave_num; i++)
		{
			gabor.making_gabor_filter3(wave[i], 1.5, M_PI*((45.0)/180.0), M_PI*((theta[j])/180.0), &(real_part[i][j]), &(img_part[i][j]));
		}

	for (j=0; j<theta_num; j++)
		for (i=0; i<wave_num; i++)
		{
			GrayImage C = B;
			double *bank1 = C.point_convolution(real_part[i][j].hsize,real_part[i][j].vsize, real_part[i][j].co, myxy, 12, 1.0/real_part[i][j].sum);
			double *bank2 = C.point_convolution(img_part[i][j].hsize,img_part[i][j].vsize, img_part[i][j].co, myxy, 12, 1.0/real_part[i][j].sum);

			for (int k=0; k<12; k++)
			{
				mag = (bank1[myxy[k].x + myxy[k].y * C.w()])*(bank1[myxy[k].x + myxy[k].y * C.w()])
					+ (bank2[myxy[k].x + myxy[k].y * C.w()])*(bank2[myxy[k].x + myxy[k].y * C.w()]);
				fprintf(fp2, "%d %d %f \n", myxy[k].x, myxy[k].y, mag);

				D.setPixel(myxy[k].x, myxy[k].y, 255);
			}

			delete [] bank2;
			delete [] bank1;
		}

	float area_ratio, height_ratio, fg;

	height_ratio = (float)(xy[7].y-xy[6].y)/(float)(xy[9].y-xy[8].y);
	area_ratio = height_ratio * (float)(xy[7].x-xy[6].x)/(float)(xy[9].x-xy[8].x);
	GrayImage *C = A.getSubImage(xy[6].x, xy[6].y, xy[7].x, xy[7].y);
	fg = getfG(*C, 4.5);

	fprintf(fp3, "%f %f %f \n", area_ratio, height_ratio, fg);

	fclose(fp3);
	fclose(fp2);
	fclose(fp);

	CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&D));

	for (j=0; j<theta_num; j++)
		for (i=0; i<wave_num; i++)
		{
			delete [] (real_part[i][j]).co;
			delete [] (img_part[i][j]).co;
		}
}