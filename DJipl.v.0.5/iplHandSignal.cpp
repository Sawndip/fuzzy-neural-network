// iplHandSignal.cpp: implementation of the iplHandSignal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pic.h"
#include "iplHandSignal.h"
#include "ipl.h"
#include "cv.h"
#include "highgui.h"
#include "math.h"

#define M_PI	(3.141592)
#define DEG2RAD	(M_PI/180.0)
#define RAD2DEG (180.0/M_PI)
#define DEBUG	(1)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

iplHandSignal::iplHandSignal()
{
	cvvNamedWindow("HS", 0);

	IPLIMAGE image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
	iplFaceAllocImage(image);
	cvReleaseImage(&image);

	grabImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
	cvtImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
	blobImage = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);

	cvInitFont(&font, CV_FONT_VECTOR0, 1.0f, 1.0f, 0.0f, 1);
}

iplHandSignal::~iplHandSignal()
{
	cvReleaseImage(&(blobImage));
	cvReleaseImage(&(cvtImage));
	cvReleaseImage(&(grabImage));

	iplFaceFreeImage();
	cvvDestroyWindow("HS");
}

void iplHandSignal::iplGetHSorientation(IPLIMAGE image, float& max_angle, int& flip_x, int& flip_y, IPLIMAGE image2)
{
	int i, j;
	int xc, yc;
	int length = 150;
	int m_area[2][2];

	// get &moment
	cvMoments(image, &moment, 0);

	xc = (int)(cvGetSpatialMoment(&moment, 1, 0)/cvGetSpatialMoment(&moment, 0, 0));
	yc = (int)(cvGetSpatialMoment(&moment, 0, 1)/cvGetSpatialMoment(&moment, 0, 0));

	max_angle = (float)(atan2(2.0*((cvGetSpatialMoment(&moment, 1, 1)/cvGetSpatialMoment(&moment, 0, 0)) - xc*yc), 
		((cvGetSpatialMoment(&moment, 2, 0)/cvGetSpatialMoment(&moment, 0, 0)) - xc*xc)
		-((cvGetSpatialMoment(&moment, 0, 2)/cvGetSpatialMoment(&moment, 0, 0)) - yc*yc)))/2.0;

	/*
	cvCircle(procCPGimg[0], cvPoint(xc, yc), 5, 0x55, -1);
	cvLine(procCPGimg[0], cvPoint(xc + length*cos(max_angle), yc - length*sin(max_angle)), 
		cvPoint(xc - length*cos(max_angle), yc + length*sin(max_angle)), 0x55, 5);
	*/

	const double coeffs[2][3] = {{1, 0, 320-xc}, {0, 1, 240-yc}};
	iplSet(procCPGimg[2], 0);
	iplSet(procCPGimg[3], 0);
	iplWarpAffine(procCPGimg[0], procCPGimg[2], coeffs, IPL_INTER_LINEAR);
	iplRotateCenter(procCPGimg[2], procCPGimg[3], (-max_angle)*RAD2DEG, 320, 240, IPL_INTER_LINEAR);
	// iplSaveAnyImage("", IPLIMAGE) 만들자.

	iplSet(blobImage, 0);
	iplCopy(procCPGimg[3], blobImage);
	
	/// blob analysis	///
	CvSeq* chain = 0;

	cvFindContours(blobImage, storage, &chain, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double max_area=0, area;
	CvSeq *contour=NULL;
	for(CvSeq* c = chain; c != 0; c = c->h_next )
	{
		area = fabs(cvContourArea(c));
		if (area > max_area) { max_area=area; contour=c; }
	}
	contMayor=contour;
	///////////////////////

	rect = cvContourBoundingRect(contMayor, 1);

	CvPoint cp_rect = cvPoint(rect.x+rect.width/2, rect.y+rect.height/2);
	/*
	cvRectangle(procCPGimg[3], cvPoint(rect.x, rect.y), cvPoint(rect.x+rect.width, rect.y+rect.height), 0xFF, 3);
	cvCircle(procCPGimg[3], cp_rect, 5, 0x99, -1);
	cvCircle(procCPGimg[3], cvPoint(320, 240), 5, 0x33, -1);
	*/

	float min_area = 1e8;
	int mi=0, mj=0;
	for (j=0; j<2; j++)
		for (i=0; i<2; i++)
		{
			cvSetImageROI(procCPGimg[3], cvRect(rect.x + (rect.width/2)*i, rect.y + (rect.height/2)*j, rect.width/2, rect.height/2));
			m_area[i][j] = (float)cvMean(procCPGimg[3]);

			if (m_area[i][j] < min_area)
			{
				min_area = m_area[i][j];
				mi = i;
				mj = j;
			}
		}

	procCPGimg[3]->roi = NULL;

	//cvCircle(procCPGimg[3], cvPoint(rect.x + (rect.width/2)*mi + rect.width/4, rect.y + (rect.height/2)*mj + rect.height/4), 10, 0x99, -1);
	cvRectangle(procCPGimg[3], cvPoint(rect.x + (rect.width/2)*mi, rect.y + (rect.height/2)*mj), 
		cvPoint(rect.x + (rect.width/2)*(mi+1), rect.y + (rect.height/2)*(mj+1)), 0xAA, 3);


	if (mi==0) //(cp_rect.x < 320)
	{
		iplSet(procCPGimg[4], 0);
		iplMirror(procCPGimg[3], procCPGimg[4], 1);

		flip_x = 1;
	}
	else
		flip_x = 0;

	if (mj==1)//((cp_rect.y > 240) || (mj == 1))
	{
		if (flip_x == 1)
		{
			iplSet(procCPGimg[5], 0);
			iplMirror(procCPGimg[4], procCPGimg[5], 0);
		}
		else
		{
			iplSet(procCPGimg[4], 0);
			iplMirror(procCPGimg[3], procCPGimg[4], 0);
		}

		flip_y = 1;
	}
	else
		flip_y = 0;

	iplSet(image2, 0);
	iplCopy(procCPGimg[3+flip_x+flip_y], image2);
	// char str[100];
	//sprintf(str, "%.2f DEG", (-max_angle)*RAD2DEG);
	//cvPutText(image2, str, cvPoint(500, 30), &font, (int)(0xAAAAAA));
	//sprintf(str, "Flip: %1dX %1dY", flip_x, flip_y);
	//cvPutText(image2, str, cvPoint(200, 30), &font, (int)(0xAAAAAA));
	cvvShowImage("HS", image2);
	cvvConvertImage(image2, procCPCimg[0], 0);
	cvvSaveImage("D:\\process001.jpg", procCPCimg[0]);

	cvvWaitKey(0);

}

void iplHandSignal::iplGetHSpose(IPLIMAGE image, float *gray16, float& xy_ratio, float& finger_ratio)
{
	unsigned char pixel;
	float sumxx;
	float *sumx;

	iplSet(blobImage, 0);
	iplCopy(image, blobImage);
	
	/// blob analysis	///
	CvSeq* chain = 0;

	cvFindContours(blobImage, storage, &chain, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double max_area=0, area;
	CvSeq *contour=NULL;
	for(CvSeq* c = chain; c != 0; c = c->h_next )
	{
		area = fabs(cvContourArea(c));
		if (area > max_area) { max_area=area; contour=c; }
	}
	contMayor=contour;
	///////////////////////
	
	CvRect rect = cvContourBoundingRect(contMayor, 1);

	int i, j;
	CvRect subrect;

	for (j=0; j<4; j++)
	{
		for (i=0; i<4; i++)
		{
			subrect = cvRect(rect.x + (rect.width/4)*i, rect.y + (rect.height/4)*j, rect.width/4, rect.height/4);
			cvSetImageROI(image, subrect);

			gray16[i+j*4] = (float)cvMean(image);
			//pixel = (unsigned char)gray16[i+j*4];
			
			//iplPutPixel(image_gray, i, j, &pixel);
		}
	}

	image->roi = NULL;

	xy_ratio = (float)((float)rect.width/(float)rect.height);

	sumx = new float[(int)((float)rect.width*0.8f)-(int)((float)rect.width*0.6f)+1];
	sumxx = 0.0f;

	for (j=rect.x + (int)((float)rect.width*0.6f); j<rect.x + (int)((float)rect.width*0.8f); j++)
	{
		sumx[j-rect.x] = 0.0f;

		for (i=rect.y; i<rect.y+rect.height; i++)
		{
			iplGetPixel(image, j, i, &pixel);
			sumx[j-rect.x] += pixel/255;
		}

		sumxx += sumx[j-rect.x];
	}

	sumxx /= ((float)((float)rect.width*0.8f)-(int)((float)rect.width*0.6f)+1.0f);

	finger_ratio = sumxx/(float)rect.height;

	delete [] sumx;
}

void iplHandSignal::iplTestHS()
{
	int i, j;
	int no_sample = 8;
	float angle;
	int fx, fy;
	float gray[16];
	float xyr, fr;
	char str[100];
	CvPoint p1, p2;

	FILE *fp;

	fp = fopen("D:\\HandOri001.dat", "awt");

	for (i=0; i<no_sample; i++)
	{
		//sprintf(str, "D:\\PROJECT_BSCL\\HS2002\\simage%d.tif", i+1);
		sprintf(str, "D:\\Works\\Project\\산자부\\HS2002\\o%d.jpg", i+1);
		IPLIMAGE file = cvvLoadImage(str);

		cvvConvertImage(file, procCPCimg[0], 0);
		cvvConvertImage(procCPCimg[0], procCPGimg[0], 0);

		iplSet(grabImage, 0);
		iplCopy(procCPGimg[0], grabImage);

		iplGetHSorientation(grabImage, angle, fx, fy, cvtImage);
		fprintf(fp, "%.2f %1d %1d ", angle*RAD2DEG, fx, fy);

		iplGetHSpose(cvtImage, gray, xyr, fr);
		fprintf(fp, "%.2f %.2f \n", xyr, fr);
	}

	fclose(fp);
}
