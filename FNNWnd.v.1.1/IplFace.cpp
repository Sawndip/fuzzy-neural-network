// IplFace.cpp: implementation of the CIplFace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "IplFace.h"
#include "math.h"
#include "GaborFilter.h"
#include "SVV.h"
#include "myresource.h"

#include "ipl.h"
#include "cv.h"
#include "highgui.h"

#include "MainFrm.h"
#include "LogView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 20020822
void iplFaceStructure::SaveFeatures(const char *str)
{
	FILE *fp;

	fp = fopen(str, "awt");

	//fprintf(fp, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f ", 
	//	dEb, dEbEl, dEbEr, Ehl, Ehr, dEMl, dEMr, Mh, Mw, NLR, NLFl, NLFr, CRWl, CRWr);
	fprintf(fp, "%f %f %f %f %f ", __max(DEOl, DEOr), NLR, __max(NLFl, NLFr), DMO, __max(dEbEl, dEbEr));


	// Emotion Code
	fprintf(fp, "%d ", emotions);

	fprintf(fp, "\n");

	fclose(fp);
}

void iplFaceTemplate::iplCorrEdge(IplImage *img, float *corr)
{
	float theta = (float)(RAD_DEG*au);
	float cost = (float)cos(theta);
	float sint = (float)sin(theta);
	int dist = 4;

	IPLIMAGE temp = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
	IPLIMAGE temp2 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);

	iplSet(temp, 0);
	cvLine(temp, cvPoint(x, y+dist), cvPoint(x + dr*cost, y+dist - dr*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y+dist), cvPoint(x - dl*cost, y+dist + dl*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y-dist), cvPoint(x + dr*cost, y-dist - dr*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y-dist), cvPoint(x - dl*cost, y-dist + dl*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y), cvPoint(x + dd*sint, y + dd*cost), (int)(0xFFFFFF), td);

	iplSet(temp2, 0);
	iplAnd(temp, img, temp2);			
	*corr = (float)cvCountNonZero(temp2);

	cvReleaseImage(&(temp2));
	cvReleaseImage(&(temp));
}

// incomplete
// 20020705
void iplFaceTemplate::iplDTMshow(IplImage *img)
{
	float theta = (float)(RAD_DEG*au);
	float cost = (float)cos(theta);
	float sint = (float)sin(theta);
	int dist = 4;

	IPLIMAGE temp = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
	IPLIMAGE temp2 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);

	iplSet(temp, 0);
	cvLine(temp, cvPoint(x, y+dist), cvPoint(x + dr*cost, y+dist - dr*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y+dist), cvPoint(x - dl*cost, y+dist + dl*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y-dist), cvPoint(x + dr*cost, y-dist - dr*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y-dist), cvPoint(x - dl*cost, y-dist + dl*sint), (int)(0xFFFFFF), tu);
	cvLine(temp, cvPoint(x, y), cvPoint(x + dd*sint, y + dd*cost), (int)(0xFFFFFF), td);

	iplSet(temp2, 0);
	iplAnd(temp, img, temp2);			

	cvReleaseImage(&(temp2));
	cvReleaseImage(&(temp));
}

CIplFace::CIplFace()
{
}

CIplFace::~CIplFace()
{

}

//////////////////////////////////////////////////////////////////////
// Processing
//////////////////////////////////////////////////////////////////////

void CIplFace::iplFaceAllocImage(IplImage *Simg)
{
	if (Simg == NULL)		return;

	CPsize = cvSize(Simg->width, Simg->height);
	LPsize = cvSize(LP_SIZE_X, LP_SIZE_Y);

	int i, j;
	float x, y;
	int twistdata[16] = {
		0.33*CE21, 0.33*CE21, 0.33*CE21, 0, 
		-0.5*CE21, 0, 0.5*CE21, 0, 
		-0.25*CE21, 0.5*CE21, -0.25*CE21, 0,
		0, 0, 0, CE21};

	int bins = 256;
	int dims1D[1] = {bins};
	float *hist_th = new float[2];
	hist_th[0] = 0.0f; hist_th[1] = 255.0f;
	histG = new CvHistogram;
	histG = cvCreateHist(1, dims1D, CV_HIST_ARRAY);
	cvSetHistThresh(histG, &hist_th, 1);
	delete [] hist_th;

	projX = cvMat(1, CPsize.width, CV_MAT32F, NULL);
	projY = cvMat(1, CPsize.height, CV_MAT32F, NULL);
	projXLP = cvMat(1, LPsize.width, CV_MAT32F, NULL);
	projYLP = cvMat(1, LPsize.height, CV_MAT32F, NULL);

	cvmAlloc(&projX);
	cvmAlloc(&projY);
	cvmAlloc(&projXLP);
	cvmAlloc(&projYLP);
	
	// color twist
	mytwist = iplCreateColorTwist(twistdata, 31);
	iplSetColorTwist(mytwist, twistdata, 31);

	twist_I1I2I3 = iplCreateColorTwist(twistdata, 31);
	iplSetColorTwist(twist_I1I2I3, twistdata, 31);
	
	twist_I1I2I3N = iplCreateColorTwist(twistdata, 31);
	iplSetColorTwist(twist_I1I2I3N, twistdata, 31);

	GaborFilter gabor;
	Filter_mask2 real_part, img_part;
    float theta[4] = {0.0f, 45.0f, -90.0f, -45.0f};

	cvInitFont(&font, CV_FONT_VECTOR0, 0.2f, 0.2f, 0.0f, 1);

	for (i=0; i<NUM_FILTERS; i++)
	{
		//gabor.making_gabor_filter4(wave, 1.5, M_PI*((45.0)/180.0), M_PI*((theta)/180.0), &real_part, &img_part);
		gabor.making_gabor_filter4(5.0f, 1.5f, M_PI*((45.0f)/180.0f), M_PI*((theta[i])/180.0f), &real_part, &img_part);

		kernel[i][0] = iplCreateConvKernelFP(real_part.hsize, real_part.vsize, real_part.hsize/2, real_part.vsize/2, (float *)real_part.co);
		kernel[i][1] = iplCreateConvKernelFP(img_part.hsize, img_part.vsize, img_part.hsize/2, img_part.vsize/2, (float *)img_part.co);
	}

	// space-variant vision
	CSVV svv(1);
	svv.LPtable(CPsize.width, CPsize.height,
		LPsize.width, LPsize.height);

	// x & y map for remapping
	xmap = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);
	ymap = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);

	for (i=0; i<LPsize.width; i++)
		for (j=0; j<LPsize.height; j++)
		{
			x = (float)svv.btrX[i*LPsize.height+j];
			y = (float)svv.btrY[i*LPsize.height+j];

			iplPutPixel(xmap, i, j, &x);
			iplPutPixel(ymap, i, j, &y);
		}

	// image allocation for processing
	for (i=0; i<NUM_BUFFERS; i++) 
	{
		procCPBimg[i] = cvCreateImage(CPsize, IPL_DEPTH_1U, 1);
		procCPGimg[i] = cvCreateImage(CPsize, IPL_DEPTH_8U, 1);
		procCPSimg[i] = cvCreateImage(CPsize, IPL_DEPTH_16S, 1);
		procCPFimg[i] = cvCreateImage(CPsize, IPL_DEPTH_32F, 1);
		procCPCimg[i] = cvCreateImage(CPsize, IPL_DEPTH_8U, 3);
		procCPCSimg[i] = cvCreateImage(CPsize, IPL_DEPTH_8U, 1);
		procLPGimg[i] = cvCreateImage(LPsize, IPL_DEPTH_8U, 1);
		procLPCimg[i] = cvCreateImage(LPsize, IPL_DEPTH_8U, 3);
	}	

	procCHATmask = cvCreateImage(CPsize, IPL_DEPTH_1U, 1);
	storage = cvCreateMemStorage();
}

void CIplFace::iplFaceFreeImage()
{
	int i;

	cvReleaseMemStorage(&storage);
	cvReleaseImage(&(procCHATmask));

	// image deallocation
	for (i=0; i<NUM_BUFFERS; i++)
	{
		cvReleaseImage(&(procLPCimg[i]));
		cvReleaseImage(&(procLPGimg[i]));
		cvReleaseImage(&(procCPCSimg[i]));
		cvReleaseImage(&(procCPCimg[i]));
		cvReleaseImage(&(procCPFimg[i]));
		cvReleaseImage(&(procCPSimg[i]));
		cvReleaseImage(&(procCPGimg[i]));		
		cvReleaseImage(&(procCPBimg[i]));
	}

	cvReleaseImage(&ymap);
	cvReleaseImage(&xmap);

	for (i=0; i<NUM_FILTERS; i++)
	{
		iplDeleteConvKernelFP(kernel[i][1]);
		iplDeleteConvKernelFP(kernel[i][0]);
	}

	iplDeleteColorTwist(twist_I1I2I3N);
	iplDeleteColorTwist(twist_I1I2I3);
	iplDeleteColorTwist(mytwist);

	cvmFree(&projYLP);
	cvmFree(&projXLP);
	cvmFree(&projY);
	cvmFree(&projX);

	cvReleaseHist( &histG );
}

void CIplFace::iplFaceLocate(IplImage *Simg, iplFaceStructure *face, IplImage *Dimg)
{
	if (!CheckSize(Simg))		return;

	static int face_th[2][2] = {{16, 255}, {4, 255}}; 
	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	CHECK_TIME_START;

	// 1. RGB -> I1I2I3
	iplApplyColorTwist(Simg, procCPCimg[0], mytwist, 0);

	// 2. get SUBimg from I1I2I3 img
	cvCvtPixToPlane(procCPCimg[0], procCPCSimg[0], procCPCSimg[1], procCPCSimg[2], NULL);

	// 3. bin. I2, bin. I3
	cvThreshold(procCPCSimg[1], procCPGimg[0], face_th[0][0], face_th[0][1], CV_THRESH_BINARY);
	cvThreshold(procCPCSimg[2], procCPGimg[1], face_th[1][0], face_th[1][1], CV_THRESH_BINARY_INV);

	// 4. get (I2 & I3)
	iplAnd(procCPGimg[0], procCPGimg[1], procCPGimg[2]);

	// 5. blob analysis (max area, 1 blob)
	CvSeq* chain = 0; 
	double area,max_area;
	int num_conts;

	// 5-1. find contours
	num_conts = cvFindContours(procCPGimg[2], storage, &chain, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (chain==0) 
	{ 
		face_th[0][0] = 16;
		face_th[0][1] = 255;
		face_th[1][0] = 4;
		face_th[1][1] = 255;

		//cvReleaseMemStorage(&storage); 
		return; 
	}

	// 5-2. search all contours to find max. sized contours
	max_area=0;
	CvSeq *contour=NULL;
	for( CvSeq* c = chain; c != 0; c = c->h_next )
	{
		area = fabs(cvContourArea(c));
		if (area > max_area) { max_area=area; contour=c; }
	}
	if (contour==NULL)
	{ 
		face_th[0][0] = 16;
		face_th[0][1] = 255;
		face_th[1][0] = 4;
		face_th[1][1] = 255;

		//cvReleaseMemStorage(&storage); 
		return; 
	}

	contMayor=contour;
	
	iplSet(procCPGimg[3], 0);
	cvDrawContours(procCPGimg[3], contMayor, 255, 255, 0, CV_FILLED);
	
	// 6. get result for face region
	face->face.rect = cvContourBoundingRect(contMayor, 1);
	/*
	face->face.rect.x = __min(__max(face->face.rect.x, 0), Simg->width);
	face->face.rect.y = __max(__min(face->face.rect.y, Simg->height), 0);
	face->face.rect.width = __min(face->face.rect.width, Simg->width);
	face->face.rect.height = __min(face->face.rect.height, Simg->height);
	*/

	iplSet(procCPBimg[0], 0);
	iplGreaterS(procCPGimg[3], 0, procCPBimg[0]);
	face->mask = procCPBimg[0];

	iplFaceCHAT(procCPCSimg[1], face, face_th[0]);
	iplFaceCHAT(procCPCSimg[2], face, face_th[1]);

	if ((face_th[1][0] > face_th[1][1]) || (face_th[1][1] <= 1) || (face_th[1][0] <= 1)
		|| (cvCountNonZero(procCPGimg[0]) < 1e1))
	{
		face_th[0][0] = 16;
		face_th[0][1] = 255;
		face_th[1][0] = 4;
		face_th[1][1] = 255;
	}

	// for debugging
	//char str[100];
	//sprintf(str, "[%d,%d] [%d,%d]", face_th[0][0], face_th[0][1], face_th[1][0], face_th[1][1]);
	//cvPutText(procCPGimg[3], str, cvPoint(10, 10), &font, (int)(0xAAAAAA));

	CHECK_TIME_END(Time1, err);

	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\timeFaceSeg.txt", "at");
	fprintf(fp, "%f \n", Time1);
	fclose(fp);

	iplOr(procCPGimg[3], procCPGimg[4], procCPGimg[3]);
	cvCopyImage(procCPGimg[3], Dimg);

	/*
	// 6. ellipse fitting
	int count = contMayor->total;
	CvBox2D myBox;
	CvPoint2D32f *PointArray32f = (CvPoint2D32f *)malloc(count * sizeof(CvPoint2D32f));
	CvPoint *PointArray = (CvPoint *)malloc(count * sizeof(CvPoint));
	int *hull = (int *) malloc(count * sizeof(int));
	int hullsize;
	
	cvCvtSeqToArray(contMayor, PointArray, CV_WHOLE_SEQ);
	cvConvexHull(PointArray, count, NULL, CV_COUNTER_CLOCKWISE, hull, &hullsize);

	for (int i=0; i<hullsize; i++)
	{ 
		PointArray32f[i].x = float( PointArray[hull[i]].x);
		PointArray32f[i].y = float( PointArray[hull[i]].y);
	}

	cvFitEllipse(PointArray32f, hullsize, &myBox);

	iplSet(procCPGimg[5], 0);
	cvDrawContours(procCPGimg[5], contMayor, 255, 0, 0, CV_FILLED);
	iplSet(procCPGimg[3], 0);
	iplAnd(procCPGimg[5], procCPGimg[0], procCPGimg[3]);

	free(PointArray);
	free(PointArray32f);
	*/

	/*
	// 5. find ellipsoidal region
	*cx = myBox.center.x;
	*cy = myBox.center.y;
	*cr = myBox.angle;	

	/*
	CvPoint center;
	center.x = *cx;	center.y = *cy;
	CvSize axes;
	axes.width = myBox.size.width/2;	axes.height = myBox.size.height/2;

	iplSet(procCPGimg[3], 0);
	cvEllipse(procCPGimg[3], center, axes, -(*cr), 0.0, 360.0, (int)(0xFFFFFF), -1);
	iplGreaterS(procCPGimg[3], 0, maskCPimg[1]);

	procCPGimg[0]->maskROI = maskCPimg[1];
	iplSet(procCPGimg[3], 0);
	iplCopy(procCPGimg[0], procCPGimg[3]);
	procCPGimg[0]->maskROI = NULL;
	*/

	/*
	// 6-3. warping(trans.), rotate
	const double coeffs[2][3] = {{1, 0, 80-*cx}, {0, 1, 60-*cy}};
	iplSet(Dimg, 0);
	iplWarpAffine(procCPGimg[3], Dimg, coeffs, IPL_INTER_LINEAR);
	//iplSet(procCPGimg[4], 0);	iplWarpAffine(procCPGimg[3], procCPGimg[4], coeffs, IPL_INTER_LINEAR);
	//iplSet(Dimg, 0);	iplRotateCenter(procCPGimg[4], Dimg, (*cr)-90.0, 80, 60, IPL_INTER_LINEAR);
	*/

}

void CIplFace::iplFaceComponent(IplImage *Simg, iplFaceStructure *face, IplImage *Dimg)
{
	if (!CheckSize(Simg))		return;

	int th;
	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	// 1. gray conversion
	iplColorToGray(Simg, procCPGimg[3]);

	/*
	// 2. get h-edge	
	iplFixedFilter(procCPGimg[0], procCPGimg[1], IPL_SOBEL_3x3_H);

	// 3. get h-edge & faceROI
	procCPGimg[1]->maskROI = face->mask;
	iplSet(procCPGimg[4], 0);
	iplCopy(procCPGimg[1], procCPGimg[4]);
	procCPGimg[1]->maskROI = NULL;
	
	// 4. get histogram & decide max. level
	iplOtsu1(procCPGimg[4], &th, 1);
	cvThreshold(procCPGimg[4], procCPGimg[5], (int)((float)th*1.2), 255, CV_THRESH_BINARY);
	*/

	/*
	// 5. get histogram & bin.
	procCPGimg[0]->maskROI = face->mask;
	iplSet(procCPGimg[6], 0);
	iplCopy(procCPGimg[0], procCPGimg[6]);
	procCPGimg[0]->maskROI = NULL;

	iplOtsu1(procCPGimg[6], &th, 0);
	cvThreshold(procCPGimg[6], procCPGimg[7], (int)((float)th*1.5), 255, CV_THRESH_BINARY_INV);

	procCPGimg[7]->maskROI = face->mask;
	iplSet(procCPGimg[8], 0);
	iplCopy(procCPGimg[7], procCPGimg[8]);
	procCPGimg[7]->maskROI = NULL;

	iplOr(procCPGimg[5], procCPGimg[8], procCPGimg[9]);
	*/

	//iplProject(procCPGimg[5], projX, 0);
	//iplProject(procCPGimg[5], projY, 1);

	//cvvShowImage("Loaded Image", procCPGimg[5]);

	CHECK_TIME_START;
	iplFaceDTM(procCPGimg[3], procCPGimg[5], face);
	CHECK_TIME_END(Time1, err);

	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\timeDTM.txt", "at");
	fprintf(fp, "%f \n", Time1);
	fclose(fp);

	cvCopyImage(procCPGimg[5], Dimg);
}

void CIplFace::iplFaceFeatureExtract(IplImage *Simg, iplFaceStructure *face)
{
	if (!CheckSize(Simg))		return;

	static int k = 0;
	int i;
	char str[100];

	iplSet(procCPGimg[5], 0);

	/*
	iplFixedFilter(Simg, procCPGimg[5], IPL_SOBEL_3x3_H);
	iplGrayToColor(procCPGimg[5], procCPCimg[0], 1, 1, 1);
	
	sprintf(str, ".\\h_edge%03d.tif", k);
	cvvSaveImage(str, procCPCimg[0]);
	
	iplSet(procCPGimg[5], 0);
	iplFixedFilter(Simg, procCPGimg[5], IPL_SOBEL_3x3_V);
	iplGrayToColor(procCPGimg[5], procCPCimg[0], 1, 1, 1);
	
	sprintf(str, ".\\v_edge%03d.tif", k);
	cvvSaveImage(str, procCPCimg[0]);
	
	iplSet(procCPGimg[5], 0);
	cvCanny(Simg, procCPGimg[5], 200, 255);
	iplGrayToColor(procCPGimg[5], procCPCimg[0], 1, 1, 1);
	
	sprintf(str, ".\\canny%03d.tif", k++);
	cvvSaveImage(str, procCPCimg[0]);
	*/
	
	/*
	float feature;
	GGfeature(Simg, 5.0, 45.0, &feature);
	*/

	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	CHECK_TIME_START;
	iplScaleFP(Simg, procCPFimg[0], 0.0, 255.0);

	for (i=0; i<NUM_FILTERS; i++)		
	{		
		iplConvolve2DFP(procCPFimg[0], procCPFimg[i+1], kernel[i], 2, IPL_SUMSQROOT);
		iplNormalize(procCPFimg[i+1], procCPGimg[i+6]);
	}

	CHECK_TIME_END(Time1, err);

	/*
	FILE *fp;
	fp = fopen("c:\\matlabr11\\work\\timeGabor.txt", "at");
	fprintf(fp, "%f \n", Time1);
	fclose(fp);
	*/

	iplFixedFilter(Simg, procCPGimg[5], IPL_SOBEL_3x3_H);
	
	/*
	//ShowSubFace(procCPGimg[6], procCPGimg[5], face->left_eye.rect, 5);
	//ShowSubFace(procCPGimg[6], procCPGimg[5], face->right_eye.rect, 5);
	ShowSubFace(procCPGimg[6], procCPGimg[5], face->left_eye.rect, 7);
	ShowSubFace(procCPGimg[6], procCPGimg[5], face->right_eye.rect, 7);
	ShowSubFace(procCPGimg[8], procCPGimg[5], face->left_eyebrow.rect, 4);
	ShowSubFace(procCPGimg[8], procCPGimg[5], face->right_eyebrow.rect, 4);
	ShowSubFace(procCPGimg[6], procCPGimg[5], face->nasolabial_root.rect, 5);
	ShowSubFace(procCPGimg[7], procCPGimg[5], face->left_nasolabial_fold.rect, 0);
	ShowSubFace(procCPGimg[9], procCPGimg[5], face->right_nasolabial_fold.rect, 0);
	ShowSubFace(procCPGimg[6], procCPGimg[5], face->mouth.rect, 6);
	*/

	//iplFaceSubFeature(procCPGimg[5], &(face->left_eye), 7);		// LP
	//iplFaceSubFeature(procCPGimg[5], &(face->right_eye), 7);		// LP
	iplFaceSubFeature(procCPGimg[8], &(face->left_eye), 8);	// gabor
	iplFaceSubFeature(procCPGimg[8], &(face->right_eye), 8);	// gabor
	iplFaceSubFeature(procCPGimg[6], &(face->nasolabial_root), 5);
	iplFaceSubFeature(procCPGimg[6], &(face->left_crow_feet), 5);
	iplFaceSubFeature(procCPGimg[6], &(face->right_crow_feet), 5);
	iplFaceSubFeature(procCPGimg[6], &(face->nasolabial_root), 5);
	iplFaceSubFeature(procCPGimg[7], &(face->left_nasolabial_fold), 0);
	iplFaceSubFeature(procCPGimg[9], &(face->right_nasolabial_fold), 0);
	iplFaceSubFeature(procCPGimg[6], &(face->mouth), 6);

	face->left_eyebrow.feature = (float)(face->left_eye.GetCenterY() - face->left_eyebrow.GetCenterY());
	//sprintf(str, "%d", (int)(face->left_eyebrow.feature));
	//cvPutText(procCPGimg[5], str, cvPoint(face->left_eyebrow.rect.x, face->left_eyebrow.rect.y + 10), &font, (int)(0xAAAAAA));

	face->right_eyebrow.feature = (float)(face->right_eye.GetCenterY() - face->right_eyebrow.GetCenterY());
	//sprintf(str, "%d", (int)(face->right_eyebrow.feature));
	//cvPutText(procCPGimg[5], str, cvPoint(face->right_eyebrow.rect.x, face->right_eyebrow.rect.y + 10), &font, (int)(0xAAAAAA));

	// 031224
	// extended features
	face->dEb = face->right_eye.l() - face->left_eye.r();
	face->dEbEl = face->left_eyebrow.feature;
	face->dEbEr = face->right_eyebrow.feature;
	face->Ehl = face->left_eye.b() - face->left_eye.t();
	face->Ehr = face->right_eye.b() - face->right_eye.t();
	face->dEMl = face->mouth.GetCenterY() - face->left_eye.b();
	face->dEMr = face->mouth.GetCenterY() - face->right_eye.b();
	face->Mh = face->mouth.b() - face->mouth.t();
	face->Mw = face->mouth.r() - face->mouth.l();
	face->NLR = face->nasolabial_root.feature;
	face->NLFl = face->left_nasolabial_fold.feature;
	face->NLFr = face->right_nasolabial_fold.feature;
	face->CRWl = face->left_crow_feet.feature;
	face->CRWr = face->right_crow_feet.feature;

	face->DEOl = face->left_eye.feature;
	face->DEOr = face->right_eye.feature;
	face->DMO = face->mouth.feature;

	face->SaveFeatures("K:\\User\\research\\faceDB\\ZBienDB\\fs.dat");

	//cvvShowImage("Result Image", procCPGimg[5]);
	//cvvWaitKey(0);
}

void CIplFace::iplFaceExpression(IplImage *Simg, iplFaceStructure *face)
{
	if (!CheckSize(Simg))		return;
}

void CIplFace::iplBlobMaxArea(IplImage *Simg, CvMemStorage *storage, CvSeq *contMayor)
{
	if (Simg->nChannels != 1)	return;

	CvSeq* chain = 0; 
	double area,max_area;
	int num_conts;

	// 1. find contours
	num_conts = cvFindContours(Simg, storage, &chain, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	if (chain==0) { cvReleaseMemStorage(&storage); return; }

	// 2. search all contours to find max. sized contours
	max_area=0;
	CvSeq *contour=NULL;
	for( CvSeq* c = chain; c != 0; c = c->h_next )
	{
		area = fabs(cvContourArea(c));
		if (area > max_area) { max_area=area; contour=c; }
	}
	if (contour==NULL) { cvReleaseMemStorage(&storage); return; }

	contMayor=contour;
}

// method
// 0: gray image, 1: color(3-band) image
void CIplFace::ShowHistogram(IplImage *Simg, IplImage *Dimg, int start, int method)
{
	int val, i, j, MaxVal = 0;
	int bins = 256;

	if (method == 0)
	{
		cvCalcHist(&Simg, histG, 0, NULL);

		for (i = 1; i < bins; i++) {
			val = cvRound(cvQueryHistValue_1D( histG, i ));
			if (val > MaxVal)
				MaxVal = val;
		}

		iplSet(Dimg, 0);

		for( i = start; i < bins; i++ ) {
			val = cvRound(cvQueryHistValue_1D( histG, i ));
			CvPoint p[4];
			
			p[0].x = p[1].x = i * Simg->width/bins;
			p[2].x = p[3].x = (i+1) * Simg->width/bins;
			
			p[1].y = p[2].y = Simg->height - 1;
			p[0].y = p[3].y = Simg->height - (int) ((double)Simg->height * (double)((double)val / (double)MaxVal));
			
			cvFillConvexPoly( Dimg, p, 4, CV_RGB(128,128,128));
		}
	}
	else
	{
		cvCvtPixToPlane(Simg, procCPCSimg[0], procCPCSimg[1], procCPCSimg[2], NULL);	
		iplSet(Dimg, 0);

		for (j=0; j<3; j++)
		{
			MaxVal = 0;

			cvCalcHist(&procCPCSimg[j], histG, 0, NULL);

			for (i = 1; i < bins; i++) {
				val = cvRound(cvQueryHistValue_1D( histG, i ));
				if (val > MaxVal)
					MaxVal = val;
			}

			for( i = start; i < bins; i++ ) {
				val = cvRound(cvQueryHistValue_1D( histG, i ));
				CvPoint p[4];
				
				p[0].x = p[1].x = i * Simg->width/bins;
				p[2].x = p[3].x = (i+1) * Simg->width/bins;
				
				p[1].y = p[2].y = Simg->height - 1;
				p[0].y = p[3].y = Simg->height - (int) ((double)Simg->height * (double)((double)val / (double)MaxVal));
				
				switch(j)
				{
					case 0:
						cvFillConvexPoly( Dimg, p, 4, CV_RGB(255,0,0));
						break;
					case 1:
						cvFillConvexPoly( Dimg, p, 4, CV_RGB(0,255,0));
						break;
					case 2:
						cvFillConvexPoly( Dimg, p, 4, CV_RGB(0,0,255));
						break;
				}
				
			}
		}
	}
}

// 0: X, 1: Y
void CIplFace::iplProject(IplImage *Simg, CvMat proj, int dir, int method)
{
	if ((dir < 0) || (dir > 1))	return;

	int x, y;
	long sum;
	unsigned char pixel;
	int wcount, bcount;
	float weight;

	// projX
	if (dir == 0)
	{
		for (x=0; x<Simg->width; x++)
		{
			sum = 0;
			wcount = 0;
			bcount = 0;

			for (y=0; y<Simg->height; y++)
			{
				iplGetPixel(Simg, x, y, &pixel);

				switch (method)
				{
					case 0:
						sum += pixel;
						break;
					case 1:
						// init.
						if (y == 0)
						{
							if (pixel == 0)
								bcount++;
							else
								wcount++;
						}
						else
						{
							if (pixel == 0)
							{
								if (bcount)
									bcount++;
								else
								{
									wcount = 0;
									bcount++;
								}
							}
							else
							{
								if (wcount)
									wcount++;
								else
								{
									bcount = 0;
									wcount++;
								}
							}
						}

						sum += (wcount)*pixel;//(wcount-bcount)*pixel;
						break;
					case 2:
						//weight = (float)(exp(y) - 1.0f);
						weight = (float)100*(float)(exp(y) - 1.0f);
						sum += (long)weight*pixel;
						break;
					default:
						break;
				}
			}

			proj.data.fl[x] = sum;
		}
	}
	// projY
	if (dir == 1)
	{
		for (y=0; y<Simg->height; y++)
		{
			sum = 0;
			wcount = 0;
			bcount = 0;

			for (x=0; x<Simg->width; x++)
			{
				iplGetPixel(Simg, x, y, &pixel);
			
				switch(method)
				{
					case 0:
						sum += pixel;
						break;
					case 1:
						// init.
						if (x == 0)
						{
							if (pixel == 0)
								bcount++;
							else
								wcount++;
						}
						else
						{
							if (pixel == 0)
							{
								if (bcount)
									bcount++;
								else
								{
									wcount = 0;
									bcount++;
								}
							}
							else
							{
								if (wcount)
									wcount++;
								else
								{
									bcount = 0;
									wcount++;
								}
							}
						}

						sum += (wcount)*pixel;//(wcount-bcount)*pixel;
						break;
					case 2:
						//weight = (float)(exp(x) - 1.0f);
						weight = (float)(exp(x) - 1.0f);
						//weight = (float)3*(x+1);
						sum += (long)weight*pixel;
						break;
					case 3:
						if (x > Simg->width/2)
						{
							weight = (float)(exp(x - Simg->width/2) - 1.0f);
							sum += (long)weight*pixel;
						}
						break;
					default:
						break;
				}
			}

			proj.data.fl[y] = sum;
		}
	}
}

void CIplFace::ShowProjection(IplImage *Simg, IplImage *Dimg, int xdir, int ydir, int method)
{
	if ((xdir == 0) && (ydir == 0))	return;

	int val, i, MaxVal = 0, MinVal = 1e6;
	int diff = abs(CPsize.width - CPsize.height);

	// get projection
	if (xdir) iplProject(Simg, projX, 0, method);
	if (ydir) iplProject(Simg, projY, 1, method);

	iplSet(Dimg, 0);

	if (xdir) {

		for (i = 0; i < CPsize.width; i++) 
		{
			val = projX.data.fl[i];
			if (val > MaxVal)
				MaxVal = val;
			if (val < MinVal)
				MinVal = val;
		}

		for( i = 0; i < CPsize.width; i++ ) 
		{
			val = projX.data.fl[i];
			CvPoint p[4];
			
			p[0].x = p[1].x = i;
			p[2].x = p[3].x = (i+1);
			
			p[1].y = p[2].y = Simg->height - 1;
			p[0].y = p[3].y = Simg->height - (int) ((double)diff * (double)((double)(val-MinVal) / (double)(MaxVal-MinVal)));
			
			cvFillConvexPoly( Dimg, p, 4, CV_RGB(0,255,0));
		}
	}

	if (ydir) {

		MaxVal = 0, MinVal = 1e6;
		for (i = 0; i < CPsize.height; i++) 
		{
			val = projY.data.fl[i];
			if (val > MaxVal)
				MaxVal = val;
			if (val < MinVal)
				MinVal = val;
		}

		for( i = 0; i < CPsize.height; i++ ) 
		{
			val = projY.data.fl[i];
			CvPoint p[4];
			
			p[0].y = p[3].y = i;
			p[1].y = p[2].y = (i+1);
			
			p[2].x = p[3].x = CPsize.width - 1;
			p[0].x = p[1].x = CPsize.width - (int) ((double)diff * (double)((double)(val-MinVal) / (double)(MaxVal-MinVal)));
			
			cvFillConvexPoly( Dimg, p, 4, CV_RGB(0,0,255));
		}
	}
}

void CIplFace::iplOtsu1(IplImage *Simg, int *th0, int start)
{
	int th;
	int i;
	int bins = 256;
	static float bucket[256], pbucket[256], wbucket[256], mbucket[256], sbucket[256];
	long tbucket = 0;
	float wsum = 0.0f;
	float msum = 0.0f;
	float max = -1.0f;

	// get histogram
	cvCalcHist(&Simg, histG, 0, NULL);

	for (i = start; i < bins; i++) {
		bucket[i] = (float)(cvRound(cvQueryHistValue_1D( histG, i )));
		
		if (bucket[i] != 0.0f)
			tbucket += bucket[i];
	}

	for (i=start; i<bins; i++)
	{
		pbucket[i] = (float)((float)bucket[i]/(float)tbucket);
		wsum += pbucket[i];
		wbucket[i] = wsum;
		msum += (i+1)*pbucket[i];
		mbucket[i] = msum;
	}

	for (i=start; i<bins; i++)
	{
		sbucket[i] = 			
			//(float)((mbucket[nbuckets-1]*wbucket[i] - mbucket[i])*(mbucket[nbuckets-1]*wbucket[i] - mbucket[i]))/(wbucket[i]*(1 - wbucket[i]));
			(float)((msum*wbucket[i] - mbucket[i])*(msum*wbucket[i] - mbucket[i]))/(wbucket[i]*(1 - wbucket[i]));
	}

	for (i=start; i<bins; i++)
	{
		if (sbucket[i] > max)
		{
			max = sbucket[i];
			*th0 = i;
		}
	}
}

// method
// 0: IB, 1: OB, 2: GT, 3: LT, 4: OTSU_H, 5: OTSU_L
void CIplFace::iplThreshold2(IplImage *Simg, IplImage *Dimg, int low, int high, int method, float otsu)
{
	int th = 0;

	switch(method)
	{
		case 0:
			cvThreshold(Simg, procCPGimg[0], low, 255, CV_THRESH_BINARY);
			cvThreshold(Simg, procCPGimg[1], high, 255, CV_THRESH_BINARY_INV);
			iplAnd(procCPGimg[0], procCPGimg[1], Dimg);
			break;
		case 1:
			cvThreshold(Simg, procCPGimg[0], low, 255, CV_THRESH_BINARY);
			cvThreshold(Simg, procCPGimg[1], high, 255, CV_THRESH_BINARY_INV);
			iplAnd(procCPGimg[0], procCPGimg[1], procCPGimg[2]);
			iplNot(procCPGimg[2], Dimg);
			break;
		case 2:
			cvThreshold(Simg, Dimg, low, 255, CV_THRESH_BINARY);
			break;
		case 3:
			cvThreshold(Simg, Dimg, low, 255, CV_THRESH_BINARY_INV);
			break;
		case 4:
			iplOtsu1(Simg, &th, 0);
			cvThreshold(Simg, Dimg, (int)((float)th*otsu), 255, CV_THRESH_BINARY);
			break;
		case 5:
			iplOtsu1(Simg, &th, 0);
			cvThreshold(Simg, Dimg, (int)((float)th*otsu), 255, CV_THRESH_BINARY_INV);
			break;
		default:
			break;
	}
}

// 2002.03.06
// 20030501
void CIplFace::ShowFace(IplImage *Simg, IplImage *Dimg)
{
	//D:\User\DJKim\Research\Figure\PEkman\//
	int no, i, j;
	char str[100];
	char str_[100];
	float fee[7];
	float judge_no;
	int intent;
	CvPoint p1, p2;
	iplFaceStructure	face;

	FILE *fp, *fp_;
	//fp_ = fopen("K:\\User\\연구\\그림(DB)\\PEkman\\Fe_Code.txt", "rt");
	fp_ = fopen("K:\\User\\research\\faceDB\\ZBienDB\\kts1\\kts1.dat", "rt");

	// 031224
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CLogView* logview = mainframe->GetLogView();
	CString current_status;

	//
	//for (i=0; i<110; i++)
	for (i=0; i<7; i++)
	{
		//fscanf(fp_, "%s %f %f %f %f %f %f %f %f %d", str_, 
		//	&(fee[0]), &(fee[1]), &(fee[2]), &(fee[3]), &(fee[4]), &(fee[5]), &(fee[6]), &judge_no, &intent);
		fscanf(fp_, "%s %d", str_, &intent);

		//sprintf(str, "K:\\User\\연구\\그림(DB)\\PEkman\\%s.dat", str_);
		sprintf(str, "K:\\User\\research\\faceDB\\ZBienDB\\kts1\\%s.dat", str_);
		fp = fopen(str, "rt");

		fscanf(fp, "%d", &no);
		if (no != 12)	
		{
			fclose(fp);
			break;
		}

		face.emotions = intent;

		//sprintf(str, "K:\\User\\연구\\그림(DB)\\PEkman\\%s.jpg", str_);
		sprintf(str, "K:\\User\\research\\faceDB\\ZBienDB\\kts1\\%s.jpg", str_);

		IplImage *file = cvvLoadImage(str);

		// set each facial component from loaded data
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.left_eyebrow.SetRect(p1, p2);

		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.left_eye.SetRect(p1, p2);
		
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.right_eyebrow.SetRect(p1, p2);
		
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.right_eye.SetRect(p1, p2);
		
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.nose.SetRect(p1, p2);

		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.mouth.SetRect(p1, p2);

		face.SetTransient();

		cvvConvertImage(file, procCPCimg[0], 0);
		cvvConvertImage(procCPCimg[0], procCPGimg[0], 0);

		iplFaceFeatureExtract(procCPGimg[0], &face);

		//iplFaceDrawing(file, face);
		//cvvShowImage("Result Image", file);		
		//cvvWaitKey(0);
		
		//sprintf(str, "D:\\User\\DJKim\\Research\\Figure\\PEkman\\%s_.jpg", str_);
		//cvvSaveImage(str, file);

		current_status.Format("Image File: %s.jpg", str_);
		logview->AddLine(current_status);

		fclose(fp);
	}
	fclose(fp_);
}

// log-polar, bulge
void CIplFace::eyeFeature(IplImage *Simg, CvRect rect1, float feature)
{
	int th;

	// eye
	iplSet(procCPGimg[0], 0);
	cvRectangle(procCPGimg[0], cvPoint(rect1.x, rect1.y), 
		cvPoint(rect1.x+rect1.width, rect1.y+rect1.height), (int)(0xFFFFFF), -1);

	iplSet(procCPGimg[1], 0);
	iplAnd(Simg, procCPGimg[0], procCPGimg[1]);

	iplOtsu1(procCPGimg[1], &th, 0);

	// LT. bin
	iplThreshold2(Simg, procCPGimg[0], th, 0, 3, 1.0);

	//feature = (rect2.y+rect2.height/2 - rect1.y+rect1.height/2);
}

// bin
void CIplFace::rootFeature(IplImage *Simg, CvRect rect1, float feature)
{
	int th;

	cvvConvertImage(Simg, procCPGimg[0], 0);

	// root
	cvSetImageROI(procCPGimg[0], rect1);
	
	iplSet(procCPGimg[1], 0);

	cvSetImageROI(procCPGimg[1], rect1);
	iplCopy(procCPGimg[0], procCPGimg[1]);
	procCPGimg[1]->roi = NULL;

	procCPGimg[0]->roi = NULL;

	iplOtsu1(procCPGimg[1], &th, 0);

	// LT. bin
	iplThreshold2(procCPGimg[1], procCPGimg[2], th, 0, 3, 1.0);

	cvvShowImage("Result Image", procCPGimg[2]);
}

// bin, ori
void CIplFace::nlfFeature(IplImage *Simg, CvRect rect, float feature)
{
	/*
	int th;

	// eyebrows
	iplSet(procGimg[0], 0);
	cvRectangle(procGimg[0], cvPoint(rect1.x, rect1.y), 
		cvPoint(rect1.x+rect1.width, rect1.y+rect1.height), (int)(0xFFFFFF), -1);

	iplSet(procGimg[1], 0);
	iplAnd(Simg, procGimg[0], procGimg[1]);

	iplOtsu1(procGimg[1], &th, 0);

	// LT. bin
	iplThreshold2(Simg, procGimg[0], th, 0, 3, 1.0);

	// eyes
	iplSet(procGimg[0], 0);
	cvRectangle(procGimg[0], cvPoint(rect2.x, rect2.y), 
		cvPoint(rect2.x+rect2.width, rect2.y+rect2.height), (int)(0xFFFFFF), -1);

	iplSet(procGimg[1], 0);
	iplAnd(Simg, procGimg[0], procGimg[1]);

	iplOtsu1(procGimg[1], &th, 0);

	// LT. bin
	iplThreshold2(Simg, procGimg[0], th, 0, 3, 1.0);
	*/

	//feature = (rect2.y+rect2.height/2 - rect1.y+rect1.height/2);
}

// gabor
void CIplFace::mouthFeature(IplImage *Simg, CvRect rect, float feature)
{
	/*
	int th;

	// eyebrows
	iplSet(procGimg[0], 0);
	cvRectangle(procGimg[0], cvPoint(rect1.x, rect1.y), 
		cvPoint(rect1.x+rect1.width, rect1.y+rect1.height), (int)(0xFFFFFF), -1);

	iplSet(procGimg[1], 0);
	iplAnd(Simg, procGimg[0], procGimg[1]);

	iplOtsu1(procGimg[1], &th, 0);

	// LT. bin
	iplThreshold2(Simg, procGimg[0], th, 0, 3, 1.0);

	// eyes
	iplSet(procGimg[0], 0);
	cvRectangle(procGimg[0], cvPoint(rect2.x, rect2.y), 
		cvPoint(rect2.x+rect2.width, rect2.y+rect2.height), (int)(0xFFFFFF), -1);

	iplSet(procGimg[1], 0);
	iplAnd(Simg, procGimg[0], procGimg[1]);

	iplOtsu1(procGimg[1], &th, 0);

	// LT. bin
	iplThreshold2(Simg, procGimg[0], th, 0, 3, 1.0);
	*/

	//feature = (rect2.y+rect2.height/2 - rect1.y+rect1.height/2);
}

// dist
void CIplFace::disteyeFeature(IplImage *Simg, CvRect rect1, CvRect rect2, float feature)
{
}

void CIplFace::ShowSubFace(IplImage *Simg, IplImage *Dimg, CvRect rect, int method)
{
	int th = 0;
	int i,j;
	double param[2] = {1, 1.0};

	cvSetImageROI(Simg, rect);
	cvSetImageROI(procCPGimg[0], rect);
	iplCopy(Simg, procCPGimg[0]);
	procCPGimg[0]->roi = NULL;
	Simg->roi = NULL;

	CSVV svv(1);
	IplImage *procimg;
	IplImage *xmap2, *ymap2;

	float value, x, y;
	char str[100];

	FILE *fp;

	switch(method)
	{
		// NLF feature
		case 0:
			// get bin. image (GT) by otsu's method with 0.8
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 0.8);

			// count nonzero pixels
			cvSetImageROI(procCPGimg[1], rect);
			sprintf(str, "%d", cvCountNonZero(procCPGimg[1]));
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));

			break;
		case 1:
			iplSet(procCPGimg[1], 0);
			iplFixedFilter(procCPGimg[0], procCPGimg[1], IPL_SOBEL_3x3_H);
			//iplOtsu1(procCPGimg[2], &th, 0);
			//iplSet(procCPGimg[1], 0);
			//iplThreshold2(procCPGimg[2], procCPGimg[1], th, 0, 4, 1.5);
			break;
		case 2:
			//double param[2] = {1, 1.0};
			iplSet(procCPGimg[1], 0);
			//cvThreshold(procCPGimg[0], procCPGimg[1], 10, 255, CV_THRESH_BINARY_INV);
			cvAdaptiveThreshold(procCPGimg[0], procCPGimg[1], 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5.0);
			break;
		case 3:
			iplSet(procCPGimg[1], 0);
			cvCanny(procCPGimg[0], procCPGimg[1], 1, 255, 3);
			break;
		case 4:
			iplCopy(procCPGimg[0], procCPGimg[1]);
			break;
		// NLR feature
		case 5:
			// get bin. image (GT) by otsu's method with 0.5
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 0.5);
			
			// count nonzero pixels
			cvSetImageROI(procCPGimg[1], rect);
			sprintf(str, "%d", cvCountNonZero(procCPGimg[1]));
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));

			break;
		case 6:
			// 0: get bin. image (GT)
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], 64, 0, DJ_IPL_GT, 0.5);
			
			// count nonzero pixels
			cvSetImageROI(procCPGimg[1], rect);
			sprintf(str, "%d", cvCountNonZero(procCPGimg[1]));
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));

			break;
		case 7:
			// 0: get bin. image (GT) otsu, 1.5
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 1.5);

			procimg = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, 1);

			// space-variant vision
			svv.LPtable(rect.width, rect.height, LPsize.width, LPsize.height);

			// x & y map for remapping
			xmap2 = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);
			ymap2 = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);

			for (i=0; i<LPsize.width; i++)
				for (j=0; j<LPsize.height; j++)
				{
					x = (float)svv.btrX[i*LPsize.height+j];
					y = (float)svv.btrY[i*LPsize.height+j];

					iplPutPixel(xmap2, i, j, &x);
					iplPutPixel(ymap2, i, j, &y);
				}

			cvSetImageROI(procCPGimg[1], rect);
			iplRemap(procCPGimg[1], xmap2, ymap2, procLPGimg[0], IPL_INTER_LINEAR);
			procCPGimg[1]->roi = NULL;

			iplProject(procLPGimg[0], projXLP, 0);

			for (i=0; i<LPsize.width; i++)
			{
				if (projXLP.data.fl[i] > 10)
				{
					value = (float)i;
					break;
				}
			}

			value /= LPsize.width;

			fp = fopen(".\\eyeLP.txt", "at");

			fprintf(fp, "%4.2f ", value);

			fclose(fp);


			cvReleaseImage(&(ymap2));
			cvReleaseImage(&(xmap2));
			cvReleaseImage(&(procimg));

			/*
			// count nonzero pixels
			cvSetImageROI(procCPGimg[1], rect);
			sprintf(str, "%d", cvCountNonZero(procCPGimg[1]));
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));
			*/

			break;
		default:
			break;
	}

	cvSetImageROI(procCPGimg[1], rect);
	cvSetImageROI(Dimg, rect);
	iplCopy(procCPGimg[1], Dimg);
	Dimg->roi = NULL;
	procCPGimg[1]->roi = NULL;
}

// 2002.4.15
void CIplFace::GGfeature(IplImage *Simg, float wave, float theta, float *feature)
{
	int i, j;
	static int k=0;

	//float mG, sG, fG;
	//float wSum, wGSum;
	float min, max, scale, val;
	int th;
	float pix1, pix2;

	//gabor.making_gabor_filter4(wave, 1.5, M_PI*((45.0)/180.0), M_PI*((theta)/180.0), &real_part, &img_part);

	iplScaleFP(Simg, procCPFimg[0], 0.0, 255.0);
	iplConvolve2DFP(procCPFimg[0], procCPFimg[1], kernel[0], 2, IPL_SUMSQROOT);

	// image normalization
	iplMinMaxFP(procCPFimg[1], &min, &max);
	iplSubtractSFP(procCPFimg[1], procCPFimg[3], min, 0);
	scale = (float)((float)(255.0)/(max - min));
	iplMultiplySFP(procCPFimg[3], procCPFimg[4], scale);

	iplScaleFP(procCPFimg[4], procCPGimg[0], 0, 255);
	// image normalization
	cvvShowImage("Result Image", procCPGimg[0]);
	
	/*
	char str[100];
	sprintf(str, ".\\gabor%03d.tif", k++);
	iplGrayToColor(procCPGimg[0], procCPCimg[0], 1, 1, 1);
	cvvSaveImage(str, procCPCimg[0]);
	*/

	/*
	double *gabor_co = new double[A.w()*A.h()];

	for(i=0;i<A.w()*A.h();i++)
		  gabor_co[i] = sqrt((bank1[i])*(bank1[i])+(bank2[i])*(bank2[i]));

	double max = -1;
	for(i=0;i<A.w()*A.h();i++)
		if (gaborImg[i] > max)
			max = gaborImg[i];

	Hist1d hist(65536, -1, 256);

	hist.addtohist(A.buf[0], A.w()*A.h());
	hist.DJotsu(&th);

	//double th = max*(0.2);
	for(i=0;i<A.w()*A.h();i++)
	{
		if (gaborImg[i] > th)
			gaborImg[i] = 1.0;
		else
			gaborImg[i] = 0.0;
	}

	double *proj = gabor.projCo(gaborImg,A.w(),A.h(),1);

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
	delete [] wG;
	delete [] dproj;
	delete [] proj;
	*/

	//iplDeleteConvKernelFP(img_kernel);
}

void CIplFace::iplNormalize(IplImage *Simg, IplImage *Dimg)
{
	float min, max, scale;

	if (Simg->depth != IPL_DEPTH_32F)	return;

	//iplScaleFP(Simg, procCPFimg[6], 0.0, 255.0);
	//iplConvolve2DFP(procCPFimg[0], procCPFimg[1], kernel[0], 2, IPL_SUMSQROOT);

	// image normalization
	iplMinMaxFP(Simg, &min, &max);
	//iplMinMaxFP(procCPFimg[6], &min, &max);
	iplSubtractSFP(Simg, procCPFimg[6], min, 0);
	scale = (float)((float)(255.0)/(max - min));
	iplMultiplySFP(procCPFimg[6], procCPFimg[7], scale);

	iplScaleFP(procCPFimg[7], Dimg, 0, 255);
}

void CIplFace::iplFaceSubFeature(IplImage *Simg, iplFaceSubComponent *comp, int method)
{
	int th = 0;
	int i, j;
	double param[2] = {1, 1.0};

	CvRect rect = comp->rect;

	cvSetImageROI(Simg, rect);
	cvSetImageROI(procCPGimg[0], rect);
	iplCopy(Simg, procCPGimg[0]);
	procCPGimg[0]->roi = NULL;
	Simg->roi = NULL;

	float value, x, y;
	char str[100];

	CSVV myvv(1);
	
	register __int64 freq, start, end; 
	register float Time1, Time2;
	register bool err;

	FILE *fp;
	FILE *fp2;

	switch(method)
	{
		// NLF feature
		case 0:
			// get bin. image (GT) by otsu's method with 0.8
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 0.8);

			// count nonzero pixels
			cvSetImageROI(procCPGimg[1], rect);
			comp->feature = (float)cvCountNonZero(procCPGimg[1]);
			sprintf(str, "%d", comp->feature);
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));

			break;
		case 1:
			iplSet(procCPGimg[1], 0);
			iplFixedFilter(procCPGimg[0], procCPGimg[1], IPL_SOBEL_3x3_H);
			break;
		case 2:
			//double param[2] = {1, 1.0};
			iplSet(procCPGimg[1], 0);
			//cvThreshold(procCPGimg[0], procCPGimg[1], 10, 255, CV_THRESH_BINARY_INV);
			cvAdaptiveThreshold(procCPGimg[0], procCPGimg[1], 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 3, 5.0);
			break;
		case 3:
			iplSet(procCPGimg[1], 0);
			cvCanny(procCPGimg[0], procCPGimg[1], 1, 255, 3);
			break;
		case 4:
			iplCopy(procCPGimg[0], procCPGimg[1]);
			break;
		// NLR feature
		case 5:
			// get bin. image (GT) by otsu's method with 0.5
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 0.5);
			
			cvSetImageROI(procCPGimg[1], rect);
			comp->feature = (float)cvCountNonZero(procCPGimg[1]);
			sprintf(str, "%d", comp->feature);
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));

			break;
		case 6:
			// 0: get bin. image (GT)
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], 64, 0, DJ_IPL_GT, 0.5);
			
			cvSetImageROI(procCPGimg[1], rect);
			comp->feature = (float)cvCountNonZero(procCPGimg[1]);
			sprintf(str, "%d", comp->feature);
			procCPGimg[1]->roi = NULL;
			cvPutText(procCPGimg[1], str, cvPoint(rect.x, rect.y + 10), &font, (int)(0xAAAAAA));
			
			break;
		case 7:

			CHECK_TIME_START;
			// eye openness 1: LP feature.
			// get bin. image (GT) otsu, 1.5
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 1.8);

			procCPimg = cvCreateImage(cvSize(rect.width, rect.height), IPL_DEPTH_8U, 1);

			// space-variant vision
			myvv.LPtable(rect.width, rect.height, LPsize.width, LPsize.height);

			// x & y map for remapping
			xmap2 = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);
			ymap2 = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);

			for (i=0; i<LPsize.width; i++)
				for (j=0; j<LPsize.height; j++)
				{
					x = (float)myvv.btrX[i*LPsize.height+j];
					y = (float)myvv.btrY[i*LPsize.height+j];

					iplPutPixel(xmap2, i, j, &x);
					iplPutPixel(ymap2, i, j, &y);
				}

			cvSetImageROI(procCPGimg[1], rect);
			iplCopy(procCPGimg[1], procCPimg);
			iplRemap(procCPimg, xmap2, ymap2, procLPGimg[0], IPL_INTER_LINEAR);
			procCPGimg[1]->roi = NULL;
			CHECK_TIME_END(Time1, err);

			CHECK_TIME_START;
			iplProject(procLPGimg[0], projXLP, 0);

			value = 0.0f;

			for (i=0; i<LPsize.width; i++)
			{
				if (projXLP.data.fl[i] > 1e1)
				{
					value = (float)i;
					break;
				}
			}

			if (((float)LPsize.width != 0.0f) && (value < LPsize.width))
				comp->feature = (float)((float)value/(float)LPsize.width);
			else
				comp->feature = 0.0f;

			CHECK_TIME_END(Time2, err);

			//fp = fopen(".\\eyeLP.txt", "awt");
			//fprintf(fp, "%4.2f ", value);
			//fclose(fp);

			cvReleaseImage(&(ymap2));
			cvReleaseImage(&(xmap2));
			cvReleaseImage(&(procCPimg));

			fp2 = fopen("c:\\matlabr11\\work\\timeEyeLP.txt", "at");
			fprintf(fp2, "%f %f \n", Time1, Time2);
			fclose(fp2);

			break;
		case 8:
			// eye openness 2: gabor feature
			// get bin. image (GT) by otsu's method with 0.5
			iplSet(procCPGimg[1], 0);
			iplThreshold2(procCPGimg[0], procCPGimg[1], th, 0, 4, 0.5);
			
			cvSetImageROI(procCPGimg[1], rect);
			comp->feature = (float)cvCountNonZero(procCPGimg[1]);
			
			/*
			fp = fopen(".\\eyeLP.txt", "awt");
			comp->feature = (float)cvCountNonZero(procCPGimg[1]);
			fprintf(fp, "%4.2f \n", comp->feature);
			fclose(fp);
			*/
			procCPGimg[1]->roi = NULL;

			break;
		default:
			break;
	}
}

void CIplFace::iplLPM(IplImage *Simg, IplImage *Dimg)
{
	iplRemap(Simg, xmap, ymap, Dimg, IPL_INTER_LINEAR);
}

void CIplFace::iplLPMbulge(IplImage *Simg, IplImage *Dimg, float *bulge)
{
	int i;
	char str[100];

	iplRemap(Simg, xmap, ymap, Dimg, IPL_INTER_LINEAR);
	iplProject(Dimg, projXLP, 0);

	for (i=0; i<LPsize.width; i++)
	{
		if (projXLP.data.fl[i] > 0)
		{
			*bulge = (float)i;
			break;
		}
	}

	cvLine(Dimg, cvPoint(*bulge, 0), cvPoint(*bulge, LPsize.height - 1), (int)(0xCCCCCC), 1);

	sprintf(str, "%3.1f", (float)(*bulge)/(float)(LPsize.width));
	cvPutText(Dimg, str, cvPoint(0, 10), &font, (int)(0x999999));

	iplGrayToColor(Dimg, procLPCimg[0], 1, 1, 1);
	cvvSaveImage(".\\result.tif", procLPCimg[0]);
}

void CIplFace::iplLPMangle(IplImage *Simg, IplImage *Dimg, float *angle)
{
	int i;
	float sum = 0.0f, wsum = 0.0f, cog = 0.0f;
	char str[100];

	iplRemap(Simg, xmap, ymap, Dimg, IPL_INTER_LINEAR);
	iplProject(Dimg, projYLP, 1, 3); // half - weighted

	for (i=0; i<LPsize.height/2; i++)
	{
		sum += (float)(projYLP.data.fl[i]);
		wsum += (float)(projYLP.data.fl[i])*i;
	}

	cog = (float)(wsum/sum);

	int start, end;

	if (projYLP.data.fl[(int)cog] < 1e1)//exp(LPsize.width/2) - 1 - LPsize.width/2)
	{
		sum = wsum = 0.0f;

		if ((int)cog < LPsize.height/4)
		{
			start = 0;
			end = (int)cog - 1;
		}
		else
		{
			start = (int)cog + 1;
			end = LPsize.height/2;
		}

		for (i=start; i<end; i++)
		{
			sum += (float)(projYLP.data.fl[i]);
			wsum += (float)(projYLP.data.fl[i])*i;
		}

		cog = (float)(wsum/sum);
	}

	*angle = (float)(cog*180.0f/(float)(LPsize.height/2));

	cvLine(Dimg, cvPoint(0, cog), cvPoint(LPsize.width - 1, cog), (int)(0xCCCCCC), 1);

	sprintf(str, "%5.1f", *angle);
	cvPutText(Dimg, str, cvPoint(0, 10), &font, (int)(0x999999));
}

void CIplFace::iplFaceDrawing(IplImage *Simg, iplFaceStructure face)
{
	cvRectangle(Simg, face.left_eyebrow.tl(), face.left_eyebrow.br(), (int)(0xFF0000), 1);
	cvRectangle(Simg, face.right_eyebrow.tl(), face.right_eyebrow.br(), (int)(0xFF0000), 1);
	cvRectangle(Simg, face.left_eye.tl(), face.left_eye.br(), (int)(0xFF0000), 1);
	cvRectangle(Simg, face.right_eye.tl(), face.right_eye.br(), (int)(0xFF0000), 1);
	cvRectangle(Simg, face.nose.tl(), face.nose.br(), (int)(0xFF0000), 1);
	cvRectangle(Simg, face.mouth.tl(), face.mouth.br(), (int)(0xFF0000), 1);

	cvRectangle(Simg, face.nasolabial_root.tl(), face.nasolabial_root.br(), (int)(0x0000FF), 1);
	cvRectangle(Simg, face.left_eye_feet.tl(), face.left_eye_feet.br(), (int)(0x0000FF), 1);
	cvRectangle(Simg, face.right_eye_feet.tl(), face.right_eye_feet.br(), (int)(0x0000FF), 1);
	cvRectangle(Simg, face.left_nasolabial_fold.tl(), face.left_nasolabial_fold.br(), (int)(0x0000FF), 1);
	cvRectangle(Simg, face.right_nasolabial_fold.tl(), face.right_nasolabial_fold.br(), (int)(0x0000FF), 1);
	cvRectangle(Simg, face.left_crow_feet.tl(), face.left_crow_feet.br(), (int)(0x0000FF), 1);
	cvRectangle(Simg, face.right_crow_feet.tl(), face.right_crow_feet.br(), (int)(0x0000FF), 1);
}

// 020506: T-shape, including angular variation
void CIplFace::iplFaceDTM(IplImage *Simg, IplImage *Dimg, iplFaceStructure *face)
{
	float maxc = -1.0f, curc;
	float theta;
	CvPoint maxpt;
	int i, j, inc = 4;
	CvRect rect = face->face.GetRect();
	iplFaceTemplate facetmp(0, 0, 0.25*rect.width, 0.25*rect.width, 0.375*rect.height, 2, 4, 0.0f, 0.0f);

	iplSet(procCPGimg[0], 0);
	Simg->maskROI = face->mask;
	procCPGimg[0]->maskROI = face->mask;
	iplFixedFilter(Simg, procCPGimg[0], IPL_SOBEL_3x3_H);
	//iplNot(Simg, procCPGimg[1]);
	procCPGimg[0]->maskROI = NULL;
	Simg->maskROI = NULL;
	
	//iplOr(procCPGimg[0], procCPGimg[1], procCPGimg[0]);
	//iplThreshold2(procCPGimg[0], procCPGimg[1], 0, 0, DJ_IPL_OTSU_H, 1.5);
	iplThreshold2(procCPGimg[0], procCPGimg[1], 0, 0, DJ_IPL_OTSU_H, 1.0);

	// 2002/5/3 0:38 am
	for (j=rect.y + rect.height/5; j<rect.y + rect.height/2 - rect.height/5; j += inc)
		for (i=rect.x; i<rect.x + rect.width; i += inc)
		{
			facetmp.x = i;
			facetmp.y = j;
			facetmp.iplCorrEdge(procCPGimg[1], &curc);

			//cvvShowImage("Result Image", procCPGimg[3]);

			if (curc > maxc)
			{
				maxc = curc;			
				maxpt = cvPoint(i, j);
			}
		}

	maxc = -1.0f;
	float maxth;
	for (i=-30; i<31; i+=5)
	{
		facetmp.x = maxpt.x;
		facetmp.y = maxpt.y;
		facetmp.au = i;
		facetmp.iplCorrEdge(procCPGimg[1], &curc);

		if (curc > maxc)
		{
			maxc = curc;			
			maxth = (float)RAD_DEG*i;
		}
	}

	//maxth = RAD_DEG*30.0;
	//iplSet(procCPGimg[1], 0);
	cvLine(procCPGimg[1], cvPoint(maxpt.x, maxpt.y+2), cvPoint(maxpt.x + facetmp.dr*cos(maxth), maxpt.y+2 - facetmp.dr*sin(maxth)), (int)(0xAAAAAA), facetmp.tu);
	cvLine(procCPGimg[1], cvPoint(maxpt.x, maxpt.y+2), cvPoint(maxpt.x - facetmp.dl*cos(maxth), maxpt.y+2 + facetmp.dl*sin(maxth)), (int)(0xAAAAAA), facetmp.tu);
	cvLine(procCPGimg[1], cvPoint(maxpt.x, maxpt.y-2), cvPoint(maxpt.x + facetmp.dr*cos(maxth), maxpt.y-2 - facetmp.dr*sin(maxth)), (int)(0xAAAAAA), facetmp.tu);
	cvLine(procCPGimg[1], cvPoint(maxpt.x, maxpt.y-2), cvPoint(maxpt.x - facetmp.dl*cos(maxth), maxpt.y-2 + facetmp.dl*sin(maxth)), (int)(0xAAAAAA), facetmp.tu);
	cvLine(procCPGimg[1], cvPoint(maxpt.x, maxpt.y), cvPoint(maxpt.x + facetmp.dd*sin(maxth), maxpt.y + facetmp.dd*cos(maxth)), (int)(0xAAAAAA), facetmp.td);

	//iplOr(procCPGimg[1], procCPGimg[0], procCPGimg[1]);			
	cvCopyImage(procCPGimg[1], Dimg);
}

void CIplFace::iplFaceCHAT(IplImage *Simg, iplFaceStructure *face, int *th)
{
	int i;
	int val = 0;
	int rngth = 1e2;
	int cogth;
	int maxval = 0;
	int start = 1, end = 256;
	float cog = 0.0f;
	long sum, wsum;

	iplSet(procCPGimg[2], 0);
	Simg->maskROI = face->mask;
	iplCopy(Simg, procCPGimg[2]);
	Simg->maskROI = NULL;

	cvCalcHist(&(procCPGimg[2]), histG, 0, NULL);

	// 1. get COG & maxval
	sum = wsum = 0L;

	for (i=start; i<end; i++)
	{
		val = cvRound(cvQueryHistValue_1D( histG, i ));

		sum += (long)(val);
		wsum += (long)(val*i);
		
		if (maxval < val)
			maxval = val;
	}

	cog = (float)((float)wsum/(float)sum);
	cogth = (int)((float)maxval*(float)0.1);

	// 2. draw HISTOGRAM
	iplSet(procCPGimg[4], 0);
	CvPoint p[4];

	for (i=start; i<end; i++)
	{
		val = cvRound(cvQueryHistValue_1D( histG, i ));

		p[0].x = p[1].x = i * Simg->width/256;
		p[2].x = p[3].x = (i+1) * Simg->width/256;
		
		p[1].y = p[2].y = Simg->height - 1;
		p[0].y = p[3].y = Simg->height - (int) ((float)Simg->height * (float)((float)val / (float)maxval));
		
		cvFillConvexPoly( procCPGimg[4], p, 4, CV_RGB(128,128,128));
	}

	// 3. Search for new threshold
	val = cvRound(cvQueryHistValue_1D( histG, (int)cog));
	
	if (val > rngth)	// In range
	{
		for (i=cog-1; i>=start; i--)
		{
			val = cvRound(cvQueryHistValue_1D( histG, i));
			if (val < cogth)
			{
				/*
				if (th[0] > i)
					th[0]--;
				else if (th[0] < i)
					th[0]++;
					*/
				th[0] = i;
				//th[0] = (th[0] + i)/2;
				break;
			}
		}

		for (i=cog+1; i<end; i++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, i));
			if (val < cogth)
			{
				/*
				if (th[1] < i)
					th[1]++;
				else if (th[1] > i)
					th[1]--;
					*/
				th[1] = i;
				//th[1] = (th[1] + i)/2;
				break;
			}
		}
	}
	/*
	else			// Out of range
	{
		for (i=cog-1; i>=start; i--)
		{
			val = cvRound(cvQueryHistValue_1D( histG, i));
			if (val > cogth)
			{
				//th[0] = i;
				th[0] = (th[0] + i)/2;
				break;
			}
		}

		for (i=cog+1; i<end; i++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, i));
			if (val > cogth)
			{
				//th[1] = i;
				th[1] = (th[1] + i)/2;
			}
		}
	}
	*/
}

void CIplFace::iplLTanal(int rindex,int gindex, char pstr[])
{
	int i,j,ii, l;
	int sample_no = 10;
	int test_no = 10;
	int test_offset = 0;

	char pixel;
	char cpixel[3];

	double max = -1;
	char str[100];
	//BinaryImage A;
	//ColorImage Z;
	IplImage *A, *Z, *Y;// = cvCreateImage(LPsize, IPL_DEPTH_8U, 1);
	double LT[256][256];
	long imgSize;

	double *CLT;
	IplImage *C; //BinaryImage C;
	double th_tmp = 0.0;
	double Cr,SEr,NSEr,Sr;

	IplImage *fileimg;

	FILE *fp;
	sprintf(str, "c:\\matlabr11\\work\\lt_%s.dat", pstr);
	fp = fopen(str,"aw");
	FILE *fp2;
	fp2 = fopen("d:\\facedb\\crl_skin_db\\skin-list-good-dj", "rt");

	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			LT[i][j] = 0.0;

	// Training stage.
	for(ii=0; ii<sample_no; ii++)
	{
		l = ii + 1;

		fscanf(fp2, "%s", str);

		// load training image.
		if (strstr(str, "jpg"))
			sprintf(str,"d:\\facedb\\crl_skin_db\\skin-images\\skin%04d.jpg", l);
		else if (strstr(str, "gif"))
			sprintf(str,"d:\\facedb\\crl_skin_db\\skin-images\\skin%04d.gif", l);
		//Z.load(str, 101);
		fileimg = cvvLoadImage(str);
		Z = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 3);
		cvvConvertImage(fileimg, Z);
		Y = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 3);

		// load ground truth image.
		sprintf(str,"d:\\facedb\\crl_skin_db\\masks\\mask%04d.tif", l);
		fileimg = cvvLoadImage(str);
		A = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 1);
		cvvConvertImage(fileimg, A);

		imgSize = Z->width*Z->height;

		// color conversion
		if (pstr == "HLS")
			iplRGB2HLS(Z, Y);
		else if (pstr == "HSV")
			iplRGB2HSV(Z, Y);
		else if (pstr == "LUV")
			iplRGB2LUV(Z, Y);
		else if (pstr == "XYZ")
			iplRGB2XYZ(Z, Y);
		else if (pstr == "YCrCb")
			iplRGB2YCrCb(Z, Y);
		else if (pstr == "YUV")
			iplRGB2YUV(Z, Y);
		else if (pstr == "I1I2I3")
			iplApplyColorTwist(Z, Y, twist_I1I2I3, 0);
		else if (pstr == "I1I2I3N")
			iplApplyColorTwist(Z, Y, twist_I1I2I3N, 0);

		// make LT. using color information.
		for (j=0; j<A->height; j++)
			for (i=0; i<A->width; i++)
			{
				iplGetPixel(A, i, j, &pixel);
				iplGetPixel(Y, i, j, cpixel);

				if (pixel == 255)
				{
					LT[cpixel[rindex]][cpixel[gindex]]++;

					if (LT[cpixel[rindex]][cpixel[gindex]] >= max)
						max = LT[cpixel[rindex]][cpixel[gindex]];
				}
			}
	}

	fclose(fp2);

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

	// Testing stage.
	fp2 = fopen("d:\\facedb\\crl_skin_db\\skin-list-good-dj", "rt");

	for (i=0; i<test_offset; i++)
		fscanf(fp2, "%s", str);

	for(ii=0; ii<test_no; ii++)
	{
		l = ii+1+test_offset;

		fscanf(fp2, "%s", str);
		// load training image.
		if (strstr(str, "jpg"))
			sprintf(str,"d:\\facedb\\crl_skin_db\\skin-images\\skin%04d.jpg", l);
		else if (strstr(str, "gif"))
			sprintf(str,"d:\\facedb\\crl_skin_db\\skin-images\\skin%04d.gif", l);
		fileimg = cvvLoadImage(str);
		//Z = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 3);
		cvvConvertImage(fileimg, Z);

		// load ground truth image.
		sprintf(str,"d:\\facedb\\crl_skin_db\\masks\\mask%04d.tif", l);
		fileimg = cvvLoadImage(str);
		//A = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 1);
		cvvConvertImage(fileimg, A);

		imgSize = Z->width*Z->height;
		CLT = new double[imgSize];
		C = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 1);
		cvCopyImage(A, C);

		// color conversion
		if (pstr == "HLS")
			iplRGB2HLS(Z, Y);
		else if (pstr == "HSV")
			iplRGB2HSV(Z, Y);
		else if (pstr == "LUV")
			iplRGB2LUV(Z, Y);
		else if (pstr == "XYZ")
			iplRGB2XYZ(Z, Y);
		else if (pstr == "YCrCb")
			iplRGB2YCrCb(Z, Y);
		else if (pstr == "YUV")
			iplRGB2YUV(Z, Y);
		else if (pstr == "I1I2I3")
			iplApplyColorTwist(Z, Y, twist_I1I2I3, 0);
		else if (pstr == "I1I2I3N")
			iplApplyColorTwist(Z, Y, twist_I1I2I3N, 0);

		for (j=0; j<Z->height; j++)
			for (i=0; i<Z->width; i++)
			{
				iplGetPixel(Y, i, j, cpixel);
				CLT[i+j*Z->width] = LT[cpixel[rindex]][cpixel[gindex]];
			}

		//CImgMisc::Normalize(CLT,Z.w(),Z.h());

		th_tmp = 0.0;

		do {
			
			for (j=0; j<C->height; j++)
				for (i=0; i<C->width; i++)
				{
					pixel = (CLT[i+C->width*j] >= th_tmp) ? 255 : 0;
					iplPutPixel(C, i, j, &pixel);
				}

			grTruthChk(A, C, Cr, SEr, NSEr, Sr);

			fprintf(fp,"%.3f %.3f %.3f %.3f %.3f\n", th_tmp, Cr, SEr, NSEr, Sr);
			
			th_tmp += 0.01;
		} while (th_tmp <= 1.0);

		delete []CLT;
	}

	
	cvReleaseImage(&(C));
	cvReleaseImage(&(Z));
	cvReleaseImage(&(Y));
	cvReleaseImage(&(A));

	fclose(fp);
	fclose(fp2);
}

void CIplFace::grTruthChk(IplImage *grImg, IplImage *segImg, double &Correct, double &SkinError, double &NonSkinError, double &Skin)
{
	// pixel number for total, skin, non-skin
	double tPix, sPix = 0, nsPix = 0;
	tPix = grImg->width*grImg->height;

	double sPixHit = 0, nsPixHit = 0;
	double sePixHit = 0, nsePixHit = 0;

	char pixel1, pixel2;
	int i, j;
	
	for (j=0; j<grImg->height; j++)
		for (i=0; i<grImg->width; i++)
		{
			iplGetPixel(grImg, i, j, &pixel1);
			iplGetPixel(segImg, i, j, &pixel2);

			if (pixel1 == 255)
			{
				sPix++;

				if (pixel2 == 255)
					sPixHit++;
				else
					sePixHit++;
			}
			else
			{
				nsPix++;

				if (pixel2 == 0)
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