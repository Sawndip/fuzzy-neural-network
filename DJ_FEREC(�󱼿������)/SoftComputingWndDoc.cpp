// SoftComputingWndDoc.cpp : implementation of the CSoftComputingWndDoc class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"

#include "SoftComputingWndDoc.h"
#include "MainFrm.h"
#include "ImageBaseView.h"
#include "MyCamView.h"
#include <highgui.h>
#include "afx.h"
#include "time.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc

IMPLEMENT_DYNCREATE(CSoftComputingWndDoc, CDocument)

BEGIN_MESSAGE_MAP(CSoftComputingWndDoc, CDocument)
	//{{AFX_MSG_MAP(CSoftComputingWndDoc)
	ON_COMMAND(ID_SHOW_IMG, OnShowImg)
	ON_COMMAND(ID_CAM_CLOSE, OnCamClose)
	ON_COMMAND(ID_CAM_FORMAT, OnCamFormat)
	ON_COMMAND(ID_CAM_INIT, OnCamInit)
	ON_COMMAND(ID_CAM_SETTING, OnCamSetting)
	ON_COMMAND(ID_EDIT_ETC, OnEditEtc)
	ON_COMMAND(ID_GRAB_ANGRY, OnGrabAngry)
	ON_COMMAND(ID_GRAB_DISGUST, OnGrabDisgust)
	ON_COMMAND(ID_GRAB_FEAR, OnGrabFear)
	ON_COMMAND(ID_GRAB_HAPPY, OnGrabHappy)
	ON_COMMAND(ID_GRAB_NEUTRAL, OnGrabNeutral)
	ON_COMMAND(ID_GRAB_SAD, OnGrabSad)
	ON_COMMAND(ID_GRAB_SURPRISE, OnGrabSurprise)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc construction/destruction

CSoftComputingWndDoc::CSoftComputingWndDoc()
{
}

CSoftComputingWndDoc::~CSoftComputingWndDoc()
{
}

BOOL CSoftComputingWndDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc serialization

void CSoftComputingWndDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc diagnostics

#ifdef _DEBUG
void CSoftComputingWndDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSoftComputingWndDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc commands

void CSoftComputingWndDoc::OnShowImg() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CImageBaseView* imageview = mainframe->GetImageBaseView();
	imageview->SetImagePlane(4, 2);

	imageview->AddImage(2, 0, m_image);
	imageview->AddImage(3, 1, m_image);

	imageview->ShowImage(TRUE);
}

BOOL CSoftComputingWndDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	CString filetype = lpszPathName;
	filename = filetype;

	if ((filetype.Find(".jpg") > -1) 
		|| (filetype.Find(".gif") > -1)
		|| (filetype.Find(".bmp") > -1)
		|| (filetype.Find(".tif") > -1))
	{
		m_image = cvvLoadImage((char*)lpszPathName);
	}

	return TRUE;
}

BOOL CSoftComputingWndDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	CString filetype = lpszPathName;

	if ((filetype.Find(".jpg") > -1) 
		|| (filetype.Find(".gif") > -1)
		|| (filetype.Find(".bmp") > -1)
		|| (filetype.Find(".tif") > -1))
	{
		cvvSaveImage(lpszPathName, m_image);
	}	

	return TRUE;
}

void CSoftComputingWndDoc::OnCloseDocument() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();

	if ((camview->Camera()).IsRunning())
		camview->CameraOff();

	CDocument::OnCloseDocument();
}



void CSoftComputingWndDoc::OnCamClose() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();
	
	if ((camview->Camera()).IsRunning())
		camview->CameraOff();
}

void CSoftComputingWndDoc::OnCamFormat() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();
	
	camview->CameraFormat();
}

void CSoftComputingWndDoc::OnCamInit() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();
	CImageBaseView* imgview = mainframe->GetImageBaseView();

	if (!(camview->Camera()).IsRunning())
		camview->CameraOn();

	IPLIMAGE image;
	image = cvvLoadImage(".\\example.jpg");
	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}



void CSoftComputingWndDoc::OnCamSetting() 
{
	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = mainframe->GetMyCamView();
	
	camview->CameraSetting();
}

// 20030211
void CSoftComputingWndDoc::OnEditEtc() 
{
	int i, j, val;
	CString filename;

	/*
	IPLIMAGE TimageC = cvvLoadImage(".\\CHATanal\\TestImage3.jpg");
	IPLIMAGE TimageG = cvCreateImage(cvSize(960,720), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageC = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 3);
	IPLIMAGE SimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);

	cvvConvertImage(TimageC, TimageG, 0);

	k = 0;
	for (i=0; i<6; i++)
		for (j=0; j<6; j++)
		{
			cvSetImageROI(TimageG, cvRect(160*i,120*j,160,120));
			iplCopy(TimageG, SimageG);
			cvvConvertImage(SimageG, SimageC, 0);
			filename.Format(".\\CHATanal\\CHATimgG%03d.jpg", k);
			cvvSaveImage(filename, SimageC);

			k += 10;
		}

	cvReleaseImage(&TimageG);
	cvReleaseImage(&SimageG);
	cvReleaseImage(&SimageC);	
	*/

	#define	CE21	(2147483648)

	FILE *fp, *fp2, *fp3, *fp4, *fp5, *fp6;
	fp = fopen(".\\CHATperformance.dat", "wt");
	fp2 = fopen(".\\CHAT_I2.dat", "wt");
	fp3 = fopen(".\\CHAT_I3.dat", "wt");
	fp4 = fopen(".\\CHAT_H.dat", "wt");
	fp5 = fopen(".\\CHAT_L.dat", "wt");
	fp6 = fopen(".\\CHAT_S.dat", "wt");

	float sHit, seHit, nseHit, nsHit, tPix, sPix, nPix;
	float C, S, SE, NSE;
	int bins = 256;
	int dims1D[1] = {bins};
	float *hist_th = new float[2];
	hist_th[0] = 0.0f; hist_th[1] = 255.0f;
	CvHistogram* histG = new CvHistogram;
	histG = cvCreateHist(1, dims1D, CV_HIST_ARRAY);
	cvSetHistThresh(histG, &hist_th, 1);
	delete [] hist_th;

	int twistdata[16] = {
		0.33*CE21, 0.33*CE21, 0.33*CE21, 0, 
		-0.5*CE21, 0, 0.5*CE21, 0, 
		-0.25*CE21, 0.5*CE21, -0.25*CE21, 0,
		0, 0, 0, CE21};

	IplColorTwist *mytwist = iplCreateColorTwist(twistdata, 31);
	iplSetColorTwist(mytwist, twistdata, 31);

	tPix = 160*120;

	IPLIMAGE SimageCT = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 3);
	IPLIMAGE SimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE GimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE TimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE TimageG = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE GimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE NSimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE NGimageB = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);

	IPLIMAGE SimageC1 = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageC2 = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);
	IPLIMAGE SimageC3 = cvCreateImage(cvSize(160,120), IPL_DEPTH_8U, 1);

	for (i=0; i<59; i++)
	{		
		filename.Format(".\\CHATanal\\DJface%03d.bmp", i*10);
		IPLIMAGE SimageC = cvvLoadImage(filename);
		iplSet(SimageG, 0);
		cvvConvertImage(SimageC, SimageG);
		iplSet(SimageB, 0);
		iplThreshold(SimageG, SimageB, 1);
		iplSet(NSimageB, 0);
		iplNot(SimageB, NSimageB);

		iplRGB2HLS(SimageC, SimageCT);
		cvCvtPixToPlane(SimageCT, SimageC1, SimageC2, SimageC3, 0);

		cvCalcHist(&SimageC1, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp4, "%d ", (int)val);
		}
		fprintf(fp4, "\n");

		cvCalcHist(&SimageC2, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp5, "%d ", (int)val);
		}
		fprintf(fp5, "\n");
		
		cvCalcHist(&SimageC3, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp6, "%d ", (int)val);
		}
		fprintf(fp6, "\n");


		iplApplyColorTwist(SimageC, SimageCT, mytwist, 0);
		cvCvtPixToPlane(SimageCT, SimageC1, SimageC2, SimageC3, 0);

		cvCalcHist(&SimageC2, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp2, "%d ", (int)val);
		}
		fprintf(fp2, "\n");
		
		cvCalcHist(&SimageC3, histG);
		for (j = 1; j < bins; j++)
		{
			val = cvRound(cvQueryHistValue_1D( histG, j ));
			fprintf(fp3, "%d ", (int)val);
		}
		fprintf(fp3, "\n");

		/*
				for (i = 1; i < bins; i++) {
			val = cvRound(cvQueryHistValue_1D( histG, i ));
			if (val > MaxVal)
				MaxVal = val;
		}
*/
		
		filename.Format(".\\CHATanal\\CHATimgG%03d.jpg", i*10);
		IPLIMAGE GimageC = cvvLoadImage(filename);
		iplSet(GimageG, 0);
		cvvConvertImage(GimageC, GimageG);
		iplSet(GimageB, 0);
		iplThreshold(GimageG, GimageB, 1);
		iplSet(NGimageB, 0);
		iplNot(GimageB, NGimageB);

		sPix = (float)cvCountNonZero(GimageG);
		nPix = tPix - sPix;

		iplSet(TimageB, 0);
		iplAnd(SimageB, GimageB, TimageB);
		sHit = (float)cvCountNonZero(TimageB);

		iplSet(TimageB, 0);
		iplAnd(NSimageB, GimageB, TimageB);
		seHit = (float)cvCountNonZero(TimageB);

		iplSet(TimageB, 0);
		iplAnd(SimageB, NGimageB, TimageB);
		nseHit = (float)cvCountNonZero(TimageB);

		iplSet(TimageB, 0);
		iplAnd(NSimageB, NGimageB, TimageB);
		nsHit = (float)cvCountNonZero(TimageB);

		C = (sHit+nsHit)/tPix;
		SE = seHit/tPix;
		NSE = nseHit/tPix;
		S = sHit/sPix;

		fprintf(fp, "%.2f %.2f %.2f %.2f %.0f %.0f %.0f %.0f %.0f \n", C, S, SE, NSE, sPix, sHit, seHit, nseHit, nsHit);
	}

	cvReleaseImage(&SimageC3);
	cvReleaseImage(&SimageC2);	
	cvReleaseImage(&SimageC1);

	cvReleaseImage(&NGimageB);
	cvReleaseImage(&NSimageB);	
	cvReleaseImage(&GimageB);
	cvReleaseImage(&SimageB);	
	cvReleaseImage(&TimageG);
	cvReleaseImage(&TimageB);
	cvReleaseImage(&GimageG);
	cvReleaseImage(&SimageG);	
	cvReleaseImage(&SimageCT);	

	iplDeleteColorTwist(mytwist);

	cvReleaseHist(&histG);

	fclose(fp6);
	fclose(fp5);
	fclose(fp4);
	fclose(fp3);
	fclose(fp2);
	fclose(fp);
}


void CSoftComputingWndDoc::OnGrabAngry() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(3);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_angry.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::OnGrabDisgust() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(5);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_disgust.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::OnGrabFear() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(2);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_fear.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::OnGrabHappy() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(0);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_happy.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::OnGrabNeutral() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(6);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_neutral.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::OnGrabSad() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(1);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_sad.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::OnGrabSurprise() 
{
	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(4);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_surprise.jpg", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
}

void CSoftComputingWndDoc::GrabImage(int fe)
{
	switch(fe)
	{
		case '1':
			OnGrabHappy();
			break;
		case '2':
			OnGrabSad();
			break;
		case '3':
			OnGrabFear();
			break;
		case '4':
			OnGrabAngry();
			break;
		case '5':
			OnGrabSurprise();
			break;
		case '6':
			OnGrabDisgust();
			break;
		case '7':
			OnGrabNeutral();
			break;
		default:
			break;
	}
}

void CSoftComputingWndDoc::SetID(CString id)
{
	subject_id = id;
	// 20031231
	// 여기서 "K:\\" 부분을 원하는 폴더로 만들어 주면 된다.
	filepath.Format("D:\\FEDB\\%s", subject_id);

	::CreateDirectory(filepath, NULL);
	::SetCurrentDirectory(filepath);
}
