// CT21IVSTK.cpp: implementation of the CCT21IVSTK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "IVSTK.h"

#include "MainFrm.h"

#include "ImageBaseView.h"
#include "LogView.h"
#include "math.h"
#include "SVV.h"
#include "GaborFilter.h"

#include "cvaux.h"

//#include <afxcoll.h>

#define M_PI	(3.1415926535897932384626433832795)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CIVSTKface::load(const char* str)
{
	int num;
	CvPoint p1, p2;

	FILE *fp;

	fp = fopen(str, "rt");

	fscanf(fp, "%d ", &num);

	if (num < 12)
	{
		fclose(fp);
		return;
	}
	else
	{
		// set each facial component from loaded data
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		left_eyebrow.SetRect(p1, p2);

		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		right_eyebrow.SetRect(p1, p2);
		
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		left_eye.SetRect(p1, p2);
		
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		right_eye.SetRect(p1, p2);
		
		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		nose.SetRect(p1, p2);

		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		mouth.SetRect(p1, p2);

		fscanf(fp, "%d %d %d %d", &(p1.x), &(p1.y), &(p2.x), &(p2.y));
		face.SetRect(p1, p2);

		SetTransient();
	}

	fclose(fp);
}

void CIVSTKface::save(const char* str)
{
	CvPoint p1, p2;

	FILE *fp;

	fp = fopen(str, "wt");

	fprintf(fp, "12 \n");

	// set each facial component from loaded data
	p1 = left_eyebrow.tl();
	p2 = left_eyebrow.br();
	fprintf(fp, "%d %d \n %d %d \n", (p1.x), (p1.y), (p2.x), (p2.y));
	
	p1 = right_eyebrow.tl();
	p2 = right_eyebrow.br();
	fprintf(fp, "%d %d \n %d %d \n", (p1.x), (p1.y), (p2.x), (p2.y));

	p1 = left_eye.tl();
	p2 = left_eye.br();
	fprintf(fp, "%d %d \n %d %d \n", (p1.x), (p1.y), (p2.x), (p2.y));

	p1 = right_eye.tl();
	p2 = right_eye.br();
	fprintf(fp, "%d %d \n %d %d \n", (p1.x), (p1.y), (p2.x), (p2.y));

	p1 = nose.tl();
	p2 = nose.br();
	fprintf(fp, "%d %d \n %d %d \n", (p1.x), (p1.y), (p2.x), (p2.y));

	p1 = mouth.tl();
	p2 = mouth.br();
	fprintf(fp, "%d %d \n %d %d \n", (p1.x), (p1.y), (p2.x), (p2.y));

	p1 = face.tl();
	p2 = face.br();
	fprintf(fp, "%d %d \n %d %d", (p1.x), (p1.y), (p2.x), (p2.y));

	fclose(fp);
}

void CIVSTKface::savefeature(const char* str)
{
	FILE *fp;

	fp = fopen(str, "wt");

	fprintf(fp, "%.3f %.3f %.3f %.3f %.3f", 
		__max(left_eye.feature, right_eye.feature),
		nasolabial_root.feature,
		__max(left_nasolabial_fold.feature, right_nasolabial_fold.feature), 
		mouth.feature,
		__max(left_eyebrow.feature, right_eyebrow.feature));

	fclose(fp);
}

void CIVSTKface::draw(IPLIMAGE src)
{
	cvRectangle(src, left_eyebrow.tl(), left_eyebrow.br(), 0xAA, 2);
	cvRectangle(src, right_eyebrow.tl(), right_eyebrow.br(), 0xAA, 2);
	
	cvRectangle(src, left_eye.tl(), left_eye.br(), 0xAA, 2);
	cvRectangle(src, right_eye.tl(), right_eye.br(), 0xAA, 2);
	
	cvRectangle(src, nasolabial_root.tl(), nasolabial_root.br(), 0xAA, 2);
	
	cvRectangle(src, nose.tl(), nose.br(), 0xAA, 2);

	cvRectangle(src, left_nasolabial_fold.tl(), left_nasolabial_fold.br(), 0xAA, 2);
	cvRectangle(src, right_nasolabial_fold.tl(), right_nasolabial_fold.br(), 0xAA, 2);
	
	cvRectangle(src, mouth.tl(), mouth.br(), 0xAA, 2);
}

CIVSTK::CIVSTK()
{
	init_flag = 0;
}

CIVSTK::~CIVSTK()
{
	m_fnn.freeFNN();
	m_nn.NNfree();
}

void CIVSTK::IVSTKinit(IPLIMAGE img)
{
	if (init_flag)	return;

	int i, j;
	float x, y;
	int twistdata[16] = {
		(int)(0.33*CE21),	(int)(0.33*CE21),	(int)(0.33*CE21),	0, 
		(int)(-0.5*CE21),	0,					(int)(0.5*CE21),	0, 
		(int)(-0.25*CE21),	(int)(0.5*CE21),	(int)(-0.25*CE21),	0,
		0,					0,					0,					(int)(CE21)};
	
	CvSize imgsize = cvSize(img->width, img->height);
	CPsize = imgsize;
	LPsize = cvSize(64, 128);

	for (i=0; i<10; i++)
	{
		colimg[i] = cvCreateImage(imgsize, IPL_DEPTH_8U, 3);
		grayimg[i] = cvCreateImage(imgsize, IPL_DEPTH_8U, 1);
		colLPimg[i] = cvCreateImage(LPsize, IPL_DEPTH_8U, 3);
		grayLPimg[i] = cvCreateImage(LPsize, IPL_DEPTH_8U, 1);
		floatimg[i] = cvCreateImage(imgsize, IPL_DEPTH_32F, 1);
		longimg[i] = cvCreateImage(imgsize, IPL_DEPTH_32S, 1);
		binimg[i] = cvCreateImage(imgsize, IPL_DEPTH_1U, 1);
	}

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

	cvInitFont(&font, CV_FONT_VECTOR0, 0.2f, 0.2f, 0.0f, 1);

	GaborFilter gabor;
	Filter_mask2 real_part, img_part;
    
	float angle[NUM_ANGLE] = {0.0f, 45.0f, -90.0f, -45.0f};
	float wave[NUM_WAVE] = {4.5f, 6.0f, 7.5f, 9.0f};

	for (j=0; j<NUM_ANGLE; j++)
		for (i=0; i<NUM_WAVE; i++)
		{
			gabor.making_gabor_filter4(wave[i], 1.5f, (float)M_PI*((45.0f)/180.0f), (float)M_PI*((angle[j])/180.0f), &real_part, &img_part);

			kernel[i+NUM_WAVE*j][0] = iplCreateConvKernelFP(real_part.hsize, real_part.vsize, real_part.hsize/2, real_part.vsize/2, (float *)real_part.co);
			kernel[i+NUM_WAVE*j][1] = iplCreateConvKernelFP(img_part.hsize, img_part.vsize, img_part.hsize/2, img_part.vsize/2, (float *)img_part.co);
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

			((float*)(xmap->imageData + xmap->widthStep*j))[i] = x;
			((float*)(ymap->imageData + ymap->widthStep*j))[i] = y;
		}

	//procCHATmask = cvCreateImage(CPsize, IPL_DEPTH_1U, 1);
	storage = cvCreateMemStorage();
}

void CIVSTK::IVSTKend()
{
	if (!init_flag)	return;

	int i;

	cvReleaseMemStorage(&storage);
//	cvReleaseImage(&(procCHATmask));

	for (i=0; i<10; i++)
	{
		cvReleaseImage(&(binimg[i]));
		cvReleaseImage(&(longimg[i]));
		cvReleaseImage(&(floatimg[i]));
		cvReleaseImage(&(grayimg[i]));
		cvReleaseImage(&(colimg[i]));
		cvReleaseImage(&(grayLPimg[i]));
		cvReleaseImage(&(colLPimg[i]));
	}

	cvReleaseImage(&ymap);
	cvReleaseImage(&xmap);

	for (i=0; i<NUM_ANGLE*NUM_WAVE; i++)
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

// 20030410
// RGB HLS HSV LUV YUV YCrCb XYZ
void CIVSTK::IVSTKcolorconvert(IPLIMAGE src, IPLIMAGE dst, CString str)
{
	int i;
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (!str.Compare("RGB2HLS"))
		iplRGB2HLS(src, colimg[0]);
	else if (!str.Compare("HLS2RGB"))
		iplHLS2RGB(src, colimg[0]);
	else if (!str.Compare("RGB2HSV"))
		iplRGB2HSV(src, colimg[0]);
	else if (!str.Compare("HSV2RGB"))
		iplHSV2RGB(src, colimg[0]);
	else if (!str.Compare("RGB2LUV"))
		iplRGB2LUV(src, colimg[0]);
	else if (!str.Compare("LUV2RGB"))
		iplLUV2RGB(src, colimg[0]);
	else if (!str.Compare("RGB2XYZ"))
		iplRGB2XYZ(src, colimg[0]);
	else if (!str.Compare("XYZ2RGB"))
		iplXYZ2RGB(src, colimg[0]);
	else if (!str.Compare("RGB2YCrCb"))
		iplRGB2YCrCb(src, colimg[0]);
	else if (!str.Compare("YCrCb2RGB"))
		iplYCrCb2RGB(src, colimg[0]);
	else if (!str.Compare("RGB2YUV"))
		iplRGB2YUV(src, colimg[0]);
	else if (!str.Compare("YUV2RGB"))
		iplYUV2RGB(src, colimg[0]);
	else if (!str.Compare("RGB2PCA"))
		iplApplyColorTwist(src, colimg[0], twist_I1I2I3, 0);

	CString str2 = str + " conversion";
	msgview->AddLine(str2);

	cvCvtPixToPlane(colimg[0], grayimg[0], grayimg[1], grayimg[2], NULL);

	for (i=0; i<3; i++)
	{
		debug[i]->ShowSingleImage(grayimg[i]);
	}

	debug[3]->ShowSingleImage(colimg[0]);
}

// 20030420
// PREWITT_3x3_V PREWITT_3x3_H SOBEL_3x3_V SOBEL_3x3_H LAPLACIAN_3x3 LAPLACIAN_5x5 GAUSSIAN_3x3
// HIPASS_3x3 HIPASS_5x5 SHARPEN_3x3 MEDIAN_3x3 MEDIAN_5x5 MAX_3x3 MAX_5x5 MIN_3x3 MIN_5x5
void CIVSTK::IVSTKfiltering(IPLIMAGE src, IPLIMAGE dst, CString str, int minmax[2])
{
	int i;
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	GaborFilter gabor;

	if (str.Find("MEDIAN") == -1)
	{
		if (src->nChannels == 3)
			cvvConvertImage(src, grayimg[0], 0);
		else
			iplCopy(src, grayimg[0]);

		iplSet(grayimg[1], 0);

		if (!str.Compare("GAUSSIAN_3x3"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_GAUSSIAN_3x3);
		else if (!str.Compare("LAPLACIAN_3x3"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_LAPLACIAN_3x3);
		else if (!str.Compare("LAPLACIAN_5x5"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_LAPLACIAN_5x5);
		else if (!str.Compare("PREWITT_3x3_H"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_PREWITT_3x3_H);
		else if (!str.Compare("PREWITT_3x3_V"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_PREWITT_3x3_V);
		else if (!str.Compare("SOBEL_3x3_H"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_SOBEL_3x3_H);
		else if (!str.Compare("SOBEL_3x3_V"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_SOBEL_3x3_V);
		else if (!str.Compare("HIPASS_3x3"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_HIPASS_3x3);
		else if (!str.Compare("HIPASS_5x5"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_HIPASS_5x5);
		else if (!str.Compare("SHARPEN_3x3"))
			iplFixedFilter(grayimg[0], grayimg[1], IPL_SHARPEN_3x3);
		else if (!str.Compare("MAX_3x3"))
			iplMaxFilter(grayimg[0], grayimg[1], 3, 3, 1, 1);
		else if (!str.Compare("MAX_5x5"))
			iplMaxFilter(grayimg[0], grayimg[1], 5, 5, 2, 2);
		else if (!str.Compare("MIN_3x3"))
			iplMinFilter(grayimg[0], grayimg[1], 3, 3, 1, 1);
		else if (!str.Compare("MIN_5x5"))
			iplMinFilter(grayimg[0], grayimg[1], 5, 5, 2, 2);
		else if (!str.Compare("CANNY_3x3"))
			cvCanny(grayimg[0], grayimg[1], (double)minmax[0], (double)minmax[1], 3);
		else if (!str.Compare("CANNY_5x5"))
			cvCanny(grayimg[0], grayimg[1], (double)minmax[0], (double)minmax[1], 5);
		else if (!str.Compare("GABOR"))
		{
			iplSet(grayimg[1], 0);
			iplScaleFP(grayimg[0], floatimg[0], 0.0, 255.0);

			iplConvolve2DFP(floatimg[0], floatimg[1], kernel[minmax[1]+NUM_ANGLE*minmax[0]], 2, IPL_SUMSQROOT);
			IVSTKnormalize(floatimg[1], grayimg[1]);
		}
	
		debug[0]->ShowSingleImage(grayimg[1]);
	}
	else
	{
		if (src->nChannels == 3)
		{
			iplSet(colimg[0], 0);

			if (!str.Compare("MEDIAN_3x3"))
				iplColorMedianFilter(src, colimg[0], 3, 3, 1, 1);
			else if (!str.Compare("MEDIAN_5x5"))
				iplColorMedianFilter(src, colimg[0], 5, 5, 2, 2);

			debug[0]->ShowSingleImage(colimg[0]);
		}
		else
		{
			cvvConvertImage(src, grayimg[0], 0);

			iplSet(grayimg[1], 0);

			if (!str.Compare("MEDIAN_3x3"))
				iplMedianFilter(grayimg[0], grayimg[1], 3, 3, 1, 1);
			else if (!str.Compare("MEDIAN_5x5"))
				iplMedianFilter(grayimg[0], grayimg[1], 5, 5, 2, 2);

			debug[0]->ShowSingleImage(grayimg[1]);
		}		
	}

	CString str2 = str + " filtering";
	msgview->AddLine(str2);
}

// 20030420
// FT IFT DCT IDCT
void CIVSTK::IVSTKfourier(IPLIMAGE src, IPLIMAGE dst, CString str)
{
	int i;
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	if (str.Find("I") == -1)
	{		
		iplSet(grayimg[1], 0);

		if (!str.Compare("FT"))
			iplRealFft2D(grayimg[0], grayimg[1], IPL_FFT_Forw);
		else if (!str.Compare("DCT"))
			iplDCT2D(grayimg[0], grayimg[1], IPL_DCT_Forward);
		
		debug[0]->ShowSingleImage(grayimg[1]);
	}
	else
	{
		iplSet(grayimg[1], 0);
		iplSet(grayimg[2], 0);

		if (!str.Compare("IFT"))
		{
			iplRealFft2D(grayimg[0], grayimg[1], IPL_FFT_Forw);
			iplRealFft2D(grayimg[1], grayimg[2], IPL_FFT_Inv);
		}
		else if (!str.Compare("IDCT"))
		{
			iplDCT2D(grayimg[0], grayimg[1], IPL_DCT_Forward);
			iplDCT2D(grayimg[1], grayimg[2], IPL_DCT_Inverse);
		}
		
		debug[0]->ShowSingleImage(grayimg[1]);
		debug[1]->ShowSingleImage(grayimg[2]);
	}

	CString str2 = str + " transform";
	msgview->AddLine(str2);
}

//20030417
//LABELING, MAX_BLOB
void CIVSTK::IVSTKblobanal(IPLIMAGE src, IPLIMAGE dst, CString str, CString morph, int num)
{
	int i;	
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	CvSeq* chain = 0; 
	double area, max_area;
	int num_conts;

	num_conts = cvFindContours(grayimg[0], storage, &chain, sizeof(CvContour), 
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
	max_area=0;
	CvSeq *contour=NULL;

	if (!str.Compare("MAX_BLOB"))	// original blob, bounding rectangle, bounding ellipse!
	{
		for( CvSeq* c = chain; c != 0; c = c->h_next )
		{
			area = fabs(cvContourArea(c));
			if (area > max_area) { max_area=area; contour=c; }
		}

		contMayor=contour;
		
		iplSet(grayimg[1], 0);
		cvDrawContours(grayimg[1], contMayor, 255, 0, 0, CV_FILLED);

		// bounding rectangle
		cvSetZero(grayimg[3]);
		CvRect rect = cvContourBoundingRect(contMayor, 1);
		cvRectangle(grayimg[3], cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), 0xFF, -1);
		debug[2]->ShowSingleImage(grayimg[3]);

		// ellipse fitting
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

		cvSetZero(grayimg[4]);
		cvEllipse(grayimg[4], cvPoint(myBox.center.x, myBox.center.y), 
			cvSize(myBox.size.width/2, myBox.size.height/2), -(myBox.angle), 
			0.0, 360.0, 0xFF, -1);
		debug[3]->ShowSingleImage(grayimg[4]);

		free(PointArray);
		free(PointArray32f);

		CString str3;
		str3.Format("Aspect Ratio : %.2f", (float)((float)rect.height/(float)rect.width));
		msgview->AddLine(str3);
	}
	else if (!str.Compare("LABELING"))
	{
		iplSet(grayimg[1], 0);
		
		i = 0;
		for( CvSeq* c = chain; c != 0; c = c->h_next )
		{
			cvDrawContours(grayimg[1], c, 255 - (int)((float)(255.0/(float)(num_conts+1))*(float)i), 0, 0, CV_FILLED);
			i++;
		}

		CString str3;
		str3.Format("%d blobs", num_conts);
		msgview->AddLine(str3);
	}

	if (!morph.Compare("OPEN"))
		iplOpen(grayimg[1], grayimg[2], num);
	else if (!morph.Compare("CLOSE"))
		iplClose(grayimg[1], grayimg[2], num);
	else if (!morph.Compare("DILATE"))
		iplDilate(grayimg[1], grayimg[2], num);
	else if (!morph.Compare("ERODE"))
		iplErode(grayimg[1], grayimg[2], num);

	
	debug[0]->ShowSingleImage(grayimg[1]);
	debug[1]->ShowSingleImage(grayimg[2]);

	CString str2 = "Blob analysis : " + str;
	msgview->AddLine(str2);
}

// CARTESIAN, POLAR, LOGPOLAR
void CIVSTK::IVSTKcoordtx(IPLIMAGE src, IPLIMAGE dst, CString str)
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	//svv.LPtable(CPsize.width, CPsize.height, LPsize.width, LPsize.height);

	/*
	xmap2 = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);
	ymap2 = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);

	for (i=0; i<LPsize.width; i++)
		for (j=0; j<LPsize.height; j++)
		{
			x = (float)svv.btrX[i*LPsize.height+j];
			y = (float)svv.btrY[i*LPsize.height+j];

			((float*)(xmap2->imageData + xmap2->widthStep*j))[i] = x;
			((float*)(ymap2->imageData + ymap2->widthStep*j))[i] = y;
		}*/

	if (!str.Compare("CARTESIAN2LOGPOLAR"))
		iplRemap(grayimg[0], xmap, ymap, grayLPimg[0], IPL_INTER_LINEAR);
/*	else if (!str.Compare("CARTESIAN2POLAR"))
		iplRemap(grayimg[0], xmap2, ymap2, grayLPimg[0], IPL_INTER_LINEAR);
	else if (!str.Compare("POLAR2CARTESIAN"))
		iplRemap(grayimg[0], xmap2, ymap2, grayLPimg[0], IPL_INTER_LINEAR);
	else if (!str.Compare("LOGPOLAR2CARTESIAN"))
		iplRemap(grayimg[0], xmap2, ymap2, grayLPimg[0], IPL_INTER_LINEAR);
*/
	/*cvReleaseImage(&(ymap2));
	cvReleaseImage(&(xmap2));*/

	debug[0]->ShowSingleImage(grayLPimg[0]);

	CString str2 = "Coordinate Transform : " + str;
	msgview->AddLine(str2);
}

//void CIVSTK::IVSTKhistogram(IPLIMAGE Simg, IPLIMAGE Dimg, )

//20030420
void CIVSTK::IVSTKproject2(IPLIMAGE Simg, CvMat proj, int dir, int method)
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
				pixel = ((uchar*)(Simg->imageData + Simg->widthStep*y))[x];

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

			proj.data.fl[x] = (float)sum;
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
				pixel = ((uchar*)(Simg->imageData + Simg->widthStep*y))[x];
			
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

			proj.data.fl[y] = (float)sum;
		}
	}
}

//20030417
//20030420
void CIVSTK::IVSTKprojext(IPLIMAGE src, IPLIMAGE dst, CString str)
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	CIVSTKface face;

	if (!str.Compare("HORIZ"))
	{
		IVSTKproject(grayimg[0], grayimg[2], 1, 0, 0);
	}
	else if (!str.Compare("VERT"))
	{
		IVSTKproject(grayimg[0], grayimg[1], 0, 1, 0);
	}
	else if (!str.Compare("BOTH"))
	{
		IVSTKproject(grayimg[0], grayimg[2], 1, 0, 0);
		IVSTKproject(grayimg[0], grayimg[1], 0, 1, 0);
	}
	else if (!str.Compare("FACE"))
	{
	}
	else if (!str.Compare("FACECOMP"))
	{
		IVSTKproject2(grayimg[0], projY, 1);
		IVSTKcompext(grayimg[0], projY, cvPoint(0, grayimg[0]->height), face);

		iplCopy(grayimg[0], grayimg[5]);
		face.draw(grayimg[5]);

		debug[3]->ShowSingleImage(grayimg[5]);

		CFileDialog *pDlg = new CFileDialog(FALSE, "dat", NULL, OFN_READONLY, 
			"얼굴모델 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

		if (pDlg->DoModal())
		{
			CString filename = pDlg->GetFileName();

			if (filename != "")
			{
				face.savefeature(filename);
			}
		}

	}

	debug[0]->ShowSingleImage(grayimg[0]);
	debug[1]->ShowSingleImage(grayimg[1]);
	debug[2]->ShowSingleImage(grayimg[2]);

	CString str2 = "Projection for " + str;
	msgview->AddLine(str2);
}

//20030420
//GT GTE LT LTE OUT IN
void CIVSTK::IVSTKthreshold(IPLIMAGE src, IPLIMAGE dst, CString str, int th1, int th2)
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	if (!str.Compare("GT"))
		cvThreshold(grayimg[0], grayimg[1], th1, 255, CV_THRESH_BINARY);
	else if (!str.Compare("GTE"))
		cvThreshold(grayimg[0], grayimg[1], th1, 255, CV_THRESH_BINARY);
	else if (!str.Compare("LT"))
		cvThreshold(grayimg[0], grayimg[1], th1, 255, CV_THRESH_BINARY_INV);
	else if (!str.Compare("LTE"))
		cvThreshold(grayimg[0], grayimg[1], th1, 255, CV_THRESH_BINARY_INV);
	else if (!str.Compare("OUT"))
	{
		cvThreshold(grayimg[0], grayimg[1], th2, 255, CV_THRESH_BINARY);
		cvThreshold(grayimg[0], grayimg[2], th1, 255, CV_THRESH_BINARY_INV);

		iplOr(grayimg[1], grayimg[2], grayimg[3]);
		iplCopy(grayimg[3], grayimg[1]);
	}
	else if (!str.Compare("IN"))
	{
		cvThreshold(grayimg[0], grayimg[1], th1, 255, CV_THRESH_BINARY);
		cvThreshold(grayimg[0], grayimg[2], th2, 255, CV_THRESH_BINARY_INV);

		iplAnd(grayimg[1], grayimg[2], grayimg[3]);
		iplCopy(grayimg[3], grayimg[1]);
	}
	else if (!str.Compare("HIST"))
	{
		if (src->nChannels == 3)
		{
			cvCvtPixToPlane(src, grayimg[4], grayimg[5], grayimg[6], NULL);
			IVSTKhistogram(grayimg[4], grayimg[1], 0);
			IVSTKhistogram(grayimg[5], grayimg[2], 0);
			IVSTKhistogram(grayimg[6], grayimg[3], 0);

			debug[1]->ShowSingleImage(grayimg[2]);
			debug[2]->ShowSingleImage(grayimg[3]);
		}
		
		IVSTKhistogram(grayimg[0], grayimg[1], 0);
	}

	debug[0]->ShowSingleImage(grayimg[1]);

	CString str2 = "Thresholding with " + str;
	msgview->AddLine(str2);

	if (dst != NULL)	iplCopy(grayimg[1], dst);
}

//20030420
void CIVSTK::IVSTKproject(IplImage *Simg, IplImage *Dimg, int xdir, int ydir, int method)
{
	if ((xdir == 0) && (ydir == 0))	return;

	int val, i, MaxVal = 0, MinVal = (int)1e6;
	int diff = abs(CPsize.width - CPsize.height);

	// get projection
	if (xdir) IVSTKproject2(Simg, projX, 0, method);
	if (ydir) IVSTKproject2(Simg, projY, 1, method);

	iplSet(Dimg, 0);

	if (xdir) {

		for (i = 0; i < CPsize.width; i++) 
		{
			val = (int)projX.data.fl[i];
			if (val > MaxVal)
				MaxVal = val;
			if (val < MinVal)
				MinVal = val;
		}

		for( i = 0; i < CPsize.width; i++ ) 
		{
			val = (int)projX.data.fl[i];
			CvPoint p[4];
			
			p[0].x = p[1].x = i;
			p[2].x = p[3].x = (i+1);
			
			p[1].y = p[2].y = Simg->height - 1;
			p[0].y = p[3].y = Simg->height - (int) ((double)(Simg->height) * (double)((double)(val-MinVal) / (double)(MaxVal-MinVal)));
			
			//cvFillConvexPoly( Dimg, p, 4, CV_RGB(0,255,0));
			cvFillConvexPoly( Dimg, p, 4, 0x99);
		}
	}

	if (ydir) {

		MaxVal = 0, MinVal = (int)1e6;
		for (i = 0; i < CPsize.height; i++) 
		{
			val = (int)projY.data.fl[i];
			if (val > MaxVal)
				MaxVal = val;
			if (val < MinVal)
				MinVal = val;
		}

		for( i = 0; i < CPsize.height; i++ ) 
		{
			val = (int)projY.data.fl[i];
			CvPoint p[4];
			
			p[0].y = p[3].y = i;
			p[1].y = p[2].y = (i+1);
			
			p[2].x = p[3].x = Simg->width - 1;
			p[0].x = p[1].x = Simg->width - (int) ((double)(Simg->width) * (double)((double)(val-MinVal) / (double)(MaxVal-MinVal)));
			
			//cvFillConvexPoly( Dimg, p, 4, CV_RGB(0,0,255));
			cvFillConvexPoly( Dimg, p, 4, 0x99);
		}
	}
}

//20030420
void CIVSTK::IVSTKhistogram(IplImage *Simg, IplImage *Dimg, int start)
{
	int val, i, MaxVal = 0;
	int bins = 256;

	if (Simg->nChannels == 1)
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
			p[0].y = p[3].y = Simg->height - (int) ((double)(Simg->height) * (double)((double)val / (double)MaxVal));
			
			cvFillConvexPoly( Dimg, p, 4, 0x99);
		}
	}
}

//20030420	// 20030430
// SQDIFF SQDIFF_NORM CCORR CCORR_NORM  CCOEFF CCOEFF_NORM
void CIVSTK::IVSTKtempmatch(IPLIMAGE src, IPLIMAGE dst, CString str, CString part, int degree)
{
	int i, j;
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	CFileDialog *pDlg = new CFileDialog( TRUE, "jpg", NULL, OFN_READONLY, 
		"템플릿영상 *.jpg|*.jpg|모든파일 *.*|*.*|", NULL );

	if (pDlg->DoModal())
	{
		CString filename = pDlg->GetFileName();

		if (filename != "")
		{
			IPLIMAGE cimg = cvvLoadImage(filename);
			IPLIMAGE tempimg = cvCreateImage(cvSize(cimg->width, cimg->height), IPL_DEPTH_8U, 1);
			cvvConvertImage(cimg, tempimg);

			IPLIMAGE resultimg = cvCreateImage(cvSize(src->width - cimg->width + 1, src->height - cimg->height + 1), IPL_DEPTH_32F, 1);
			IPLIMAGE crapimg = cvCreateImage(cvSize(src->width - cimg->width + 1, src->height - cimg->height + 1), IPL_DEPTH_8U, 1);

			if (!str.Compare("SQDIFF"))
				cvMatchTemplate(grayimg[0], tempimg, resultimg, CV_TM_SQDIFF);
			else if (!str.Compare("SQDIFF_NORM"))
				cvMatchTemplate(grayimg[0], tempimg, resultimg, CV_TM_SQDIFF_NORMED);
			else if (!str.Compare("CCORR"))
				cvMatchTemplate(grayimg[0], tempimg, resultimg, CV_TM_CCORR);
			else if (!str.Compare("CCORR_NORM"))
				cvMatchTemplate(grayimg[0], tempimg, resultimg, CV_TM_CCORR_NORMED);
			else if (!str.Compare("CCOEFF"))
				cvMatchTemplate(grayimg[0], tempimg, resultimg, CV_TM_CCOEFF);
			else if (!str.Compare("CCOEFF_NORM"))
				cvMatchTemplate(grayimg[0], tempimg, resultimg, CV_TM_CCOEFF_NORMED);

			float val[2];
			iplMinMaxFP(resultimg, &val[0], &val[1]);
			iplScaleFP(resultimg, crapimg, val[0], val[1]);

			if (!str.Compare("SQDIFF") || !str.Compare("SQDIFF_NORM"))
			{
				iplNot(crapimg, grayimg[4]);
				iplThreshold(grayimg[4], grayimg[2], degree);
			}
			else if (!str.Compare("CCORR") || !str.Compare("CCORR_NORM"))
			{
				iplThreshold(crapimg, grayimg[2], degree);
			}
			else if (!str.Compare("CCOEFF") || !str.Compare("CCOEFF_NORM"))
			{
				iplThreshold(crapimg, grayimg[2], degree);
			}

			iplCopy(grayimg[0], grayimg[3]);

			for (j=0; j<crapimg->height; j++)
				for (i=0; i<crapimg->width; i++)
				{
					unsigned char pixel;

					pixel = ((uchar*)(grayimg[2]->imageData + grayimg[2]->widthStep*j))[i];

					if (pixel == 255)
						cvRectangle(grayimg[3], cvPoint(i, j), cvPoint(i + tempimg->width, j + tempimg->height), 0xAA, 1);
				}

			debug[0]->ShowSingleImage(tempimg);
			debug[1]->ShowSingleImage(crapimg);
			debug[2]->ShowSingleImage(grayimg[3]);

			cvReleaseImage(&crapimg);
			cvReleaseImage(&tempimg);
			cvReleaseImage(&resultimg);
		}
	}
}

//20030429
// ANGLE ENTROPY MOMENT ...
void CIVSTK::IVSTKstatfeat(IPLIMAGE src, IPLIMAGE dst, CString str)
{
	CString str2;
	int i;
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	IplMomentState mState;
	CvMoments moments;
	
	iplMoments(grayimg[0], mState);
	cvMoments(grayimg[0], &moments);

	if (!str.Compare("ANGLE"))
	{
		CPoint pt;
		float angle;
		float aa, bb, cc;
		float l, w;

		pt.x = (long)(iplGetSpatialMoment(mState, 1, 0)/iplGetSpatialMoment(mState, 0, 0));
		pt.y = (long)(iplGetSpatialMoment(mState, 0, 1)/iplGetSpatialMoment(mState, 0, 0));

		aa = (float)(iplGetSpatialMoment(mState, 2, 0)/iplGetSpatialMoment(mState, 0, 0) - pt.x*pt.x);
		bb = (float)(2.0*(iplGetSpatialMoment(mState, 1, 1)/iplGetSpatialMoment(mState, 0, 0) - pt.x*pt.y));
		cc = (float)(iplGetSpatialMoment(mState, 0, 2)/iplGetSpatialMoment(mState, 0, 0) - pt.y*pt.y);

		l = (float)(sqrt(((aa+cc) + sqrt(bb*bb + (aa-cc)*(aa-cc)))/2.0f));
		w = (float)(sqrt(((aa+cc) - sqrt(bb*bb + (aa-cc)*(aa-cc)))/2.0f));

		str2.Format("Center Pt. [%3d,%3d]", pt.x, pt.y);
		msgview->AddLine(str2);

		angle = (float)(atan2(bb, aa-cc)/2.0f);

		str2.Format("Slanted Angle [%.2f]", 180.0*angle/M_PI);
		msgview->AddLine(str2);

		cvLine(grayimg[0], cvPoint((int)((float)pt.x - l/10.0), pt.y), cvPoint((int)((float)pt.x + l/10.0), pt.y), 0x55, 3);
		cvLine(grayimg[0], cvPoint(pt.x, (int)((float)pt.y - l/10.0)), cvPoint(pt.x, (int)((float)pt.y + l/10.0)), 0x55, 3);

		cvLine(grayimg[0], cvPoint((int)((float)pt.x - l/2.0*cos(angle)), (int)((float)pt.y - l/2.0*sin(angle))), cvPoint((int)((float)pt.x + l/2.0*cos(angle)), (int)((float)pt.y + l/2.0*sin(angle))), 0xAA, 2);
		cvLine(grayimg[0], cvPoint((int)((float)pt.x - w/2.0*cos(angle+M_PI/2)), (int)((float)pt.y - w/2.0*sin(angle+M_PI/2))), cvPoint((int)((float)pt.x + w/2.0*cos(angle+M_PI/2)), (int)((float)pt.y + w/2.0*sin(angle+M_PI/2))), 0xAA, 2);
	}
	else if (!str.Compare("HUMOMENT"))	//???
	{
		CvHuMoments humoments;

		cvGetHuMoments(&moments, &humoments);

		str2.Format("HuMoments [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]", 
			humoments.hu1, humoments.hu2, humoments.hu3, humoments.hu4, humoments.hu5, humoments.hu6, humoments.hu7);
		msgview->AddLine(str2);
	}
	else if (!str.Compare("L1 NORM"))
	{
		str2.Format("L1 Norm [%.2f]", iplNorm(grayimg[0], NULL, IPL_L1));
		msgview->AddLine(str2);
	}
	else if (!str.Compare("L2 NORM"))
	{
		str2.Format("L2 Norm [%.2f]", iplNorm(grayimg[0], NULL, IPL_L2));
		msgview->AddLine(str2);
	}
	else if (!str.Compare("C NORM"))
	{
		str2.Format("C Norm [%.2f]", iplNorm(grayimg[0], NULL, IPL_C));
		msgview->AddLine(str2);
	}
	else if (!str.Compare("MINMAX"))
	{
		float min, max;
		iplScaleFP(grayimg[0], floatimg[0], 0.0f, 255.0f);
		
		iplMinMaxFP(floatimg[0], &min, &max);

		str2.Format("Pixel Range [%.0f, %.0f]", min, max);
		msgview->AddLine(str2);
	}
	else if (!str.Compare("NONZERO"))
	{
		str2.Format("Non-zero Pixel [%d]", cvCountNonZero(grayimg[0]));
		msgview->AddLine(str2);
	}
	else if (!str.Compare("MEANSTD"))
	{
		double mean, std;

		cvMean_StdDev(grayimg[0], &mean, &std);

		str2.Format("Mean [%.2f], Standard Deviation [%.2f]", mean, std);
		msgview->AddLine(str2);
	}
	else if (!str.Compare("MINMAXLOC"))
	{
		double min, max;
		CvPoint minp, maxp;

		cvMinMaxLoc(grayimg[0], &min, &max, &minp, &maxp);

		str2.Format("Extremums are [%d,%d : %.2f] and [%d,%d : %.2f]", minp.x, minp.y, min, maxp.x, maxp.y, max);
		msgview->AddLine(str2);
	}

	debug[0]->ShowSingleImage(grayimg[0]);

}

//20030501
//EYE EYEBROW MOUTH NRF NLF WHOLE
void CIVSTK::IVSTKgeofeat(IPLIMAGE src, IPLIMAGE dst, CString str, CString str2, CIVSTKface face)
{
	CString str3;
	int i;
	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	//
	iplSet(grayimg[5], 0);
	iplScaleFP(grayimg[0], floatimg[0], 0.0, 255.0);

	for (i=0; i<NUM_ANGLE; i++)		
	{		
		iplConvolve2DFP(floatimg[0], floatimg[i+1], kernel[1+NUM_WAVE*i], 2, IPL_SUMSQROOT);
		IVSTKnormalize(floatimg[i+1], grayimg[i+6]);
	}

	iplFixedFilter(grayimg[0], grayimg[5], IPL_SOBEL_3x3_H);

	if ((!str.Compare("NONE")) && (!str2.Compare("NONE")))
		return;

	// str = Local
	if (!str2.Compare("NONE"))
	{
		if (!str.Compare("WHOLE"))
		{
			IVSTKsubfeatext(grayimg[5], &(face.left_eye), EYEOPENLP);
			IVSTKsubfeatext(grayimg[5], &(face.right_eye), EYEOPENLP);
			
			face.EyeBrowFeat();
			
			//IVSTKsubfeatext(grayimg[6], &(face.mouth), CNTNZ);
			IVSTKsubfeatext(grayimg[8], &(face.mouth), GG);
			
			IVSTKsubfeatext(grayimg[6], &(face.nasolabial_root), NLR);
			
			IVSTKsubfeatext(grayimg[7], &(face.left_nasolabial_fold), NLF);
			IVSTKsubfeatext(grayimg[7], &(face.right_nasolabial_fold), NLF);

			str3.Format("Eye Openness = [%.2f, %.2f]", face.left_eye.feature, face.right_eye.feature);
			msgview->AddLine(str3);
			str3.Format("Eye-Eyebrow Distance = [%.2f, %.2f]", face.left_eyebrow.feature, face.right_eyebrow.feature);
			msgview->AddLine(str3);
			str3.Format("Mouth Openness = [%.2f]", face.mouth.feature);
			msgview->AddLine(str3);
			str3.Format("NRF feature = [%.2f]", face.nasolabial_root.feature);
			msgview->AddLine(str3);
			str3.Format("NLF feature = [%.2f, %.2f]", face.left_nasolabial_fold.feature, face.right_nasolabial_fold.feature);
			msgview->AddLine(str3);
		}
		else if (!str.Compare("EYE"))
		{
			IVSTKsubfeatext(grayimg[5], &(face.left_eye), EYEOPENLP);
			IVSTKsubfeatext(grayimg[5], &(face.right_eye), EYEOPENLP);

			str3.Format("Eye Openness = [%.2f, %.2f]", face.left_eye.feature, face.right_eye.feature);
			msgview->AddLine(str3);
		}
		else if (!str.Compare("EYEBROW"))
		{
			face.EyeBrowFeat();

			str3.Format("Eye-Eyebrow Distance = [%.2f, %.2f]", face.left_eyebrow.feature, face.right_eyebrow.feature);
			msgview->AddLine(str3);
		}
		else if (!str.Compare("MOUTH"))
		{
			// grayimg[8] -> -90 (horizontal)
			IVSTKsubfeatext(grayimg[8], &(face.mouth), GG);
			//IVSTKsubfeatext(grayimg[6], &(face.mouth), CNTNZ);

			str3.Format("Mouth Openness = [%.2f]", face.mouth.feature);
			msgview->AddLine(str3);
		}
		else if (!str.Compare("NRF"))
		{
			IVSTKsubfeatext(grayimg[6], &(face.nasolabial_root), NLR);

			str3.Format("NRF feature = [%.2f]", face.nasolabial_root.feature);
			msgview->AddLine(str3);
		}
		else if (!str.Compare("NLF"))
		{
			IVSTKsubfeatext(grayimg[7], &(face.left_nasolabial_fold), NLF);
			IVSTKsubfeatext(grayimg[7], &(face.right_nasolabial_fold), NLF);

			str3.Format("NLF feature = [%.2f, %.2f]", face.left_nasolabial_fold.feature, face.right_nasolabial_fold.feature);
			msgview->AddLine(str3);
		}
	}
	else
	{
		if (!str2.Compare("WHOLE"))
		{
			face.EyeOpennessG();
			face.EyeBrowFeat();

			face.MouthOpennessG1();
			str3.Format("Mouth Openness 1 = [%.2f]", face.mouth.feature);
			msgview->AddLine(str3);

			face.MouthOpennessG2();
			str3.Format("Mouth Openness 2 = [%.2f]", face.mouth.feature);
			msgview->AddLine(str3);

			IVSTKsubfeatext(grayimg[6], &(face.nasolabial_root), NLR);
			
			IVSTKsubfeatext(grayimg[7], &(face.left_nasolabial_fold), NLF);
			IVSTKsubfeatext(grayimg[7], &(face.right_nasolabial_fold), NLF);

			str3.Format("Eye Openness = [%.2f, %.2f]", face.left_eye.feature, face.right_eye.feature);
			msgview->AddLine(str3);
			str3.Format("Eye-Eyebrow Distance = [%.2f, %.2f]", face.left_eyebrow.feature, face.right_eyebrow.feature);
			msgview->AddLine(str3);
			str3.Format("NRF feature = [%.2f]", face.nasolabial_root.feature);
			msgview->AddLine(str3);
			str3.Format("NLF feature = [%.2f, %.2f]", face.left_nasolabial_fold.feature, face.right_nasolabial_fold.feature);
			msgview->AddLine(str3);
		}
		else if (!str2.Compare("EYE"))
		{
			face.EyeOpennessG();

			str3.Format("Eye Openness = [%.2f, %.2f]", face.left_eye.feature, face.right_eye.feature);
			msgview->AddLine(str3);
		}
		else if (!str2.Compare("EYEBROW"))
		{
			face.EyeBrowFeat();

			str3.Format("Eye-Eyebrow Distance = [%.2f, %.2f]", face.left_eyebrow.feature, face.right_eyebrow.feature);
			msgview->AddLine(str3);
		}
		else if (!str2.Compare("MOUTH"))
		{
			face.MouthOpennessG1();
			str3.Format("Mouth Openness 1 = [%.2f]", face.mouth.feature);
			msgview->AddLine(str3);

			face.MouthOpennessG2();
			str3.Format("Mouth Openness 2 = [%.2f]", face.mouth.feature);
			msgview->AddLine(str3);
		}
		else if (!str2.Compare("NRF"))
		{
			IVSTKsubfeatext(grayimg[6], &(face.nasolabial_root), NLR);

			str3.Format("NRF feature = [%.2f]", face.nasolabial_root.feature);
			msgview->AddLine(str3);
		}
		else if (!str2.Compare("NLF"))
		{
			IVSTKsubfeatext(grayimg[7], &(face.left_nasolabial_fold), NLF);
			IVSTKsubfeatext(grayimg[7], &(face.right_nasolabial_fold), NLF);

			str3.Format("NLF feature = [%.2f, %.2f]", face.left_nasolabial_fold.feature, face.right_nasolabial_fold.feature);
			msgview->AddLine(str3);
		}
	}
	//

	debug[0]->ShowSingleImage(grayimg[5]);
	debug[1]->ShowSingleImage(grayimg[6]);
	debug[2]->ShowSingleImage(grayimg[7]);

	iplCopy(grayimg[0], grayimg[9]);
	face.draw(grayimg[9]);	
	debug[3]->ShowSingleImage(grayimg[9]);

	CFileDialog *pDlg = new CFileDialog( FALSE, "dat", NULL, OFN_READONLY, 
		"얼굴특징값 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

	if (pDlg->DoModal())
	{
		CString filename = pDlg->GetFileName();

		if (filename != "")
		{
			face.savefeature(filename);
		}
	}	
}

/*
void CIVSTK::IVSTKfacefeatext(IPLIMAGE Simg, CIVSTKface *face)
{
	static int k = 0;
	int i;

	iplSet(grayimg[5], 0);

	iplScaleFP(Simg, floatimg[0], 0.0, 255.0);

	for (i=0; i<NUM_FILTERS; i++)		
	{		
		iplConvolve2DFP(floatimg[0], floatimg[i+1], kernel[i], 2, IPL_SUMSQROOT);
		IVSTKnormalize(floatimg[i+1], grayimg[i+6]);
	}

	iplFixedFilter(Simg, grayimg[5], IPL_SOBEL_3x3_H);
	
	IVSTKsubfeatext(grayimg[5], &(face->left_eye), EYEOPENLP);		// LP
	IVSTKsubfeatext(grayimg[5], &(face->right_eye), EYEOPENLP);		// LP
	//IVSTKsubfeatext(grayimg[8], &(face->left_eye), EYEOPENG);	// gabor
	//IVSTKsubfeatext(grayimg[8], &(face->right_eye), EYEOPENG);	// gabor
	IVSTKsubfeatext(grayimg[6], &(face->nasolabial_root), NLR);
	IVSTKsubfeatext(grayimg[7], &(face->left_nasolabial_fold), NLF);
	IVSTKsubfeatext(grayimg[9], &(face->right_nasolabial_fold), NLF);
	IVSTKsubfeatext(grayimg[6], &(face->mouth), CNTNZ);


	face->left_eyebrow.feature = (float)(face->left_eye.GetCenterY() - face->left_eyebrow.GetCenterY());
	face->right_eyebrow.feature = (float)(face->right_eye.GetCenterY() - face->right_eyebrow.GetCenterY());

	CFileDialog *pDlg = new CFileDialog( TRUE, NULL, NULL, OFN_READONLY, 
		"얼굴특징 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

	if (pDlg->DoModal())
	{
		CString filename = pDlg->GetFileName();

		if (filename != "")
		{
			face->save(filename);
		}
	}	
}
*/

// 20030502 -> GG 추가
void CIVSTK::IVSTKsubfeatext(IPLIMAGE Simg, CIVSTKsubcomp *comp, FeatureIndex method)
{
	int th = 0;
	int i, j;
	double param[2] = {1, 1.0};

	CvRect rect = comp->rect;

	cvSetImageROI(Simg, rect);
	cvSetImageROI(grayimg[0], rect);
	iplCopy(Simg, grayimg[0]);
	grayimg[0]->roi = NULL;
	Simg->roi = NULL;

	float value, x, y;

	IPLIMAGE procCPimg;

	CSVV myvv(1);
	
	switch(method)
	{
		// NLF feature
		case NLF:
			// get bin. image (GT) by otsu's method with 0.8
			iplSet(grayimg[1], 0);
			IVSTKthreshold2(grayimg[0], grayimg[1], th, 0, OTSU_H, 0.8f);

			// count nonzero pixels
			cvSetImageROI(grayimg[1], rect);
			comp->feature = (float)cvCountNonZero(grayimg[1]);
			grayimg[1]->roi = NULL;
			break;
		case HEDGE:
			iplSet(grayimg[1], 0);
			iplFixedFilter(grayimg[0], grayimg[1], IPL_SOBEL_3x3_H);
			break;
		case ADTH:
			//double param[2] = {1, 1.0};
			iplSet(grayimg[1], 0);
			//cvThreshold(procCPGimg[0], procCPGimg[1], 10, 255, CV_THRESH_BINARY_INV);
			cvAdaptiveThreshold(grayimg[0], grayimg[1], 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY);
			break;
		case CANNY:
			iplSet(grayimg[1], 0);
			cvCanny(grayimg[0], grayimg[1], 1, 255, 3);
			break;
		case ICOPY:
			iplCopy(grayimg[0], grayimg[1]);
			break;
		// NLR feature
		case NLR:
			// get bin. image (GT) by otsu's method with 0.5
			iplSet(grayimg[1], 0);
			IVSTKthreshold2(grayimg[0], grayimg[1], th, 0, OTSU_H, 0.5);
			
			cvSetImageROI(grayimg[1], rect);
			comp->feature = (float)cvCountNonZero(grayimg[1]);
			grayimg[1]->roi = NULL;

			break;
		case CNTNZ:
			// 0: get bin. image (GT)
			iplSet(grayimg[1], 0);
			IVSTKthreshold2(grayimg[0], grayimg[1], 64, 0, GT, 0.5f);
			
			cvSetImageROI(grayimg[1], rect);
			comp->feature = (float)cvCountNonZero(grayimg[1]);
			grayimg[1]->roi = NULL;
			
			break;
		case EYEOPENLP:

			// eye openness 1: LP feature.
			// get bin. image (GT) otsu, 1.5
			iplSet(grayimg[1], 0);
			IVSTKthreshold2(grayimg[0], grayimg[1], th, 0, OTSU_H, 1.8f);

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

					((float*)(xmap2->imageData + xmap2->widthStep*j))[i] = x;
					((float*)(ymap2->imageData + ymap2->widthStep*j))[i] = y;
				}

			cvSetImageROI(grayimg[1], rect);
			iplCopy(grayimg[1], procCPimg);
			iplRemap(procCPimg, xmap2, ymap2, grayLPimg[0], IPL_INTER_LINEAR);
			grayimg[1]->roi = NULL;

			IVSTKproject2(grayLPimg[0], projXLP, 0);

			for (i=0; i<LPsize.width; i++)
			{
				if (projXLP.data.fl[i] > 1e1)
				{
					value = (float)i;
					break;
				}
			}

			comp->feature = value = (float)((float)value/(float)LPsize.width);

			cvReleaseImage(&(ymap2));
			cvReleaseImage(&(xmap2));
			cvReleaseImage(&(procCPimg));

			break;
		case EYEOPENG:
			// eye openness 2: gabor feature
			// get bin. image (GT) by otsu's method with 0.5
			iplSet(grayimg[1], 0);
			IVSTKthreshold2(grayimg[0], grayimg[1], th, 0, OTSU_H, 0.5f);
			
			cvSetImageROI(grayimg[1], rect);
			comp->feature = (float)cvCountNonZero(grayimg[1]);
			grayimg[1]->roi = NULL;

			break;
		case GG:	// local feature of mouth
			comp->feature = IVSTKgg(grayimg[0]);

			break;
		default:
			break;
	}
}

//20030502
float CIVSTK::IVSTKgg(IPLIMAGE A)
{
	int i, j;
	float mG, sG, fG;
	float wSum, wGSum;
	int th;
	unsigned char pixel;

	IPLIMAGE B = cvCreateImage(cvGetSize(A), IPL_DEPTH_8U, 1);

	IVSTKotsu(A, &th, 0);

	for (j=0; j<A->height; j++)
		for (i=0; i<A->width; i++)
		{
			pixel = ((uchar*)(A->imageData + A->widthStep*j))[i];
			((uchar*)(B->imageData + B->widthStep*j))[i] = (pixel > th) ? 1:0;
		}

	IVSTKproject2(B, projY, 1);

	float *dproj = new float[B->height];	
	for(i=1; i<B->height; i++)
		dproj[i] = (float)fabs(projY.data.fl[i]-projY.data.fl[i-1]);

	float *wG = new float[B->height];	
	mG = ((float)B->height/2.0f);
	sG = ((float)B->height/4.0f)*((float)B->height/4.0f);
	wSum = wGSum = 0.0;
	for(i=1;i<B->height;i++)
	{
		wG[i] = (float)exp(-((float)i-mG)/sG);
		wSum += wG[i];
		wGSum += wG[i]*dproj[i];
	}

	fG = wGSum/wSum;

	cvReleaseImage(&(B));

	delete [] wG;
	delete [] dproj;

	return fG;
}

void CIVSTK::IVSTKnormalize(IplImage *Simg, IplImage *Dimg)
{
	float min, max, scale;

	if (Simg->depth != IPL_DEPTH_32F)	return;

	//iplScaleFP(Simg, procCPFimg[6], 0.0, 255.0);
	//iplConvolve2DFP(procCPFimg[0], procCPFimg[1], kernel[0], 2, IPL_SUMSQROOT);

	// image normalization
	iplMinMaxFP(Simg, &min, &max);
	//iplMinMaxFP(procCPFimg[6], &min, &max);
	iplSubtractSFP(Simg, floatimg[6], min, 0);
	scale = (float)((float)(255.0)/(max - min));
	iplMultiplySFP(floatimg[6], floatimg[7], scale);

	iplScaleFP(floatimg[7], Dimg, 0, 255);
}

void CIVSTK::IVSTKthreshold2(IPLIMAGE Simg, IPLIMAGE Dimg, int low, int high, ThresholdIndex method, float otsu)
{
	int th = 0;

	switch(method)
	{
		case IB:
			cvThreshold(Simg, grayimg[0], low, 255, CV_THRESH_BINARY);
			cvThreshold(Simg, grayimg[1], high, 255, CV_THRESH_BINARY_INV);
			iplAnd(grayimg[0], grayimg[1], Dimg);
			break;
		case OB:
			cvThreshold(Simg, grayimg[0], low, 255, CV_THRESH_BINARY);
			cvThreshold(Simg, grayimg[1], high, 255, CV_THRESH_BINARY_INV);
			iplAnd(grayimg[0], grayimg[1], grayimg[2]);
			iplNot(grayimg[2], Dimg);
			break;
		case GT:
			cvThreshold(Simg, Dimg, low, 255, CV_THRESH_BINARY);
			break;
		case LT:
			cvThreshold(Simg, Dimg, low, 255, CV_THRESH_BINARY_INV);
			break;
		case OTSU_H:
			IVSTKotsu(Simg, &th, 0);
			cvThreshold(Simg, Dimg, (int)((float)th*otsu), 255, CV_THRESH_BINARY);
			break;
		case OTSU_L:
			IVSTKotsu(Simg, &th, 0);
			cvThreshold(Simg, Dimg, (int)((float)th*otsu), 255, CV_THRESH_BINARY_INV);
			break;
		default:
			break;
	}
}

void CIVSTK::IVSTKotsu(IplImage *Simg, int *th0, int start)
{
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
			tbucket += (long)bucket[i];
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

// 20030502
// -_-++++++++++++++++++++++++++
void CIVSTK::IVSTKcompext(IPLIMAGE A, CvMat proj, CvPoint range, CIVSTKface& facecomp)
{
	int i, j;
	int est_num = 0, est_num2 = 0;
	int flageye = 0, flageyebrow = 0;
	double diff, sigma;
	CString str;
	int flag = 0;
	int start, end, length;
	int projNum = range.y - range.x;
	float ref_Length = ((float)(range.y - range.x)*0.01f);
	int pre_est_num = 0;

	float max_proj = -1.0f;
	for (i=range.x; i<range.y; i++)
		max_proj = __max(proj.data.fl[i], max_proj);

	float lower_limit = max_proj*(0.01f);

	while (flag == 0)
	{
		start = end = length = 0;
		est_num = 0;

		// first scan
		for (i=range.x; i<range.y; i++)
		{
			if (proj.data.fl[i] > lower_limit && i == 0)
			{
				start = i;
				est_num++;
			}
			else if (i != 0)
			{
				if (proj.data.fl[i] > lower_limit && proj.data.fl[i-1] <= lower_limit)
				{
					start = i;
					est_num++;
				}
				else if (proj.data.fl[i-1] > lower_limit && proj.data.fl[i] <= lower_limit)
				{
					end = i-1;
					length = end - start + 1;

					if ((float)length <= ref_Length)
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

	str.Format("lower limit = %.2f, est_num = %d", lower_limit, est_num);
	msgview->AddLine(str);

	CIVSTKsegment* segment = NULL;
	segment = new CIVSTKsegment[est_num+1];

	// second scan
	for (i=range.x; i<range.y; i++)
	{
		if (proj.data.fl[i] > lower_limit && i == 0)
		{
			segment[est_num2].start = i;
			segment[est_num2].sum += (long)proj.data.fl[i];
		}
		else if (i != 0)
		{
			if (proj.data.fl[i] > lower_limit && proj.data.fl[i-1] <= lower_limit)
			{
				//est_num2++;
				segment[est_num2].start = i;
				segment[est_num2].sum += (long)proj.data.fl[i];
			}
			else if (proj.data.fl[i-1] > lower_limit && proj.data.fl[i] <= lower_limit)
			{
				segment[est_num2].end = i-1;
				segment[est_num2].length = segment[est_num2].end - segment[est_num2].start + 1;

				if ((float)segment[est_num2].length > ref_Length)
				{
					//str.Format("%d th segment's range : [%d, %d]", est_num2, segment[est_num2].start, segment[est_num2].end);
					est_num2++;				
					//msgview->AddLine(str);
				}
			}
			else if (proj.data.fl[i] > lower_limit)
			{
				segment[est_num2].sum += (long)proj.data.fl[i];
			}
		}
	}

	if (proj.data.fl[i-1] > lower_limit)
	{
		segment[est_num2].end = i-1;
		segment[est_num2].length = segment[est_num2].end - segment[est_num2].start + 1;
		//est_num2++;		
	}

	if (est_num != est_num2)
	{
		str.Format("Grouping error due to different such as %d != %d", est_num, est_num2);
		msgview->AddLine(str);

		delete [] segment;
		segment = NULL;

		return;
	}

	// gaussian weight for each group
	for (i=0; i<est_num2; i++)
	{
		for (j = segment[i].start; j<segment[i].end; j++)
		{
			diff = (double)((proj.data.fl[j] - segment[i].length/2)*(proj.data.fl[j] - segment[i].length/2));
			sigma = (double)((segment[i].length/4)*(segment[i].length/4));
			segment[i].gauss += (float)(proj.data.fl[j]*exp(-(diff/sigma)));
		}
	}

	double gaussmax = -1;
	int face_index[2] = {0, 0};

	for (i=0; i<est_num2; i++)
	{
		if (segment[i].end < (int)((float)projNum/2.0f))
		{
			face_index[0] = i;

			for (j=i+1; j<est_num2; j++)
			{
				if (segment[j].start > (int)((float)projNum/2.0f))
				{
					face_index[1] = j;
					break;
				}
			}
		}
		else
			break;
	}

	//str.Format("Face Index = [%d, %d]", face_index[0], face_index[1]);	// -,.-?
	//msgview->AddLine(str);

	// 20030502@18:00 - DJ
	// Nose is located under the face_index[1].
	if (segment[face_index[1]].start - segment[face_index[0]].end < (int)((float)projNum/10.0f))
	{
		for (i=0; i<face_index[0]; i++)	// ?
		{
			if (segment[i].length > (int)((float)projNum/20.0f))
			{
				facecomp.left_eyebrow.rect.y = facecomp.right_eyebrow.rect.y = segment[i].start;
				facecomp.left_eyebrow.rect.height = facecomp.right_eyebrow.rect.height = segment[i].length;

				for (j=i+1; j<est_num2; j++)
				{
					if (segment[j].length > (int)((float)projNum/20.0f))
					{
						facecomp.left_eye.rect.y = facecomp.right_eye.rect.y = segment[j].start;
						facecomp.left_eye.rect.height = facecomp.right_eye.rect.height = segment[j].length;
						break;
					}
				}

				if (j != est_num2)
					break;
				// fail to find eyes or eyebrows.
				else
				{
					// found by heuristic
					// eye의 크기 = 20 pixel
					facecomp.left_eye.rect.y = facecomp.right_eye.rect.y 
						= facecomp.left_eyebrow.rect.y + facecomp.left_eyebrow.rect.height+ 20;
					facecomp.left_eye.rect.height = facecomp.right_eye.rect.height = 20;
				}
			}
		}

		gaussmax = -1;
		for (i=face_index[1]+1; i<est_num2; i++)
		{
			if ((segment[i].length > (int)((float)projNum/20.0f)) && (segment[i].gauss > gaussmax))
			{
				gaussmax = segment[i].gauss;

				facecomp.nose.rect.y = segment[i].start;
				facecomp.nose.rect.height = segment[i].length;

				break;
			}
		}

		for (j=i+1; j<est_num2; j++)
		{
			if ((segment[j].length > (int)((float)projNum/20.0f)))
			{
				facecomp.mouth.rect.y = segment[j].start;
				facecomp.mouth.rect.height = segment[j].length;

				break;
			}
		}

		msgview->AddLine("Lower");
	}
	// Nose is located in the middle of face_index[1] and face_index[0].
	else
	{
		for (i=0; i<face_index[0]; i++)
		{
			//if (segment[i].length > (int)((float)projNum/20.0f))
			if (segment[i].length > (int)ref_Length)
			{
				facecomp.left_eyebrow.rect.y = facecomp.right_eyebrow.rect.y = segment[i].start;
				facecomp.left_eyebrow.rect.height = facecomp.right_eyebrow.rect.height = segment[i].length;

				for (j=i+1; j<est_num2; j++)
				{
					//if (segment[j].length > (int)((float)projNum/20.0f))
					if (segment[j].length > (int)ref_Length)
					{
						facecomp.left_eye.rect.y = facecomp.right_eye.rect.y = segment[j].start;
						facecomp.left_eye.rect.height = facecomp.right_eye.rect.height = segment[j].length;
						break;
					}
				}

				if (j != est_num2)
					break;
				// fail to find eyes or eyebrows.
				else
				{
					facecomp.left_eye.rect.y = facecomp.right_eye.rect.y 
						= facecomp.left_eyebrow.rect.y + facecomp.left_eyebrow.rect.height+ 20;
					facecomp.left_eye.rect.height = facecomp.right_eye.rect.height = 20;
				}
			}
		}
		
		//gaussmax = -1;
		for (i=face_index[0]+1; i<face_index[1]+1; i++)
		{
			//if ((segment[i].length > (int)((float)projNum/20.0f)) && (segment[i].gauss > gaussmax))
			if (segment[i].length > (int)ref_Length)
			{
				//gaussmax = segment[i].gauss;

				facecomp.nose.rect.y = segment[i].start;
				facecomp.nose.rect.height = segment[i].length;

				break;
			}
		}

		for (i=face_index[1]+1; i<est_num; i++)
		{
			//if ((segment[i].length > (int)((float)projNum/20.0f)))
			if (segment[i].length > (int)ref_Length)
			{
				facecomp.mouth.rect.y = segment[i].start;
				facecomp.mouth.rect.height = segment[i].length;

				break;
			}
		}

		msgview->AddLine("Middle");
	}

	//////////////////////////////////////////////////////

	// eyebrows
	if (facecomp.left_eyebrow.rect.height > 0)
	{
		iplSet(grayimg[1], 0);
		cvSetImageROI(A, cvRect(0, __min(facecomp.left_eyebrow.t(), facecomp.right_eyebrow.t()), 
			A->width, __max(facecomp.left_eyebrow.rect.height, facecomp.right_eyebrow.rect.height)));
		iplCopy(A, grayimg[1]);
		A->roi = NULL;

		IVSTKproject2(grayimg[1], projX, 0);
		IVSTKsubsearch(projX, cvPoint(0, A->width), 0, facecomp);
	}

	// eyes
	if (facecomp.left_eye.rect.height > 0)
	{
		iplSet(grayimg[1], 0);
		cvSetImageROI(A, cvRect(0, __min(facecomp.left_eye.t(), facecomp.right_eye.t()), 
			A->width, __max(facecomp.left_eye.rect.height, facecomp.right_eye.rect.height)));
		iplCopy(A, grayimg[1]);
		A->roi = NULL;

		IVSTKproject2(grayimg[1], projX, 0);
		IVSTKsubsearch(projX, cvPoint(0, A->width), 1, facecomp);
	}

	// nose
	if (facecomp.nose.rect.height > 0)
	{
		iplSet(grayimg[1], 0);
		cvSetImageROI(A, cvRect(0, facecomp.nose.t(), A->width, facecomp.nose.rect.height));
		iplCopy(A, grayimg[1]);
		A->roi = NULL;

		IVSTKproject2(grayimg[1], projX, 0);
		IVSTKsubsearch(projX, cvPoint(0, A->width), 2, facecomp);
	}

	// mouth
	if (facecomp.mouth.rect.height > 0)
	{
		iplSet(grayimg[1], 0);
		cvSetImageROI(A, cvRect(0, facecomp.mouth.t(), A->width, facecomp.mouth.rect.height));
		iplCopy(A, grayimg[1]);
		A->roi = NULL;

		IVSTKproject2(grayimg[1], projX, 0);
		IVSTKsubsearch(projX, cvPoint(0, A->width), 3, facecomp);
	}

	delete [] segment;
	segment = NULL;
}

// 20010615@05:09@DJ
// 20030502 IPL & CV
// eyes and eyebrows.
// even mouth and nose.
void CIVSTK::IVSTKsubsearch(CvMat proj, CvPoint range, int mode, CIVSTKface& facecomp)
{
	int i, j;
	int est_num = 0, est_num2 = 0;
	CIVSTKsegment *segment;

	int projNum = range.y - range.x;
	float ref_Length = ((float)(range.y - range.x)*0.01f);

	float max_proj = 0;
	for (i=range.x; i<range.y; i++)
	{
		if (proj.data.fl[i] > max_proj)
			max_proj = proj.data.fl[i];
	}

	float th = max_proj*(0.01f);	// threshold

	// first scan
	for (i=range.x; i<range.y; i++)
	{
		if ((proj.data.fl[i] > th) && i == 0)
			est_num++;
		else if (i != 0)
		{
			if (proj.data.fl[i] > th && proj.data.fl[i-1] <= th)
				est_num++;
		}
	}

	segment = new CIVSTKsegment[est_num+1];

	// second scan
	for (i=range.x; i<range.y; i++)
	{
		if (proj.data.fl[i] > th && i == 0)
		{
			segment[est_num2].start = i;
			segment[est_num2].sum += (long)proj.data.fl[i];
		}
		else if (i != 0)
		{
			if (proj.data.fl[i] > th && proj.data.fl[i-1] <= th)
			{
				segment[est_num2].sum += (long)proj.data.fl[i];
				segment[est_num2].start = i;
			}
			else if (proj.data.fl[i-1] > th && proj.data.fl[i] <= th)
			{
				segment[est_num2].end = i-1;
				est_num2++;
			}
			else if (proj.data.fl[i] > th)
			{
				segment[est_num2].sum += (long)proj.data.fl[i];
			}
		}
	}

	if (proj.data.fl[i-1] > th)
		segment[est_num2].end = i-1;

	for (i=0; i<est_num2; i++)
		segment[i].length = segment[i].end - segment[i].start + 1;

	for (i=0; i<est_num2; i++)
	{
		if (segment[i].length > (int)ref_Length)
		{
			switch(mode)
			{
				case 0:
					facecomp.left_eyebrow.rect.x = segment[i].start;
					facecomp.left_eyebrow.rect.width = segment[i].length;
					break;
				case 1:
					facecomp.left_eye.rect.x = segment[i].start;
					facecomp.left_eye.rect.width = segment[i].length;
					break;
				case 2:
					facecomp.nose.rect.x = segment[i].start;
					facecomp.nose.rect.width = segment[i].length;
					break;
				case 3:
					facecomp.mouth.rect.x = segment[i].start;
					facecomp.mouth.rect.width = segment[i].length;
					break;
				default:
					break;
			}
		}

		// only for eyes, eyebrows and nose...
		if (mode < 3)
		{
			for (j=i+1; j<est_num2; j++)
			{
				if (segment[j].length > (int)ref_Length)
				{
					switch(mode)
					{
						case 0:
							facecomp.right_eyebrow.rect.x = segment[j].start;
							facecomp.right_eyebrow.rect.width = segment[j].length;
							break;
						case 1:
							facecomp.right_eye.rect.x = segment[j].start;
							facecomp.right_eye.rect.width = segment[j].length;
							break;
						case 2:
							if (facecomp.nose.rect.x == 0)
								facecomp.nose.rect.x = segment[j].start;

							facecomp.nose.rect.width = (segment[j].end - facecomp.nose.rect.x);
							break;
						default:
							break;
					}
					break;
				}
			}
		}
		break;
	}
	
	delete [] segment;
}

//20030503 AND  OR NOT  XOR MULT DIV SUB ADD
void CIVSTK::IVSTKalu(IPLIMAGE src, IPLIMAGE dst, CString str, BOOL flag[3], float Const)
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);

	CMainFrame *main = (CMainFrame*)AfxGetMainWnd();

	for (i=0; i<3; i++)
	{
		if ((flag[i] != 0) && ((main->GetMemoryImage(i+1))->GetSingleImage())->nChannels == 3)
			cvCvtColor((main->GetMemoryImage(i+1))->GetSingleImage(), grayimg[i], CV_BGR2GRAY);
	}

	// just for 2 images!!!
	if (!str.Compare("AND"))
	{
		iplAnd(grayimg[0], grayimg[1], grayimg[4]);
	}
	else if (!str.Compare("OR"))
	{
		iplOr(grayimg[0], grayimg[1], grayimg[4]);
	}
	else if (!str.Compare("NOT"))
	{
		iplNot(grayimg[0], grayimg[4]);
	}	
	else if (!str.Compare("XOR"))	
	{
		iplXor(grayimg[0], grayimg[1], grayimg[4]);
	}
	else if (!str.Compare("MULT"))
	{
		iplMultiply(grayimg[0], grayimg[1], grayimg[4]);
	}
	else if (!str.Compare("DIV"))
	{
		//iplDivide(grayimg[0], grayimg[1], grayimg[4]);
	}
	else if (!str.Compare("SUB"))
	{
		iplSubtract(grayimg[0], grayimg[1], grayimg[4]);
	}
	else if (!str.Compare("ADD"))
	{
		iplAdd(grayimg[0], grayimg[1], grayimg[4]);
	}
	
	debug[0]->ShowSingleImage(grayimg[4]);

	CString str2 = "ALU operation : " + str;
	msgview->AddLine(str2);
}

// 20030507
// MainFrm 과의 통신 문제??
void CIVSTK::IVSTKrecFMMNN(CString str, float param[])
{
	int i, j;
	float **data;
	int *classInfo;
	int dataLength;
	int numInput, numClass;
	float Theta, Gamma;
	int *cmat;
	float rr = 0.0f;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();

	CFileDialog *pDlg = new CFileDialog( TRUE, "fmm", NULL, OFN_READONLY, 
		"입력패턴파일 *.fmm|*.fmm|모든파일 *.*|*.*|", NULL );

	CFileDialog *pDlg2 = new CFileDialog( TRUE, "bxd", NULL, OFN_READONLY, 
		"FMMNN포맷파일 *.bxd|*.bxd|모든파일 *.*|*.*|", NULL );

	if (!str.Compare("TRAIN"))
	{
		if (pDlg->DoModal())
		{
			CString filename = pDlg->GetFileName();

			if (filename != "")
			{
				FILE *fp;

				fp = fopen(filename, "rt");
				fscanf(fp, "%d %d %d %f %f", &dataLength, &numInput, &numClass, &Theta, &Gamma);

				CString str2;
				str2.Format("\r\nData File[%s]", filename);
				(mainframe->GetDJMessage())->AddLine(str2);
				str2.Format("Total[%d] In[%d] Out[%d] Theta[%.2f] Gamma[%.2f]", dataLength, numInput, numClass, Theta, Gamma);
				(mainframe->GetDJMessage())->AddLine(str2);

				m_fmmnn.Init(numInput, numClass, Theta, Gamma);

				data = new float*[dataLength];
				classInfo = new int[dataLength];

				for (i=0; i<dataLength; i++)
				{
					data[i] = new float[numInput];
					for (j=0; j<numInput; j++)			fscanf(fp, "%f", &(data[i][j]));
					fscanf(fp, "%d", &(classInfo[i]));
				}
				fclose(fp);

				//filename += ".out";
				//fp = fopen(filename, "wt");

				//for (k=0; k<10; k++)
				{
				//	m_fmmnn.SetTheta((double)((double)k*0.01f));

					(mainframe->GetDJMessage())->AddLine("\r\nLearning Start!");

					::SetCursor(::LoadCursor(NULL, IDC_WAIT));
					for (i=0; i<dataLength; i++)
						m_fmmnn.Training(classInfo[i], data[i]);
					::SetCursor(::LoadCursor(NULL, IDC_ARROW));


					str2.Format("Learning End [%d]", m_fmmnn.GetTotalBoxNum());
					(mainframe->GetDJMessage())->AddLine(str2);

					//fprintf(fp, "%.2f %d %.2f \n", m_fmmnn.GetTheta(), m_fmmnn.GetTotalBoxNum(), rr);
				}
				//fclose(fp);


				if (pDlg2->DoModal())
				{
					filename = pDlg2->GetFileName();

					if (filename != "")
					{
						fp = fopen(filename, "wt");
						m_fmmnn.SaveBox(fp);
						str2.Format("\r\nBox File[%s]", filename);
						(mainframe->GetDJMessage())->AddLine(str2);

						fclose(fp);
					}
				}

				delete [] classInfo; classInfo = NULL;
				delete [] data;	data = NULL;
			}
		}
	}
	else if (!str.Compare("TEST"))
	{
		if (pDlg->DoModal())
		{
			CString filename = pDlg->GetFileName();

			if (filename != "")
			{
				FILE *fp;

				// load data file
				fp = fopen(filename, "rt");
				fscanf(fp, "%d %d %d %f %f", &dataLength, &numInput, &numClass, &Theta, &Gamma);
				
				CString str2;
				str2.Format("\r\nData File[%s]", filename);
				(mainframe->GetDJMessage())->AddLine(str2);
				str2.Format("Total[%d] In[%d] Out[%d] Theta[%.2f] Gamma[%.2f]", dataLength, numInput, numClass, Theta, Gamma);
				(mainframe->GetDJMessage())->AddLine(str2);

				m_fmmnn.Init(numInput, numClass, Theta, Gamma);

				data = new float*[dataLength];
				classInfo = new int[dataLength];

				cmat = new int[numClass*numClass];
				for (i=0; i<numClass; i++)
					for (j=0; j<numClass; j++)
						cmat[i*numClass + j] = 0;

				// data loading
				for (i=0; i<dataLength; i++)
				{
					data[i] = new float[numInput];
					for (j=0; j<numInput; j++)			fscanf(fp, "%f", &(data[i][j]));
					fscanf(fp, "%d", &(classInfo[i]));
				}
				fclose(fp);

				if (pDlg2->DoModal())
				{
					filename = pDlg2->GetFileName();

					if (filename != "")
					{
						m_fmmnn.LoadBox2(filename);

						str2.Format("\r\nBox File[%s]", filename);
						(mainframe->GetDJMessage())->AddLine(str2);

						(mainframe->GetDJMessage())->AddLine("\r\nTest Start!");
						// test
						for (i=0; i<dataLength; i++)
							cmat[classInfo[i]*numClass + m_fmmnn.Test(data[i])]++;

						for (i=0; i<numClass; i++)
							rr += cmat[i*numClass + i];

						rr /= (float)dataLength;

						str2.Format("\r\nRec. Rate for Test Set[%.2f]", rr*100.0f);
						(mainframe->GetDJMessage())->AddLine(str2);

						CString str3;
						str3.Format("");
						for (i=0; i<numClass; i++)
						{
							str2.Format("%d ", cmat[i*numClass+i]);
							str3 += str2;
						}
						(mainframe->GetDJMessage())->AddLine(str3);

						delete [] cmat; cmat = NULL;
						delete [] classInfo; classInfo = NULL;
						delete [] data;	data = NULL;					
					}
				}
			}
		}
	}


	/*
	/////////////////////////////////
	int i, j;
	float **data;
	int *classInfo;
	int dataLength;
	int numInput, numClass;
	float Theta, Gamma;
	CString message, message2;
	FILE *fp;

*/
}

//20030507
void CIVSTK::IVSTKrecNN(CString str, float param[])
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();

	CFileDialog *pDlg = new CFileDialog( TRUE, "dat", NULL, OFN_READONLY, 
		"입력패턴파일 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

	CFileDialog *pDlg2 = new CFileDialog( TRUE, "fnn", NULL, OFN_READONLY, 
		"NN포맷파일 *.nn|*.nn|모든파일 *.*|*.*|", NULL );

	if (!str.Compare("TRAIN"))
	{
		if (pDlg->DoModal())
		{
			CString filename = pDlg->GetFileName();

			if (filename != "")
			{
				int layer[] = {5, 10, 32, 14, 7};

				m_nn.NNalloc(layer, 5, 0.15, 1e3, 1e-3, 0.8);	

				int no_sample = 350;
				int i, outputv;//j, outputv;

				float* input;
				float* output;
				input = new float[no_sample*layer[0]];
				output = new float[no_sample*layer[4]];

				for (i=0; i<no_sample*layer[4]; i++)
					output[i] = 0.2f;

				FILE *fp;

				fp = fopen(filename, "rt");

				for (i=0; i<no_sample; i++)
				{
					fscanf(fp, "%f %f %f %f %f %d ", &input[i*layer[0]], &input[i*layer[0]+1], &input[i*layer[0]+2], 
						&input[i*layer[0]+3], &input[i*layer[0]+4], &outputv);

					output[i*layer[4]+outputv] = 0.8f;
				}

				fclose(fp);

				::SetCursor(::LoadCursor(NULL, IDC_WAIT));
				m_nn.NNtrain(input, output, no_sample);
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));

				if (pDlg2->DoModal())
				{
					filename = pDlg2->GetFileName();

					if (filename != "")
					{
						m_nn.NNsave(filename);
					}
				}

				m_nn.NNeval((float *)input, (float *)output, no_sample);

				delete [] input;	input = NULL;
				delete [] output;	output = NULL;
			}
		}
	}
	else if (!str.Compare("TEST"))
	{
		if (pDlg2->DoModal())
		{
			CString filename = pDlg2->GetFileName();

			if (filename != "")
			{
				int layer[] = {5, 10, 32, 14, 7};

				m_nn.NNalloc(layer, 5, 0.15, 1e3, 1e-3, 0.8);	

				int no_sample = 350;
				int i, outputv;//j, outputv;

				float* input;
				float* output;
				input = new float[no_sample*layer[0]];
				output = new float[no_sample*layer[4]];

				FILE *fp;

				m_nn.NNload(filename);

				if (pDlg->DoModal())
				{
					filename = pDlg->GetFileName();

					if (filename != "")
					{
						fp = fopen(filename, "rt");

						for (i=0; i<no_sample*layer[4]; i++)
							output[i] = 0.2f;

						for (i=0; i<no_sample; i++)
						{
							fscanf(fp, "%f %f %f %f %f %d", &input[i*layer[0]], &input[i*layer[0]+1], &input[i*layer[0]+2], 
								&input[i*layer[0]+3], &input[i*layer[0]+4], &outputv);

							output[i*layer[4]+outputv] = 0.8f;
						}

						fclose(fp);

						m_nn.NNeval((float *)input, (float *)output, no_sample);

					}
				}

				delete [] input;	input = NULL;
				delete [] output;	output = NULL;
			}
		}

	}
}

// 20030507
// MainFrm과의 통신 문제! -> FN 값을 갱신하고 다시 쓰는 방법?
// free, save, load...
void CIVSTK::IVSTKrecFNN(CString str, float param[])
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	m_fnn.logview = mainframe->GetDJMessage();

	CFileDialog *pDlg = new CFileDialog( TRUE, "dat", NULL, OFN_READONLY, 
		"입력패턴파일 *.dat|*.dat|모든파일 *.*|*.*|", NULL );

	CFileDialog *pDlg2 = new CFileDialog( TRUE, "fnn", NULL, OFN_READONLY, 
		"FNN포맷파일 *.fnn|*.fnn|모든파일 *.*|*.*|", NULL );

	if (!str.Compare("TRAIN"))
	{
		if (pDlg->DoModal())
		{
			CString filename = pDlg->GetFileName();

			if (filename != "")
			{
				if (m_fnn.init_flag)
					m_fnn.freeFNN();

				m_fnn.loadDataN1(filename);	// raw data

				// load fnn structure file
				int n = filename.Find(".");
				filename.Delete(n, 4);
				filename.Insert(n, ".fn");
				m_fnn.loadFN(filename);	// fnn parameters
				
				m_fnn.initFNN();
				m_fnn.setMembership();
				m_fnn.compFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);

				m_fnn.recTrain();

				m_fnn.ruleCombination();
				m_fnn.ioHistogram();
				//m_fnn.SaveIOHistogram(".\\myIOHisto.dat");

				::SetCursor(::LoadCursor(NULL, IDC_WAIT));
				m_fnn.learnFNN(m_fnn.lin, m_fnn.lout, m_fnn.l_no);
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));

				m_fnn.recTrain();
				m_fnn.recTest();

				//m_fnn.ShowMF(0); 	m_fnn.ShowMF(1);
				//m_fnn.ShowFNND();

				if( pDlg2->DoModal() )
				{
				   filename = pDlg2->GetFileName();

				   if( filename != "" )  
				   {
					   m_fnn.saveFNN(filename);	// save trained fnn
				   }
				}
			}
		}
	}
	else if (!str.Compare("TEST"))
	{
		/*
		char seps[] = " ";
		char *token;
		int no_nodes = 0;
		int* nodes = new int[m_fnn.layer[1]];
		int n;

		token = strtok((char*)(LPCTSTR)FNNkillnodes, seps);

		while (token != NULL)
		{
			nodes[no_nodes++] = atoi(token);
			token = strtok( NULL, seps);
		}
		*/

		if( pDlg2->DoModal() )
		{
		   CString filename = pDlg2->GetFileName();
   
		   if( filename != "" )  
		   {
			   m_fnn.loadFNN(filename);	// load saved fnn structure

				if( pDlg->DoModal() )
				{
				   filename = pDlg->GetFileName();

				   if( filename != "" )  
				   {
					   m_fnn.loadDataN1(filename);	// load data file

					   m_fnn.initFNN();
					   m_fnn.ioHistogram();

						// delete input nodes
						//m_fnn.deleteInputNodes(nodes, no_nodes);

						//m_fnn.ShowMF(0); 	m_fnn.ShowMF(1);
						//m_fnn.ShowFNND();

					   m_fnn.recTrain();
					   m_fnn.recTest();
				   }
				}
		   }
		}
		
		//delete [] nodes;
	}
}

//20030508
// 삽질 성공 -_-v~
void CIVSTK::IVSTKeigenobject(IPLIMAGE src, IPLIMAGE dst, CString str, float param[])
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	/*
	imgview->ShowSingleImage(src);

	if (src->nChannels == 3)
		cvvConvertImage(src, grayimg[0], 0);
	else
		iplCopy(src, grayimg[0]);
		*/

	// Multifile loading!
	CFileDialog *pDlg = new CFileDialog( TRUE, "jpg", NULL, OFN_ALLOWMULTISELECT, 
		"입력영상 *.jpg|*.jpg|모든파일 *.*|*.*|", NULL );

	TCHAR* m_pszFile = new TCHAR[2048];
	m_pszFile[0] = '\0';
	pDlg->m_ofn.lpstrFile = m_pszFile;
	pDlg->m_ofn.nMaxFile = 2048;

	if (!str.Compare("TRAIN"))
	{
		if (pDlg->DoModal())
		{
			CPtrList filelist;
			filelist.RemoveAll();
			POSITION pos = pDlg->GetStartPosition();
			CString filename;
			CvSize mysize;

			IplImage *simage, *fimage;

			do 
			{
				filename = pDlg->GetNextPathName(pos);
				
				if (filename != "")
					fimage = cvLoadImage(filename);
				
				mysize = cvSize(fimage->width, fimage->height);
				simage = cvCreateImage(mysize, IPL_DEPTH_8U, 1);

				iplColorToGray(fimage, simage);

				filelist.AddTail((IPLIMAGE)simage);

				//cvReleaseImage(&simage);
			} while (pos != NULL);

			// eigenobject parameters
			CvTermCriteria calcLimit;
			calcLimit.type = CV_TERMCRIT_EPS;
			calcLimit.maxIter = filelist.GetCount();
			calcLimit.epsilon = 0;

			IPLIMAGE* input;	input = new IPLIMAGE[filelist.GetCount()];
			IPLIMAGE* output;	output = new IPLIMAGE[filelist.GetCount()];

			for (i=0; i<filelist.GetCount(); i++)
			{
				input[i] = (IPLIMAGE)filelist.GetAt(filelist.FindIndex(i));
				output[i] = cvCreateImage(mysize, IPL_DEPTH_32F, 1);
			}

			IPLIMAGE avgimg = cvCreateImage(mysize, IPL_DEPTH_32F, 1);
			IPLIMAGE avgimg2 = cvCreateImage(mysize, IPL_DEPTH_8U, 1);

			float* eigVals = new float[filelist.GetCount()];

			::SetCursor(LoadCursor(NULL, IDC_WAIT));
			cvCalcEigenObjects(filelist.GetCount(), (void*)input, (void*)output, CV_EIGOBJ_NO_CALLBACK, 0, NULL, &calcLimit, avgimg, eigVals);
			::SetCursor(LoadCursor(NULL, IDC_ARROW));

			// show results
			IVSTKnormalize(output[0], avgimg2);
			debug[0]->ShowSingleImage(avgimg2);
			IVSTKnormalize(output[1], avgimg2);
			debug[1]->ShowSingleImage(avgimg2);
			IVSTKnormalize(output[2], avgimg2);
			debug[2]->ShowSingleImage(avgimg2);
			IVSTKnormalize(avgimg, avgimg2);
			debug[3]->ShowSingleImage(avgimg2);

			// memory free!
			delete [] eigVals;	eigVals = NULL;
			cvReleaseImage(&avgimg2);
			cvReleaseImage(&avgimg);

			for (i=0; i<filelist.GetCount(); i++)
				cvReleaseImage(&(output[i]));
			
			delete [] output;	output = NULL;
			delete [] input;	input = NULL;

			filelist.RemoveAll();
		}
	}

	//debug[0]->ShowSingleImage(grayLPimg[0]);

	//CString str2 = "Coordinate Transform : " + str;
	//msgview->AddLine(str2);
}

//20030508
void CIVSTK::IVSTKmotion(IPLIMAGE left, IPLIMAGE right, CString str)
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	cvCvtColor(left, grayimg[0], CV_BGR2GRAY);
	cvCvtColor(right, grayimg[1], CV_BGR2GRAY);

	IPLIMAGE velx = cvCreateImage(cvSize(left->width/5, left->height/5), IPL_DEPTH_32F, 1);
	IPLIMAGE vely = cvCreateImage(cvSize(left->width/5, left->height/5), IPL_DEPTH_32F, 1);
	IPLIMAGE velxg = cvCreateImage(cvSize(left->width/5, left->height/5), IPL_DEPTH_8U, 1);
	IPLIMAGE velyg = cvCreateImage(cvSize(left->width/5, left->height/5), IPL_DEPTH_8U, 1);

	if (!str.Compare("HS"))
	{
		CvTermCriteria criteria;
        
		criteria.type = CV_TERMCRIT_ITER;
		criteria.maxIter = 100;
		criteria.epsilon = 0.1f;

		cvCalcOpticalFlowHS(grayimg[0], grayimg[1], 0, floatimg[0], floatimg[1], 100, criteria);

		IVSTKnormalize(floatimg[0], grayimg[5]);
		IVSTKnormalize(floatimg[1], grayimg[6]);

		debug[0]->ShowSingleImage(grayimg[5]);
		debug[1]->ShowSingleImage(grayimg[6]);

		cvSetZero(colimg[0]);
		IVSTKdrawflow(floatimg[0], floatimg[1], colimg[0]);
		debug[2]->ShowSingleImage(colimg[0]);
	}
	else if (!str.Compare("LK"))
	{
		cvCalcOpticalFlowLK(grayimg[0], grayimg[1], cvSize(5,5), floatimg[0], floatimg[1]);

		IVSTKnormalize(floatimg[0], grayimg[5]);
		IVSTKnormalize(floatimg[1], grayimg[6]);

		debug[0]->ShowSingleImage(grayimg[5]);
		debug[1]->ShowSingleImage(grayimg[6]);

		cvSetZero(colimg[0]);
		IVSTKdrawflow(floatimg[0], floatimg[1], colimg[0]);
		debug[2]->ShowSingleImage(colimg[0]);
	}
	else if (!str.Compare("BM"))
	{
		cvCalcOpticalFlowBM(grayimg[0], grayimg[1], cvSize(5,5), cvSize(5,5), cvSize(5,5), 0, velx, vely);

		IVSTKnormalize(velx, velxg);
		IVSTKnormalize(vely, velyg);

		debug[0]->ShowSingleImage(velxg);
		debug[1]->ShowSingleImage(velyg);

		//IVSTKdrawflow(velx, vely, colimg[0]);
		//debug[2]->ShowSingleImage(colimg[0]);
	}
	else if (!str.Compare("PYRLK"))
	{
		/*
		cvCalcOpticalFlowPyrLK(grayimg[0], grayimg[1], CvArr* pyrA, CvArr* pyrB,
                             CvPoint2D32f* featuresA, CvPoint2D32f* featuresB,
                             int count, CvSize winSize, int level, char* status,
                             float* error, CvTermCriteria criteria , int flags );
							 */
	}

	cvReleaseImage(&velyg);
	cvReleaseImage(&velxg);
	cvReleaseImage(&vely);
	cvReleaseImage(&velx);
}

//20030508
void CIVSTK::IVSTKdepth(IPLIMAGE left, IPLIMAGE right, CString str)
{
	int i;

	imgview->SetImagePlane(1, 1);
	for (i=0; i<4; i++)
		debug[i]->SetImagePlane(1, 1);

	cvCvtColor(left, grayimg[0], CV_BGR2GRAY);
	cvCvtColor(right, grayimg[1], CV_BGR2GRAY);

	if (!str.Compare("BIRCHFIELD"))
		cvFindStereoCorrespondence( grayimg[0], grayimg[1], CV_DISPARITY_BIRCHFIELD, grayimg[2], 50, 25, 5, 12, 15, 25 );

	debug[0]->ShowSingleImage(grayimg[2]);
}

//#define GRIDSIZE	4

//20030509
void CIVSTK::IVSTKdrawflow(IPLIMAGE velx, IPLIMAGE vely, IPLIMAGE vel, int gridsize)
{
	CvPoint p0, p1; 
	
	for ( int y = 0; y < velx->height; y += gridsize ) 
	{ 
		p0.y = y; 
		
		for ( int x = 0; x < velx->width; x += gridsize ) 
		{ 
			p0.x = x; 
			
			p1.x = p0.x + int(((float*)(velx->imageData + velx->widthStep*y))[x]); 
			p1.y = p0.y + int(((float*)(vely->imageData + vely->widthStep*y))[x]); 

			if ( abs( p0.x - p1.x) > gridsize || abs( p0.y - p1.y ) > gridsize ) 
			{ 
				// draw a red line from p0 to p1. 
				cvLine( vel, p0, p1, 0xFF0000, 1 ); // BGR 
				// draw a filled green circle with r = 1 around p1. 
				cvCircle( vel, p1, 1, 0x00FF00, -1 ); 
			} 
		} 
	} 
}