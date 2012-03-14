// ImageDoc.cpp : implementation of the CPicDoc class
//

#include "stdafx.h"
#include "Pic.h"
#include "PicDoc.h"
// image pixels and definitions
#include "Image.h"
#include "ImagePixel.h"
#include "cimage.h"
#include "PGMimage.h"
// image processing
#include "ImageAdjust.h"
#include "ImageFiltering.h"
#include "ImageGeometry.h"
#include "ImageTransform.h"
#include "OpticalFlow.h"
#include "FaceTool.h"
#include "IplFace.h"
//#include "iplHandSignal.h"

// auxilary functions
#include "ImgMisc.h"
#include "BPNN.h"
//#include "NNFunction.h"
#include "HLAC.h"
#include "moment.h"
#include "MyPrSheet.h"
#include "histogram.h"
#include "cv.h"
#include "SVV.h"
#include "malloc.h"
#include "highgui.h"
#include "myresource.h"

//forward definition for motion detector
LONG	summ_rect_arena32(LPBYTE data, int dx, int dy, int ptX, int ptY, int rx, int ry);
LONG	summ_rect_arena24(LPBYTE data, int dx, int dy, int ptX, int ptY, int rx, int ry);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CWait::CWait(CPicDoc *pDoc)
{
	m_pDoc = pDoc;
	m_pDoc->BeginWaitCursor();
	m_pDoc->m_Image.PrepareUndo();
	m_pDoc->SetModifiedFlag();
}

CWait::~CWait()
{
	m_pDoc->m_bRegionSelectMode = FALSE;
	m_pDoc->m_Tracker.m_rect = CRect(0,0,0,0);
	m_pDoc->UpdateAllViews(NULL);
	m_pDoc->EndWaitCursor();
}

/////////////////////////////////////////////////////////////////////////////
// CPicDoc

IMPLEMENT_DYNCREATE(CPicDoc, CDocument)

BEGIN_MESSAGE_MAP(CPicDoc, CDocument)
	//{{AFX_MSG_MAP(CPicDoc)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(IDM_BRIGHT, OnBright)
	ON_COMMAND(IDM_CONTRAST, OnContrast)
	ON_COMMAND(IDM_DARK, OnDark)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(IDM_CONTRASTDOWN, OnContrastdown)
	ON_COMMAND(IDM_TURNLEFT, OnTurnleft)
	ON_COMMAND(IDM_TURNRIGHT, OnTurnright)
	ON_COMMAND(IDM_TURNVERTICAL, OnTurnvertical)
	ON_COMMAND(IDM_TURNHORIZONTAL, OnTurnhorizontal)
	ON_COMMAND(IDM_REGIONSELECT, OnRegionselect)
	ON_UPDATE_COMMAND_UI(IDM_REGIONSELECT, OnUpdateRegionselect)
	ON_COMMAND(IDM_DUPLICATE, OnDuplicate)
	ON_COMMAND(IDM_INVERT, OnInvert)
	ON_COMMAND(ID_FACE_LFT, OnFaceLft)
	ON_COMMAND(ID_ADJUST_THRESHOLD, OnAdjustThreshold)
	ON_COMMAND(ID_FACE_LP, OnFaceLp)
	ON_COMMAND(ID_FILTER_HLAC, OnFilterHlac)
	ON_COMMAND(ID_ROTATE_AFFINE, OnRotateAffine)
	ON_COMMAND(ID_FACE_COLOR, OnFaceColor)
	ON_COMMAND(ID_FACE_RCER, OnFaceRcer)
	ON_COMMAND(ID_FILTERING_MORP, OnFilteringMorp)
	ON_COMMAND(ID_FILT_PROJ, OnFiltProj)
	ON_COMMAND(ID_MOMENT_FEATURE, OnMomentFeature)
	ON_COMMAND(ID_FACE_GABOR, OnFaceGabor)
	ON_COMMAND(ID_HOUGH, OnHough)
	ON_COMMAND(ID_FACE_FIND, OnFaceFind)
	ON_COMMAND(ID_FACE_RLP, OnFaceRlp)
	ON_COMMAND(ID_FACE_LPCENTER, OnFaceLpcenter)
	ON_COMMAND(ID_FACE_FL, OnFaceFl)
	ON_COMMAND(ID_FACE_COLOR_SPEC, OnFaceColorSpec)
	ON_COMMAND(ID_DYNAMIC_OPT, OnDynamicOpt)
	ON_COMMAND(ID_FACE_COMP, OnFaceComp)
	ON_COMMAND(ID_FACE_NN, OnFaceNn)
	ON_COMMAND(ID_FACE_STUDY, OnFaceStudy)
	ON_COMMAND(ID_DYNAMIC_EDGE, OnDynamicEdge)
	ON_COMMAND(ID_DYNAMIC_MOTION, OnDynamicMotion)
	ON_COMMAND(ID_DYNAMIC_BACKGND, OnDynamicBackgnd)
	ON_COMMAND(ID_DYNAMIC_FACE, OnDynamicFace)
	ON_COMMAND(ID_DYNAMIC_WHITE, OnDynamicWhite)
	ON_COMMAND(ID_DYNAMIC_VIEW, OnDynamicView)
	ON_COMMAND(ID_VIEW_BLOB, OnViewBlob)
	ON_COMMAND(ID_VIEW_WHITE, OnViewWhite)
	ON_COMMAND(ID_DYNAMIC_GETBACK, OnDynamicGetback)
	ON_COMMAND(ID_FILT_EDGE, OnFiltEdge)
	ON_COMMAND(ID_FILT_HISTO, OnFiltHisto)
	ON_COMMAND(ID_FACE_DTM, OnFaceDtm)
	ON_COMMAND(ID_FACE_CT21_4, OnFaceCt214)
	ON_COMMAND(ID_EDIT_JINWOO, OnEditJinwoo)
	ON_COMMAND(ID_HS_TEST, OnHsTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicDoc construction/destruction

CPicDoc::CPicDoc()
{
	m_bRegionSelectMode = FALSE;
	m_Tracker.m_nStyle = CRectTracker::dottedLine | CRectTracker::resizeOutside;
	m_Tracker.m_sizeMin = CSize(10,10);
	m_Tracker.m_rect = CRect(0,0,0,0);

	m_ProcessorMode=PROCESSOR_SIMPLE_VIEWER;
	m_bViewBlob = TRUE;
	m_bWhiteDetect = TRUE;
	m_Back = FALSE;

	dispCPimg = NULL;
	dispLPimg = NULL;
	dispBGimg = NULL;
	dispFCimg = NULL;
	backCPimg = NULL;
	dispRBGimg = NULL;

	CPsize = cvSize(CP_SIZE_X, CP_SIZE_Y);
	LPsize = cvSize(LP_SIZE_X, LP_SIZE_Y);

	lut.num = 257; lut.key = NULL; lut.value = NULL; lut.factor = NULL; lut.interpolateType = IPL_LUT_LOOKUP;
	plut = &lut;

	lut.key = (int *)malloc(sizeof(int)*257);
	lut.value = (int *)malloc(sizeof(int)*256);

	for (int i=0; i<=256; i++)
		lut.key[i] = i;

	AllocImage();
}

CPicDoc::~CPicDoc()
{
	DeallocImage();

	if (lut.value)	free(lut.value);
	if (lut.key)	free(lut.key);

	if (backCPimg)		cvReleaseImage(&backCPimg);
	if (dispFCimg)		cvReleaseImage(&dispFCimg);
	if (dispBGimg)		cvReleaseImage(&dispBGimg);	
	if (dispLPimg)		cvReleaseImage(&dispLPimg);
	//if (dispCPimg)		cvReleaseImage(&dispCPimg);

	if (fileimg)	cvReleaseImage(&fileimg);
}

/////////////////////////////////////////////////////////////////////////////
// CPicDoc diagnostics

#ifdef _DEBUG
void CPicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPicDoc commands

BOOL CPicDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString filetype;
	loadedfile = filetype = lpszPathName;
	filetype.MakeUpper();

	int n = loadedfile.Find(".");
	loadedfile.Delete(n, 4);
	loadedfile.Insert(n, ".dat");

	if(filetype.Find(".PGM") > -1)
	{
		PGMimage img;
		int k = 0;
		IMAGE *B = img.img_open((char *)lpszPathName);
		
		GrayImage A(B->cols,B->rows);
		for(int j=0;j<B->rows;j++)
			for(int i=0;i<B->cols;i++,k++)
				A.setPixel(i,j,B->data[k]);

		delete B;

		CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&A));
	}
			
	// 이미지 파일 읽기
	if ( !m_Image.Load(lpszPathName) ) return FALSE;
	
	int bitcount = m_Image.GetBitCount();
	if(!(bitcount == 2 || bitcount == 8 || bitcount == 24))
	{
		m_Image.Free();
		AfxMessageBox("이 프로그램은 트루 컬러와 그레이스케일 영상만 지원합니다.");
		return FALSE;
	}

	fileimg = cvvLoadImage(lpszPathName);
	if (!fileimg)	return FALSE;

	//cvvShowImage("Loaded Image", fileimg);
	//cvvResizeWindow("Loaded Image", fileimg->width, fileimg->height);
	//cvvResizeWindow("Result Image", fileimg->width, fileimg->height);

//	int slider_pos[1] = {0};
//	cvvCreateTrackbar( "Test Bar", "Loaded Image", &slider_pos[0], 255, NULL);
	face.iplFaceAllocImage(fileimg);

	return TRUE;
}


BOOL CPicDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// 이미지 저장하기
	if ( !m_Image.Save(lpszPathName) ) return FALSE;

	cvvSaveImage(lpszPathName, fileimg);

	return TRUE;
}


BOOL CPicDoc::OnNewDocument() 
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if( theApp.m_pNewImage )
	{
		m_Image = * theApp.m_pNewImage;
		theApp.m_pNewImage = NULL;
		m_Image.InitDIB();
	}
	
	int bitcount = m_Image.GetBitCount();
	if(!(bitcount == 8 || bitcount == 24))
	{
		m_Image.Free();
		AfxMessageBox("이 프로그램은 트루 컬러와 그레이스케일 영상만 지원합니다.");
		return FALSE;
	}

	return TRUE;
}


void CPicDoc::OnEditCopy() 
{
	CmyImage image;
	int x, y, cx, cy, i;

	// 영역 선택 모드인 경우 선택된 영역의 크기 계산
	if(m_bRegionSelectMode)
	{
		x = __max(m_Tracker.m_rect.left, 0);	
		y = __max(m_Tracker.m_rect.top, 0);	
		if(m_Tracker.m_rect.right < m_Image.GetWidth())
			cx = __max(m_Tracker.m_rect.right - x, 0);
		else
			cx = __max(m_Image.GetWidth() - x, 0);
		
		if(m_Tracker.m_rect.bottom< m_Image.GetHeight())
			cy = __max(m_Tracker.m_rect.bottom - y, 0);
		else
			cy = __max(m_Image.GetHeight() - y, 0);
	}
	// 영역 선택 모드가 아닌 경우에는 전체 이미지를 복사
	else
	{
		x = 0;	y = 0;
		cx = m_Image.GetWidth();
		cy = m_Image.GetHeight();
	}

	// 클립보드에 복사하기 위해 새로운 이미지 생성
	image.Create(cx, cy, m_Image.GetBitCount());
	image.InitDIB(FALSE);
	
	// 새로 생성된 이미지에 복사할 영역을 픽셀 단위로 복사
	if(m_Image.GetBitCount() == 8)
	{
		CPixelPtr SourcePtr(m_Image);
		CPixelPtr DestPtr(image);
		for(i = 0 ; i<cy ; i++)
			memcpy(DestPtr[i], SourcePtr[y+i]+x, cx);
	}
	else
	{
		CColorPixelPtr ColorSourcePtr(m_Image);
		CColorPixelPtr ColorDestPtr(image);
		for(i = 0 ; i<cy ; i++)
			memcpy(ColorDestPtr[i], ColorSourcePtr[y+i]+x, cx*3);
	}

	// 클립보드에 데이터 복사
	::OpenClipboard(NULL);
	::SetClipboardData(CF_DIB, ::CopyHandle(image.GetHandle()));
	::CloseClipboard();
}

void CPicDoc::OnEditUndo() 
{
	m_Image.Undo();
	m_bRegionSelectMode = FALSE;
	m_Tracker.m_rect = CRect(0,0,0,0);
	UpdateAllViews(NULL);
}

void CPicDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Image.CanUndo());
}

void CPicDoc::OnInvert() 
{
	CWait wait(this);		
	CImageAdjust image(m_Image);
	image.Invert();		// 반전
}

void CPicDoc::OnBright() 
{
	/*
	CWait wait(this);		
	CImageAdjust image(m_Image);
	image.Brightness(30);	// 밝게
	*/
	int i;
	char str[100];
	CvMemStorage *storage = cvCreateMemStorage();
	CvSeq *contMayor;

	IPLIMAGE procCimg[6];
	IPLIMAGE procCSimg[3];
	IPLIMAGE procGimg[6];

	for (i=0; i<6; i++)
	{
		procCimg[i] = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 3);
		procGimg[i] = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	}

	for (i=0; i<3; i++)
		procCSimg[i] = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);

	/*
	fileimg = cvvLoadImage("D:\\User\\DJKim\\Project\\산기평2001\\수신호DB\\image.bmp");
	cvvConvertImage(fileimg, procCimg[0], 0);
	cvvConvertImage(procCimg[0], procGimg[0], 0);
	*/

	for (i=12; i<43; i++)
	{
		if ((i == 12) || (i==42))
		{

		sprintf(str, "D:\\PROJECT_BSCL\\산기평2001\\수신호 DB\\image%d.tif", i);
		fileimg = cvvLoadImage(str);
		
		cvvConvertImage(fileimg, procCimg[1], 0);
		cvCvtPixToPlane(procCimg[1], procCSimg[0], procCSimg[1], procCSimg[2], NULL);
		//face.iplThreshold2(procCSimg[2], procGimg[4], 32, 64, 0);	// 18 - 20, 24
		face.iplThreshold2(procCSimg[2], procGimg[4], 8, 64, 0);	// 12, 42
		iplNot(procGimg[4], procGimg[4]);

		iplColorToGray(procCimg[1], procGimg[0]);
		//face.iplThreshold2(procGimg[0], procGimg[5], 32, 254, 0);
		face.iplThreshold2(procGimg[0], procGimg[5], 64, 220, 0);	// 18 - 20

		iplRGB2HLS(procCimg[1], procCimg[2]);
		cvCvtPixToPlane(procCimg[2], procCSimg[0], procCSimg[1], procCSimg[2], NULL);

		iplAnd(procCSimg[0], procGimg[4], procCSimg[1]);	// AND w/ no blue, 18 - 20

		// 1 - 50 : H(16,128,0)
		// 51 - 83 : H(64,254,3) S(64,254,2)
		if (i < 51)
			//face.iplThreshold2(procCSimg[0], procGimg[1], 16, 84, 0);
			face.iplThreshold2(procCSimg[1], procGimg[1], 16, 84, 0);	// 18 - 20
		else
			face.iplThreshold2(procCSimg[1], procGimg[1], 16, 48, 0);


		// 5. blob analysis (max area, 1 blob)
		CvSeq* chain = 0; 
		double area,max_area;
		int num_conts;

		// 5-1. find contours
		num_conts = cvFindContours(procGimg[1], storage, &chain, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		// 5-2. search all contours to find max. sized contours
		max_area=0;
		CvSeq *contour=NULL;
		for( CvSeq* c = chain; c != 0; c = c->h_next )
		{
			area = fabs(cvContourArea(c));
			if (area > max_area) { max_area=area; contour=c; }
		}

		contMayor=contour;
		
		iplSet(procGimg[2], 0);
		cvDrawContours(procGimg[2], contMayor, 255, 255, 0, CV_FILLED);
		iplAnd(procGimg[5], procGimg[2], procGimg[3]);	// AND w/ gray
		//iplAnd(procGimg[3], procGimg[0], procGimg[4]);
	
		cvvShowImage("Result Image", procGimg[3]);
		cvvWaitKey(0);

		cvvConvertImage(procGimg[3], fileimg, 0);
		sprintf(str, "D:\\PROJECT_BSCL\\산기평2001\\수신호 DB\\simage%d.tif", i);
		cvvSaveImage(str, fileimg);
		}
	}

	for (i=0; i<3; i++)
		cvReleaseImage(&procCSimg[i]);

	for (i=0; i<6; i++)
	{
		cvReleaseImage(&procGimg[i]);
		cvReleaseImage(&procCimg[i]);
	}

	cvReleaseMemStorage(&storage);
}

void CPicDoc::OnDark() 
{
	CWait wait(this);	
	CImageAdjust image(m_Image);
	image.Brightness(-30);	// 어둡게
}

void CPicDoc::OnContrast() 
{
	CWait wait(this);
	CImageAdjust image(m_Image);
	image.Contrast(30);		// 콘트라스트 증가
}

void CPicDoc::OnContrastdown() 
{
	CWait wait(this);
	CImageAdjust image(m_Image);
	image.Contrast(-30);	// 콘트라스트 감소
}

void CPicDoc::OnTurnleft() 
{
	CWait wait(this);
	CImageGeometry image(m_Image);
	image.TurnLeft();
}

void CPicDoc::OnTurnright() 
{
	CWait wait(this);	
	CImageGeometry image(m_Image);
	image.TurnRight();
}

void CPicDoc::OnTurnvertical() 
{
	CWait wait(this);	
	CImageGeometry image(m_Image);
	image.FlipVertical();
}

void CPicDoc::OnTurnhorizontal() 
{
	CWait wait(this);	
	CImageGeometry image(m_Image);
	image.FlipHorizontal();
}

void CPicDoc::OnRegionselect() 
{
	if(m_bRegionSelectMode) 
	{
		m_bRegionSelectMode = FALSE;
		UpdateAllViews(NULL);
	}
	else 
	{
		m_bRegionSelectMode = TRUE;
		m_Tracker.m_rect = CRect(0,0,0,0);
	}
}

void CPicDoc::OnUpdateRegionselect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bRegionSelectMode);
	
}

void CPicDoc::OnDuplicate() 
{
	OnEditCopy();
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_EDIT_PASTE);
}

void CPicDoc::OnFaceLft() 
{
	CWait wait(this);	
	CImageTransform image(m_Image);
	image.MyLFT();
}

void CPicDoc::OnAdjustThreshold() 
{
	IPLIMAGE procimg = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	IPLIMAGE procimg2 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);

	if (fileimg->nChannels != 1)
		cvvConvertImage(fileimg, procimg, 0);

	CMyPrSheet prop("Environment",NULL,0);

	if (prop.DoModal() == IDOK)
	{
		face.iplThreshold2(procimg, procimg2, (int)prop.m_thr.GetMinPos(), (int)prop.m_thr.GetMaxPos(), prop.m_thr.m_nRange, prop.m_thr.m_otsup);
		//cvThreshold(procimg, procimg2, (int)prop.m_thr.GetMinPos(), 255, CV_THRESH_BINARY);
		cvvShowImage("Result Image", procimg2);

#ifdef	_DEBUG_
		cvvConvertImage(procimg2, fileimg, 0);
#endif

	}

	cvReleaseImage(&procimg2);
	cvReleaseImage(&procimg);
}

void CPicDoc::OnFaceLp() 
{
	IPLIMAGE procimg = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 3);
	IPLIMAGE procimg2 = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_8U, 1);
	//CWait wait(this);	
	//CImageTransform image(m_Image);

	float value;
	int i;
	char str[100];
	FILE *fp;

	fp = fopen(".\\angleLP.txt", "wt");

	CMyPrSheet prop("Environment",NULL,4);

	if (prop.DoModal() == IDOK)
	{
		switch(prop.m_lp.m_nMethod)
		{
		case 0:
			face.iplLPM(procimg2, procLPGimg[0]);
			break;
		case 1:
			face.iplLPMbulge(procimg2, procLPGimg[0], &value);
			break;
		case 2:
			face.iplLPMangle(procimg2, procLPGimg[0], &value);

			for (i=0; i<13; i++)
			{
				sprintf(str, "D:\\User\\DJKim\\Research\\Figure\\sDFT\\angle%03d.bmp", i*15);
				fileimg = cvvLoadImage(str);

				cvvConvertImage(fileimg, procimg, 0);
				iplColorToGray(procimg, procimg2);

				face.iplLPMangle(procimg2, procLPGimg[0], &value);

				cvvResizeWindow("Result Image", procLPGimg[0]->width, procLPGimg[0]->height);
				cvvShowImage("Result Image", procLPGimg[0]);

				fprintf(fp, "%5.1f ", value);

				sprintf(str, "D:\\User\\DJKim\\Research\\Figure\\sDFT\\LPangle%03d.bmp", i*15);
				iplGrayToColor(procLPGimg[0], procLPCimg[0], 1, 1, 1);
				cvvSaveImage(str, procLPCimg[0]);
				cvvWaitKey(0);
			}
			break;
		default:
			break;
		}
	}

	fclose(fp);

	cvReleaseImage(&procimg2);
	cvReleaseImage(&procimg);
}

void CPicDoc::OnFilterHlac() 
{	
	CWait wait(this);
	CHLAC image;

	if (m_Image.GetBitCount() == 8)
		image.init(CImgMisc::ImgConvc_G(m_Image));
	else
		image.init(CImgMisc::ImgConvc_C(m_Image));
	
	CMyPrSheet prop("Environment",NULL,7);

	if (prop.DoModal() == IDOK)
		if (prop.m_HLAC.m_nFlag == 2)
			image.AutoHLAC(prop.m_HLAC.m_nLPu,prop.m_HLAC.m_nLPv,prop.m_HLAC.m_nXf,prop.m_HLAC.m_nYf,prop.m_HLAC.m_nNf,prop.m_HLAC.m_nNnf);
		else
			image.HLAC(prop.m_HLAC.m_nLPu,prop.m_HLAC.m_nLPv,prop.m_HLAC.m_nXf,prop.m_HLAC.m_nYf,prop.m_HLAC.m_nFlag);
		//image.HLAC(80,60,1,1,prop.m_HLAC.m_nFlag);
}

void CPicDoc::OnRotateAffine() 
{
	CWait wait(this);
	CImageGeometry image(m_Image);

	CMyPrSheet prop("Environment",NULL,3);
	if (prop.DoModal() == IDOK)
		image.Affine(prop.m_affine.m_fDeg,prop.m_affine.m_fZx,prop.m_affine.m_fZy,prop.m_affine.m_fPx,prop.m_affine.m_fPy);
}

void CPicDoc::OnFaceColor() 
{
	/*
	CWait wait(this);
	CImageTransform image(m_Image);
	int sub_flag = 0;

	CMyPrSheet prop("Environment",NULL,1);
	if (prop.DoModal() == IDOK)
	{
		image.MyColor(prop.m_conv.m_nCS,prop.m_conv.m_nAll,prop.m_conv.m_nR,prop.m_conv.m_nG,prop.m_conv.m_nB);
	}
	*/


}

void CPicDoc::OnFaceRcer() 
{
	CWait wait(this);
	CImageGeometry image(m_Image);

	//image.RCER();
	image.Histogram();
}

void CPicDoc::OnFilteringMorp() 
{
	CWait wait(this);
	CImageFiltering image(m_Image);

	CMyPrSheet prop("Environment",NULL,5);
	if (prop.DoModal() == IDOK)
		image.Morp(prop.m_morph.m_nMorph);
}

void CPicDoc::OnFiltProj() 
{
	IPLIMAGE procimg = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	IPLIMAGE procimg2 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);

	IPLIMAGE procimg3 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 3);
	IPLIMAGE procimg4 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 3);

	if (fileimg->nChannels != 1)
		cvvConvertImage(fileimg, procimg, 0);

	CMyPrSheet prop("Environment",NULL,11);
	
	if (prop.DoModal() == IDOK)
	{
		int xdir = 0, ydir = 0, method = 0;
		if (prop.m_proj.xdir)	xdir = 1;
		if (prop.m_proj.ydir)	ydir = 1;
		if (prop.m_proj.count)	method = 1;
		if (prop.m_proj.weight)	method = 2;

		face.ShowProjection(procimg, procimg3, xdir, ydir, method);
		//face.ShowProjection(procimg2, procimg3, 1, 1, 1);
		iplOr(fileimg, procimg3, procimg4);
		cvvShowImage("Result Image", procimg4);

#ifdef _DEBUG_
		cvvConvertImage(procimg4, fileimg, 0);
#endif
	}

	cvReleaseImage(&procimg4);
	cvReleaseImage(&procimg3);
	cvReleaseImage(&procimg2);
	cvReleaseImage(&procimg);
}

void CPicDoc::OnMomentFeature() 
{
	CWait wait(this);
	CImageGeometry image(m_Image);

	image.Feature();
}

void CPicDoc::OnFaceGabor() 
{
	CWait wait(this);
	CImageFiltering image(m_Image);

	CMyPrSheet prop("Environment",NULL,2);
	if (prop.DoModal() == IDOK)
		image.MyGabor(prop.m_gabor.m_dWave,prop.m_gabor.m_dTheta);
}

void CPicDoc::OnHough() 
{
	CWait wait(this);
	CImageGeometry image(m_Image);

	image.hough("80-100",254);
/*	CMyPrSheet prop("Environment",NULL,6);
	if (prop.DoModal() == IDOK)
	{
		char *str = new char[100];
		image.hough(prop.m_hough.m_strAng,prop.m_hough.m_nTh);
		delete []str;
	}*/
}

void CPicDoc::OnFaceFind() 
{
	ColorImage faceimage = CImgMisc::ImgConvc_C(m_Image);
	ColorImage E(faceimage);

	GrayImage *A;
	A = NULL;

	CMyPrSheet prop("Environment",NULL,9);
	CFaceTool face(&faceimage);
	int rindex,gindex;
	//CIplFace facetool;

	if (prop.DoModal() == IDOK)
	{
		switch(prop.m_face.m_nMethod)
		{
		case 0:
			face.getL1WithLips();
			break;
		case 1:
			face.FaceSegment();
			break;
		case 2:
			face.GroundTruth();
			break;
		case 3:
			face.faceLocate();
			break;
		case 4:
			face.getPDF();
			break;
		case 5:
			if (prop.m_face.m_nidx1 && prop.m_face.m_nidx2)
				rindex=0, gindex=1;
			else if (prop.m_face.m_nidx1 && prop.m_face.m_nidx3)
				rindex=0, gindex=2;
			else if (prop.m_face.m_nidx2 && prop.m_face.m_nidx3)
				rindex=1, gindex=2;

			face.PDFSegment(prop.m_face.m_dmr,prop.m_face.m_dmg,prop.m_face.m_dsrr,prop.m_face.m_dsgg,rindex,gindex,prop.m_face.m_dth);
			break;
		case 6:
			if (prop.m_face.m_nidx1 && prop.m_face.m_nidx2)
				rindex=0, gindex=1;
			else if (prop.m_face.m_nidx1 && prop.m_face.m_nidx3)
				rindex=0, gindex=2;
			else if (prop.m_face.m_nidx2 && prop.m_face.m_nidx3)
				rindex=1, gindex=2;

			face.PDFSegmentAnalysis(prop.m_face.m_dmr,prop.m_face.m_dmg,prop.m_face.m_dsrr,prop.m_face.m_dsgg,rindex,gindex);
			break;
		case 7:
			if (m_Image.GetBitCount() == 8)
			{
				face.getfG(CImgMisc::ImgConvc_G(m_Image), prop.m_face.m_dWave);
			}
			else
			{
				A = faceimage.getLumImg();
				face.getfG(*A, prop.m_face.m_dWave);
			}
			break;
		case 8:
			if (prop.m_face.m_nidx1 && prop.m_face.m_nidx2)
				rindex=0, gindex=1;
			else if (prop.m_face.m_nidx1 && prop.m_face.m_nidx3)
				rindex=0, gindex=2;
			else if (prop.m_face.m_nidx2 && prop.m_face.m_nidx3)
				rindex=1, gindex=2;

			face.LTSegmentAnalysis2(rindex,gindex);
			/*
			//facetool.iplLTanal(0,1, "rgb");
			facetool.iplLTanal(0,2, "HLS");
			facetool.iplLTanal(1,2, "I1I2I3");
			facetool.iplLTanal(1,2, "YCrCb");
			facetool.iplLTanal(1,2, "I1I2I3N");
			*/

			break;
		case 9:
			if (prop.m_face.m_nidx1 && prop.m_face.m_nidx2)
				rindex=0, gindex=1;
			else if (prop.m_face.m_nidx1 && prop.m_face.m_nidx3)
				rindex=0, gindex=2;
			else if (prop.m_face.m_nidx2 && prop.m_face.m_nidx3)
				rindex=1, gindex=2;

			face.faceSegment2(prop.m_conv.m_nCS,rindex,gindex,prop.m_face.m_dmr,prop.m_face.m_dsrr,prop.m_face.m_dmg,prop.m_face.m_dsgg);
			break;
		default:
			break;
		}
	}

	if (A != NULL)
		delete A;
}

void CPicDoc::OnFaceRlp() 
{
	CWait wait(this);	
	CImageTransform image(m_Image);

	CMyPrSheet prop("Environment",NULL,8);

	if (prop.DoModal() == IDOK)
		image.MyRLP(prop.m_rlp.m_nW, prop.m_rlp.m_nH);
}

void CPicDoc::OnFaceLpcenter() 
{
	CWait wait(this);
	CImageGeometry image(m_Image);

	image.LPCenter();
}

/*
void CPicDoc::OnNn() 
{
	BPNN nn;
	BPNN *nny = nn.bpnn_create(3,5,3);
	nn.bpnn_initialize();
	//nn.bpnn_feedforward(nny);

	//nny->hidden_weights[1][1] = 0.005;

	nn.bpnn_save(nny,"nn001.dat");
	
	nn.bpnn_free(nny);
}
*/

void CPicDoc::OnFaceFl() 
{
	/*
	// fuzzy rule base
	FuzzyRuleBase *hap = new FuzzyRuleBase;
	hap->load("simple.intention.frb");
	//hap->printFuzzyRuleBase();

	// make input membership
	float pas[1][2] = {{0.3f, 0.3f}};
	FuzzySet u1("L",0.0,1.0,(double)(1.0/256.0),FuzzySet::Triangular,pas[0],2);
	SbPList uList;
	uList.append(&u1);

	// inference using input mf. and given fuzzy rule base
	FuzzySet *result2 = hap->inference(uList);
	CString str;
	str.Format("\noutput = %5.3f\n", result2->defuzzifier(FuzzySet::CenterOfGravity));
	AfxMessageBox(str);

	delete hap;
	*/
}

void CPicDoc::OnFaceColorSpec() 
{
	ColorImage faceimage = CImgMisc::ImgConvc_C(m_Image);
	CFaceTool face(&faceimage);
	face.faceColor();
}

void CPicDoc::OnDynamicOpt() 
{
	// 20010610@23:51@DJ
	// 잘 모르겄다...
	OpticalFlow opt;
	
	GrayImage A(128,128), B(128,128);

	int i, j, k=0;
	double p = 0.5;
	double h1, h2;
	double dx = 2.0, dy = 0.0;

	for (j=0; j<128; j++)
		for (i=0; i<128; i++, k++)
		{
			h1 = 64 + (i - dx)/2;
			h2 = 128 + 64*sin(2*M_PI*3.5*(i - dx)/256 - M_PI/2);
			A.buf[0][k] = (1-p)*h1 + p*h2;
			h1 = 64 + (i)/2;
			h2 = 128 + 64*sin(2*M_PI*3.5*i/256 - M_PI/2);
			B.buf[0][k] = (1-p)*h1 + p*h2;
		}

		CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&A));
		CImgMisc::ShowImg(CImgMisc::ImgConvG_c(&B));
	
	opt.Horn( A, B, 3, 100, 1, 10);
}

// 20.3.02
void CPicDoc::OnFaceComp() 
{
	CFaceTool face;
	face.DJfaceComponent(CImgMisc::ImgConvc_G(m_Image));	
	//face.ShowFace(NULL, NULL);
}

void CPicDoc::OnFaceNn() 
{
	CFaceTool face;
	face.DJdegree_of_facial_expressions(CImgMisc::ImgConvc_G(m_Image), NULL);	
}

void CPicDoc::OnFaceStudy() 
{
	CMyPrSheet prop("Environment",NULL,10);
	/*
	CNNfunction nn;

	if (prop.DoModal() == IDOK)
	{
		nn.SetVar(prop.m_soft.m_Learn, prop.m_soft.m_Activation, prop.m_soft.m_train, prop.m_soft.m_rate, prop.m_soft.m_wgt);

		double in[160*2];
		double out[2*2];
		
		int i;

		for (i=0; i<4; i++)
		{
			out[i] = 0.0;
		}

		out[0] = 1.0;
		out[3] = 1.0;
		//out[8] = 1.0;

		FILE *fp;
		
		fp = fopen("c:\\matlabr11\\work\\feers\\gabor\\002gf.dat", "rt");

		for (i=0; i<160; i++)
		{
			fscanf(fp, "%f ", &in[i]);
		}
		
		fclose(fp);

		fp = fopen("c:\\matlabr11\\work\\feers\\gabor\\003gf.dat", "rt");

		for (i=0; i<160; i++)
		{
			i += 160;
			fscanf(fp, "%f ", &in[i]);
		}
		
		fclose(fp);

		int layers[3] = {160, 80, 2};
		
		nn.NN_learning(3, layers, 2, in, out);
	}
	*/
}

// 17.11.01
// 02.01.01
void	CPicDoc::ProcessImage()
{
	static BOOL bRunNow = FALSE;
	if(!dispCPimg || bRunNow)	return;
	bRunNow = TRUE;

	if (m_Back)
	{
		if (!backCPimg)
		{
			backCPimg = cvCreateImage(CPsize, IPL_DEPTH_8U, 1);
			iplColorToGray(dispCPimg, backCPimg);
		}
		else
			iplColorToGray(dispCPimg, backCPimg);

		OnDynamicGetback();
	}

	switch(m_ProcessorMode)
	{
		case PROCESSOR_SIMPLE_VIEWER:
			UpdateAllViews(NULL);
			break;
		case PROCESSOR_IMAGE_FILTER:
			break;
		case PROCESSOR_MOTION_DETECTOR:
			break;
		case PROCESSOR_WHITE_DETECTOR:
			if (WhiteDetector())
				UpdateAllViews(NULL);
			break;
		case PROCESSOR_FACE_DETECTOR:
			if(FaceDetector())
				UpdateAllViews(NULL);
			break;
		case PROCESSOR_BACKGROUND_EXCEPTION:
			break;
		default:	
			break;
	}
	bRunNow = FALSE;
}

/*
BOOL CPicDoc::ApplyFilter(LPBITMAPINFO lpBi)
{

		ASSERT(lpBi);

		int arena_dx = IMAGEWIDTH(lpBi);
		int arena_dy = IMAGEHEIGHT(lpBi);

		//
		// This is the simplest Edge detector
		// Pixel(x0,y0)	= (abs(Pixel(x0,y0)-Pixel(x+1,y+1)) + abs(Pixel(x+1,y0)-Pixel(x0,y+1)))/2
		//

		switch(IMAGEBITS(lpBi))
		{
		case 32:
			{
				LONG * ptr =(LONG *)IMAGEDATA(lpBi);
				LONG * tmpLine = new LONG[arena_dx];

				for(int y=0; y<arena_dy-1;y++)
				{

					for(int x=0; x<arena_dx-1;x++)
					{
						int x1y1=x+arena_dx+1;
						int x1y0 =x+1;
						int x0y1=x+arena_dx;
						LONG r_diag1 = 	abs(ptr[x]&0xff -ptr[x1y1]&0xff);
						LONG g_diag1 = 	abs((ptr[x]>>8)&0xff - (ptr[x1y1]>>8)&0xff);
						LONG b_diag1 = 	abs((ptr[x]>>16)&0xff -(ptr[x1y1]>>16)&0xff);

						LONG r_diag2 = 	abs(ptr[x1y0]&0xff -ptr[x0y1]&0xff);
						LONG g_diag2 = 	abs((ptr[x1y0]>>8)&0xff -(ptr[x0y1]>>8)&0xff);
						LONG b_diag2 = 	abs((ptr[x1y0]>>16)&0xff -(ptr[x0y1]>>16)&0xff);
						
						tmpLine[x] = ((r_diag1+r_diag2)>>1) |
									 (((g_diag1+g_diag2)>>1)<<8) |
									 (((b_diag1+b_diag2)>>1)<<16);


					}
					memcpy(ptr,tmpLine, (arena_dx-1)*sizeof(DWORD));
					ptr+=arena_dx;
				}
				
				delete tmpLine;
			}
			break;
		case 24:
			{
				BYTE * ptr =(BYTE *)IMAGEDATA(lpBi);
				BYTE * tmpLine = new BYTE[arena_dx*3];

				for(int y=0; y< arena_dy-1;y++)
				{

					for(int x=0; x<arena_dx-1; x++)
					{
						int x0y0 = x*3;
						int x1y1 = x0y0+arena_dx*3+3;
						int x1y0 = x0y0+3;
						int x0y1 = x0y0+arena_dx*3;
						LONG r_diag1 = 	abs( ptr[x0y0] - ptr[x1y1]);
						LONG g_diag1 = 	abs( ptr[x0y0+1] - ptr[x1y1+1]);
						LONG b_diag1 = 	abs( ptr[x0y0+2] - ptr[x1y1+2]);

						LONG r_diag2 = 	abs( ptr[x1y0] - ptr[x0y1]);
						LONG g_diag2 = 	abs( ptr[x1y0+1] - ptr[x0y1+1]);
						LONG b_diag2 = 	abs( ptr[x1y0+2] - ptr[x0y1+2]);
						
						tmpLine[x0y0]  = (BYTE)(r_diag1+r_diag2)>>1;
						tmpLine[x0y0+1]= (BYTE)(g_diag1+g_diag2)>>1;
						tmpLine[x0y0+2]= (BYTE)(b_diag1+b_diag2)>>1;


					}
					memcpy(ptr,tmpLine, (arena_dx-1)*3);
					ptr+=arena_dx*3;
				}
				
				delete tmpLine;
			}
		default: break;
		}
	return TRUE;
}

//Simplest Motion detector
BOOL CPicDoc::RunDetector(LPBITMAPINFO lpBi)
{

	//constants definitions for motion detector
#define	ZONESX	4
#define	ZONESY	4
#define	ZONES	ZONESX*ZONESY
#define	DETECTION_LEVEL 0.05f

	static LONG lastIntensity[ZONES];
	static BOOL init = TRUE;

	LONG newIntensity[ZONES];

	int rx = IMAGEWIDTH(lpBi)/ZONESX;
	int ry = IMAGEHEIGHT(lpBi)/ZONESY;

	for(int y = 0; y<ZONESY; y++)
		for(int x = 0; x<ZONESX; x++)
			if(IMAGEBITS(lpBi)==32)
			 newIntensity[y*ZONESX+x] = summ_rect_arena32(  IMAGEDATA(lpBi),
															IMAGEWIDTH(lpBi),
															IMAGEHEIGHT(lpBi),
															x*rx, y*ry, rx, ry);
			else
			if(IMAGEBITS(lpBi)==24)
			 newIntensity[y*ZONESX+x] = summ_rect_arena24(IMAGEDATA(lpBi),
															IMAGEWIDTH(lpBi),
															IMAGEHEIGHT(lpBi),
															x*rx, y*ry, rx, ry);
			else return FALSE;

	BOOL ret=FALSE;
	if(!init)
	{
		FLOAT lastRel[ZONES-1];
		FLOAT newRel[ZONES-1];
		
		for(int i=0; i<ZONES-1; i++)
			lastRel[i] = (float)lastIntensity[i]/(float)(lastIntensity[i+1]+1);

		for(i=0; i< ZONES-1; i++)
		{
			newRel[i]= (float)newIntensity[i]/(float)(newIntensity[i+1]+1);
			
			float alarm = (float)fabs(lastRel[i]-newRel[i])/newRel[i];

			if(alarm >DETECTION_LEVEL)
				{ret=TRUE; break;}
		}

	
	}
	memcpy(lastIntensity,newIntensity, ZONES*sizeof(LONG));
	init = FALSE;
	return ret;
}

//some detectors stuff
LONG	summ_rect_arena32(LPBYTE data, int dx, int dy, int ptX, int ptY, int rx, int ry)
{
	LONG summ = 0;
	int lineBytes = dx*4;

	data+= (lineBytes*ptY + ptX*4);//offset

	for(int y = 0; y<ry; y++, data+=lineBytes)
		for(int x = 0; x< rx*4; x+=4)
		{
			summ+= data[x+1];
			summ+= data[x+2];
			summ+= data[x+3];
		}
	summ/=3;
	return summ;
}

LONG	summ_rect_arena24(LPBYTE data, int dx, int dy, int ptX, int ptY, int rx, int ry)
{
	LONG summ = 0;
	int lineBytes = dx*3;

	data+= (lineBytes*ptY + ptX*3);//offset

	for(int y = 0; y<ry; y++, data+=lineBytes)
		for(int x = 0; x< rx*3; x+=3)
		{
			summ+= data[x+0];
			summ+= data[x+1];
			summ+= data[x+2];
		}
	summ/=3;
	return summ;
}
*/

void CPicDoc::OnDynamicEdge() 
{
	m_ProcessorMode=PROCESSOR_IMAGE_FILTER;
	
}

void CPicDoc::OnDynamicMotion() 
{
	m_ProcessorMode=PROCESSOR_MOTION_DETECTOR;
	
}

void CPicDoc::OnDynamicView() 
{
	m_ProcessorMode=PROCESSOR_SIMPLE_VIEWER;

}

void CPicDoc::OnDynamicBackgnd() 
{
	m_ProcessorMode=PROCESSOR_BACKGROUND_EXCEPTION;
	
}

void CPicDoc::OnDynamicFace() 
{
	m_ProcessorMode=PROCESSOR_FACE_DETECTOR;
	
}

void CPicDoc::OnDynamicWhite() 
{
	m_ProcessorMode=PROCESSOR_WHITE_DETECTOR;
	
}

// 17.11.01
// Detect White/Black Portion
// 02.01.02
BOOL CPicDoc::WhiteDetector()
{
	CvSize size;
	size.width = dispCPimg->width;
	size.height = dispCPimg->height;
	int th0 = 128;

	// image allocation	
	IplImage *imgA = cvCreateImage(size, IPL_DEPTH_8U, 3);
	IplImage *imgB = cvCreateImage(size, IPL_DEPTH_8U, 1);

	cvCopyImage(dispCPimg, imgA);
	// color2gray
	iplColorToGray(imgA, imgB);

	// histogram & otsu
	m_Myhist.addtohist((unsigned char*)imgB->imageData, size.width*size.height);
	m_Myhist.DJotsu(&th0);
	
	// white/black detect
	if (m_bWhiteDetect)
		cvThreshold(imgB, imgB, (float)th0, 255.0f, CV_THRESH_BINARY);
	else
		cvThreshold(imgB, imgB, (float)th0, 255.0f, CV_THRESH_BINARY_INV);

	// gray2color(pseudo)
	iplGrayToColor(imgB, imgA, 1.0, 1.0, 1.0);
	
	cvCopyImage(imgA, dispCPimg);

	// image deallocation
	cvReleaseImage(&imgB);
	cvReleaseImage(&imgA);
	
	return TRUE;
}

// const for color twist matrix
// 2^31 = (2147483648)
#define	CE21	(2147483648)

// 19.12.01
// 02.01.02
// 29.01.02
// IPL based programming
BOOL CPicDoc::FaceDetector()
{
//	double cx, cy, cr;
	CvPoint pt1, pt2;
	CvRect rect;
	CIplFace face;
	iplFaceStructure structure;

	clock_t start, finish;
	double duration;

	// 1. get raw image
	cvCopyImage(dispCPimg, procCPCimg[0]);
	face.iplFaceAllocImage(procCPCimg[0]);

	// 2. face location
	face.iplFaceLocate(procCPCimg[0], &structure, procCPGimg[0]);
	iplGrayToColor(procCPGimg[0], procCPCimg[1], 1.0, 1.0, 1.0);

	// 3. face component extraction
	if (structure.face.ValidRect())
	{
		face.iplFaceComponent(procCPCimg[0], &structure, procCPGimg[1]);
		iplGrayToColor(procCPGimg[1], procCPCimg[2], 1.0, 1.0, 1.0);
	}
	
	//iplGrayToColor(procCPGimg[1], procCPCimg[3], 1.0, 1.0, 1.0);
	//face.ShowProjection(procCPGimg[1], procCPCimg[4], 1, 1, 1);
	//iplOr(procCPCimg[3], procCPCimg[4], procCPCimg[2]);
		
	//iplColorToGray(procCPCimg[0], procCPGimg[1]);
	//face.ShowHistogram(procCPGimg[1], procCPCimg[5]);

	/*
	// 1-0. blur the image
	//iplBlur(procCPCimg[0], procCPCimg[0], 3, 3, 1, 1);

	// 1-1. get ROI from raw image
	if (backCPimg)
	{
		for (int img=0; img<NUM_BUFFER; img++)
			iplSet(procCPGimg[img], 0);

		cvCvtPixToPlane(procCPCimg[0], procCPGimg[0], procCPGimg[1], procCPGimg[2], NULL);
		
		iplSet(maskCPimg[0], 0);
		cvAbsDiff(procCPGimg[0], backCPimg, procCPGimg[3]);
		iplGreaterS(procCPGimg[3], 32, maskCPimg[0]);

		iplSet(procCPGimg[4], 255);
		procCPGimg[4]->maskROI = maskCPimg[0];
		iplCopy(procCPGimg[4], procCPGimg[5]);
		iplGrayToColor(procCPGimg[5], procCPCimg[3], 1.0, 1.0, 1.0);
		procCPGimg[4]->maskROI = NULL;
		
		procCPGimg[0]->maskROI = maskCPimg[0];
		procCPGimg[1]->maskROI = maskCPimg[0];
		procCPGimg[2]->maskROI = maskCPimg[0];

		iplCopy(procCPGimg[0], procCPGimg[3]);
		iplCopy(procCPGimg[1], procCPGimg[4]);
		iplCopy(procCPGimg[2], procCPGimg[5]);
		iplSet(procCPCimg[0], 0);
		cvCvtPlaneToPix(procCPGimg[3], procCPGimg[4], procCPGimg[5], NULL, procCPCimg[0]);
		
		procCPGimg[0]->maskROI = NULL;
		procCPGimg[1]->maskROI = NULL;
		procCPGimg[2]->maskROI = NULL;
	}
		
	// 2. RGB -> I1I2I3
	iplApplyColorTwist(procCPCimg[0], procCPCimg[1], mytwist, 0);

	// 3. get SUBimg from I1I2I3 img
	cvCvtPixToPlane(procCPCimg[1], procCPGimg[0], procCPGimg[1], procCPGimg[2], NULL);

	// 4. get centered/not-rotated facial image
	GetCenteredFace(procCPCimg[1], procCPGimg[2], &cx, &cy, &cr, &rect);
	//iplGrayToColor(procCPGimg[2], procCPCimg[4], 1.0, 1.0, 1.0);

	// 4-0. error management
	if ((cx - rect.width/2.0 < 0) || (cx + rect.width/2.0 > CPsize.width) 
		|| (cy - rect.height/2.0 < 0) || (cy + rect.height/2.0 > CPsize.height))
		return FALSE;

	// 4-1. set maskROI from above facial image
	iplGreaterS(procCPGimg[2], 0, maskCPimg[0]);

	// 5. get horizontal edge, bin., convert to color	
	iplFixedFilter(procCPGimg[2], procCPGimg[4], IPL_SOBEL_3x3_H);
	//cvThreshold(procCPGimg[4], procCPGimg[4], 128, 255, CV_THRESH_BINARY);
	iplGrayToColor(procCPGimg[4], procCPCimg[4], 1.0, 1.0, 1.0);
	
	// compute histogram w.r.t. LUT
	//iplComputeHisto(procCPGimg[3], &plut);
	//HistogramDemo(procCPGimg[1], procCPCimg[4]);
	//iplGrayToColor(procCPGimg[4], procCPCimg[4], 1.0, 1.0, 1.0);

	// 6. get bin. dark img, convert to color
	iplNot(procCPGimg[2], procCPGimg[3]);
	procCPGimg[3]->maskROI = maskCPimg[0];
	iplSet(procCPGimg[1], 0);
	iplCopy(procCPGimg[3], procCPGimg[1]);
	//cvThreshold(procCPGimg[1], procCPGimg[1], 200, 255, CV_THRESH_BINARY);
	procCPGimg[3]->maskROI = NULL;
	//iplGrayToColor(procCPGimg[1], procCPCimg[1], 1.0, 1.0, 1.0);

	// 7. get remap (LPM)
	iplRemap(procCPCimg[4], xmap, ymap, procLPCimg[0], IPL_INTER_LINEAR);

	// 8. a basic DFM approach.
	// Eexp = Eedge - Egray
	// Eedge = # of edge pixels
	// Egray = sum of gray pixels
	int DFMstep = (int)((double)((double)rect.height/(double)10.0));
	int i, j, k;
	double eval, gval;

	iplGrayToColor(procCPGimg[1], procCPCimg[1], 1.0, 1.0, 1.0);

	for (i=DFMstep; i<rect.height - DFMstep; i++)
	{
		iplSetROI(roi, 0, cx - rect.width/2.0, cy - rect.height/2.0 + i, rect.width, DFMstep);

		procCPGimg[4]->roi = roi;
		procCPGimg[1]->roi = roi;

		iplCopy(procCPGimg[4], procCPGimg[5]);
		eval = cvSumPixels(procCPGimg[5]);
		iplCopy(procCPGimg[1], procCPGimg[5]);
		gval = cvSumPixels(procCPGimg[5]);
		projCPY[i] = (float)(eval + gval);

		procCPGimg[1]->roi = NULL;		
		procCPGimg[4]->roi = NULL;		
	}

	float max = -1e5;
	for (i=DFMstep; i<rect.height/2.0; i++)
	{
		if (projCPY[i] > max)
		{
			max = projCPY[i];
			j = i;
		}
	}

	max = -1e5;
	for (i=rect.height - DFMstep; i>rect.height/2.0; i--)
	{
		if (projCPY[i] > max)
		{
			max = projCPY[i];
			k = i;
		}
	}

	pt1.x = cx - rect.width/2.0;
	pt1.y = j + cy - rect.height/2.0;
	pt2.x = cx + rect.width/2.0;
	pt2.y = pt1.y + DFMstep;
	cvRectangle(dispCPimg, pt1, pt2, (int)(0x00FF00), 1);

	pt1.y = k + cy - rect.height/2.0;
	pt2.y = pt1.y + DFMstep;
	cvRectangle(dispCPimg, pt1, pt2, (int)(0x00FF00), 1);
	*/
	
	///////////////////////////////////////////////////////////////////////////////////////
	// DISPLAY SECTION
	///////////////////////////////////////////////////////////////////////////////////////

	// show result of 4.
	/*
	pt1.x = cx-5;	pt1.y = cy;	pt2.x = cx+5;	pt2.y = cy;
	cvLine(dispCPimg, pt1, pt2, (int)(0xFF0000), 1);

	pt1.x = cx;	pt1.y = cy-5;	pt2.x = cx;	pt2.y = cy+5;
	cvLine(dispCPimg, pt1, pt2, (int)(0xFF0000), 1);
	*/

	rect = structure.face.rect;
	pt1.x = rect.x;	pt1.y = rect.y;	pt2.x = rect.x + rect.width;	pt2.y = rect.y + rect.height;
	cvRectangle(dispCPimg, pt1, pt2, (int)(0x0000FF), 1);

	// show result of 5.
	if (!dispBGimg)
	{
		dispBGimg = cvCreateImage(CPsize, IPL_DEPTH_8U, 3);
		cvCopyImage(procCPCimg[1], dispBGimg);
	}
	else
		cvCopyImage(procCPCimg[1], dispBGimg);

	// show result of 6.
	if (!dispFCimg)
	{
		dispFCimg = cvCreateImage(CPsize, IPL_DEPTH_8U, 3);
		cvCopyImage(procCPCimg[2], dispFCimg);
	}
	else
		cvCopyImage(procCPCimg[2], dispFCimg);

	/*
	// show result of 7.
	if (!dispLPimg)
	{
		dispLPimg = cvCreateImage(LPsize, IPL_DEPTH_8U, 3);
		cvCopyImage(procLPCimg[0], dispLPimg);
	}
	else
		cvCopyImage(procLPCimg[0], dispLPimg);

	// show result of ?.
	if (!dispRBGimg)
	{
		dispRBGimg = cvCreateImage(CPsize, IPL_DEPTH_8U, 3);
		cvCopyImage(procCPCimg[3], dispRBGimg);
	}
	else
		cvCopyImage(procCPCimg[3], dispRBGimg);
	*/

	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	face.iplFaceFreeImage();

	return TRUE;
}

/*
BOOL CPicDoc::BackGroundException(LPBITMAPINFO lpBi)
{
	return TRUE;
}
*/

void CPicDoc::OnViewBlob() 
{
	m_bViewBlob = !m_bViewBlob;
}

void CPicDoc::OnViewWhite() 
{
	m_bWhiteDetect = !m_bWhiteDetect;
}

void CPicDoc::AllocImage()
{
	int i,j;
	float x, y;

	int twistdata[16] = {
		0.33*CE21, 0.33*CE21, 0.33*CE21, 0, 
		-0.5*CE21, 0, 0.5*CE21, 0, 
		-0.25*CE21, 0.5*CE21, -0.25*CE21, 0,
		0, 0, 0, CE21};
	
	// space-variant vision
	// LP image
	CSVV svv(1);
	svv.LPtable(CPsize.width, CPsize.height,
		LPsize.width, LPsize.height);

	/*
	cvvNamedWindow("Loaded Image", 0);
	cvvNamedWindow("Histogram Image", 0);
	cvvResizeWindow("Histogram Image", 256, 128);
	cvvNamedWindow("Result Image", 0);
	*/

	// image allocation for processing
	for (i=0; i<NUM_BUFFER; i++) 
	{
		procCPCimg[i] = cvCreateImage(CPsize, IPL_DEPTH_8U, 3);
		procCPGimg[i] = cvCreateImage(CPsize, IPL_DEPTH_8U, 1);
		maskCPimg[i] = cvCreateImage(CPsize, IPL_DEPTH_1U, 1);
		procLPCimg[i] = cvCreateImage(LPsize, IPL_DEPTH_8U, 3);
		procLPGimg[i] = cvCreateImage(LPsize, IPL_DEPTH_8U, 1);
		procCPFimg[i] = cvCreateImage(CPsize, IPL_DEPTH_32F, 1);
		procLPFimg[i] = cvCreateImage(LPsize, IPL_DEPTH_32F, 1);
		procCPSFimg[i] = cvCreateImage(LPsize, IPL_DEPTH_16S, 1);
	}
	
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

	// color twist
	mytwist = iplCreateColorTwist(twistdata, 31);
	iplSetColorTwist(mytwist, twistdata, 31);

	//roi = iplCreateROI(0, 0, 0, 0, 0);
}

void CPicDoc::DeallocImage()
{
	face.iplFaceFreeImage();
	//iplDeleteROI(roi);

	// image deallocation
	iplDeleteColorTwist(mytwist);
	cvReleaseImage(&ymap);
	cvReleaseImage(&xmap);

	for (int i=0; i<NUM_BUFFER; i++)
	{
		cvReleaseImage(&(procCPSFimg[i]));
		cvReleaseImage(&(procLPFimg[i]));
		cvReleaseImage(&(procCPFimg[i]));
		cvReleaseImage(&(procLPGimg[i]));
		cvReleaseImage(&(procLPCimg[i]));
		cvReleaseImage(&(maskCPimg[i]));
		cvReleaseImage(&(procCPGimg[i]));
		cvReleaseImage(&(procCPCimg[i]));
	}

	cvvDestroyWindow("Result Image");
	cvvDestroyWindow("Histogram Image");
	cvvDestroyWindow("Loaded Image");
}

// 29.01.02
// get centered/non-rotated facial image
void CPicDoc::GetCenteredFace(IplImage *Simg, IplImage *Dimg, double *cx, double *cy, double *cr, CvRect *rect)
{
	// 1. get SUBimg from I1I2I3 img
	cvCvtPixToPlane(Simg, procCPGimg[0], procCPGimg[1], procCPGimg[2], NULL);

	// 2. bin. I2, bin. I3
	cvThreshold(procCPGimg[1], procCPGimg[3], 16, 255, CV_THRESH_BINARY);
	cvThreshold(procCPGimg[2], procCPGimg[4], 4, 255, CV_THRESH_BINARY_INV);

	// 3. get (I2 & I3)
	iplAnd(procCPGimg[3], procCPGimg[4], procCPGimg[5]);

	// 4. blob analysis
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq *contMayor;	// pointer to max. sized contour

	CvSeq* chain = 0; 
	double area,max_area;
	int num_conts;

	// 4-1. find contours
	num_conts = cvFindContours( procCPGimg[5], storage, &chain, sizeof(CvContour),
		CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE );
	if (chain==0) { cvReleaseMemStorage(&storage); return; }

	// 4-2. search all contours to find max. sized contours
	max_area=0;
	CvSeq *contour=NULL;
	for( CvSeq* c = chain; c != 0; c = c->h_next )
	{
		area = fabs(cvContourArea(c));
		if (area > max_area) { max_area=area; contour=c; }
	}
	if (contour==NULL) { cvReleaseMemStorage(&storage); return; }

	contMayor=contour;
	*rect = (CvRect)cvContourBoundingRect(contMayor, 1);

	// 4-3. ellipse fitting
	int count = contMayor->total;
	CvBox2D myBox;
	CvPoint2D32f *PointArray32f = (CvPoint2D32f *)malloc(count * sizeof(CvPoint2D32f));
	CvPoint *PointArray = (CvPoint *)malloc(count * sizeof(CvPoint));
	int *hull = (int *) malloc(count * sizeof(int));
	int hullsize;
	
	cvCvtSeqToArray(contMayor, PointArray, CV_WHOLE_SEQ);
	cvConvexHull(PointArray, count, NULL, CV_COUNTER_CLOCKWISE, hull, &hullsize);

	for (int i=0; i<hullsize/*-1*/; i++)
	{ 
		PointArray32f[i].x = float( PointArray[ /*!!!*/ hull[i]].x);
		PointArray32f[i].y = float( PointArray[ /*!!!*/ hull[i]].y);
	}

	cvFitEllipse(PointArray32f, hullsize, &myBox);

	iplSet(procCPGimg[5], 0);
	cvDrawContours(procCPGimg[5], contMayor, 255, 0, 0, CV_FILLED);
	iplSet(procCPGimg[3], 0);
	iplAnd(procCPGimg[5], procCPGimg[0], procCPGimg[3]);

	free(PointArray);
	free(PointArray32f);

	cvReleaseMemStorage(&storage);
	
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

	// 6-3. warping(trans.), rotate
	const double coeffs[2][3] = {{1, 0, 80-*cx}, {0, 1, 60-*cy}};
	iplSet(Dimg, 0);
	iplWarpAffine(procCPGimg[3], Dimg, coeffs, IPL_INTER_LINEAR);
	//iplSet(procCPGimg[4], 0);	iplWarpAffine(procCPGimg[3], procCPGimg[4], coeffs, IPL_INTER_LINEAR);
	//iplSet(Dimg, 0);	iplRotateCenter(procCPGimg[4], Dimg, (*cr)-90.0, 80, 60, IPL_INTER_LINEAR);

}

void CPicDoc::OnDynamicGetback() 
{
	m_Back = !m_Back;
}

/* dummy code

	cvDrawContours(procCPGimg[5], contMayor, 255, 0, 0, CV_FILLED);

	// get moments from max. blob
	IplMomentState mState;
	iplMoments(procCPGimg[5], mState);

	// get cx, cy, cr(rad) from moments
	*cx = (double)(iplGetSpatialMoment(mState, 1, 0)/iplGetSpatialMoment(mState, 0, 0));
	*cy = (double)(iplGetSpatialMoment(mState, 0, 1)/iplGetSpatialMoment(mState, 0, 0));
	*cr = (double)((double)180.0/(double)M_PI)*(double)(0.5*atan(2*iplGetCentralMoment(mState, 1, 1)/(iplGetCentralMoment(mState, 2, 0) - iplGetCentralMoment(mState, 0, 1))));

*/

void CPicDoc::OnFiltEdge() 
{
	IPLIMAGE procimg = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	IPLIMAGE procimg2 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	IPLIMAGE procimg3 = cvCreateImage(cvSize(fileimg->width, fileimg->height), IPL_DEPTH_16S, 1);

	if (fileimg->nChannels != 1)
		cvvConvertImage(fileimg, procimg, 0);

	CMyPrSheet prop("Environment",NULL,12);

	if (prop.DoModal() == IDOK)
	{
		switch(prop.m_edge.m_edge)
		{
		case 0:
			iplFixedFilter(procimg, procimg2, IPL_PREWITT_3x3_H);
			break;
		case 1:
			iplFixedFilter(procimg, procimg2, IPL_PREWITT_3x3_V);
			break;
		case 2:
			cvSobel(procimg, procimg3, prop.m_edge.m_p1, prop.m_edge.m_p2, prop.m_edge.m_Ksize);
			cvvConvertImage(procimg3, procimg2, 0);
			break;
		case 3:
			cvLaplace(procimg, procimg3, prop.m_edge.m_Ksize);
			cvvConvertImage(procimg3, procimg2, 0);
			break;
		case 4:
			cvCanny(procimg, procimg2, prop.m_edge.m_p1, prop.m_edge.m_p2, prop.m_edge.m_Ksize);
			break;
		case 5:
			iplFixedFilter(procimg, procimg2, IPL_SOBEL_3x3_H);
			break;
		case 6:
			iplFixedFilter(procimg, procimg2, IPL_SOBEL_3x3_V);
			break;
		case 7:
			iplFixedFilter(procimg, procimg2, IPL_LAPLACIAN_3x3);
			break;
		case 8:
			iplFixedFilter(procimg, procimg2, IPL_LAPLACIAN_5x5);
			break;
		case 9:
			iplFixedFilter(procimg, procimg2, IPL_GAUSSIAN_3x3);
			break;
		case 10:
			//iplFixedFilter(procimg, procimg2, IPL_GAUSSIAN_5x5);
			break;
		case 11:
			iplFixedFilter(procimg, procimg2, IPL_HIPASS_3x3);
			break;
		case 12:
			iplFixedFilter(procimg, procimg2, IPL_HIPASS_5x5);
			break;
		case 13:
			iplFixedFilter(procimg, procimg2, IPL_SHARPEN_3x3);
			break;
		default:
			break;
		}

		cvRectangle(procimg2, cvPoint(0,0), cvPoint(fileimg->width-1, fileimg->height-1), (int)(000000), 1);

		cvvShowImage("Result Image", procimg2);

#ifdef _DEBUG_
		cvvConvertImage(procimg2, fileimg, 0);
#endif
	}

	cvReleaseImage(&procimg3);
	cvReleaseImage(&procimg2);
	cvReleaseImage(&procimg);	
}

void CPicDoc::OnFiltHisto() 
{
	IPLIMAGE procimg = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	IPLIMAGE procimg2 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 3);
	IPLIMAGE procimg3 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 3);

	CMyPrSheet prop("Environment",NULL,13);

	if (prop.DoModal() == IDOK)
	{
		if (fileimg->nChannels != 1 && prop.m_hist.m_method == 0)
		{
			cvvConvertImage(fileimg, procimg, 0);
			face.ShowHistogram(procimg, procimg2, 1, 0);
			cvvShowImage("Histogram Image", procimg2);
		}
		else
		{
			cvvConvertImage(fileimg, procimg3, 0);
			face.ShowHistogram(procimg3, procimg2, 1, 1);
			cvvShowImage("Histogram Image", procimg2);
		}

#ifdef _DEBUG_
		cvvConvertImage(procimg2, fileimg, 0);
#endif
	}

	cvReleaseImage(&procimg3);
	cvReleaseImage(&procimg2);
	cvReleaseImage(&procimg);
}

void CPicDoc::OnFaceDtm() 
{
	IPLIMAGE procimg = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 3);
	IPLIMAGE procimg2 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);
	IPLIMAGE procimg3 = cvCreateImage(cvSize(fileimg->width, fileimg->height), fileimg->depth, 1);

	cvvConvertImage(fileimg, procimg, 0);
	iplColorToGray(procimg, procimg2);

	face.iplFaceDTM(procimg2, procimg3, NULL);

	cvvShowImage("Result Image", procimg3);

	cvReleaseImage(&procimg3);
	cvReleaseImage(&procimg2);
	cvReleaseImage(&procimg);
}

void CPicDoc::OnFaceCt214() 
{
	//CIplFace face;
	face.ShowFace(NULL, NULL);
}

void CPicDoc::OnEditJinwoo() 
{
	IPLIMAGE procimg2 = cvCreateImage(cvSize(42, 60), IPL_DEPTH_8U, 1);
	IPLIMAGE procimg3 = cvCreateImage(cvSize(42, 60), IPL_DEPTH_8U, 1);

	iplSet(procimg3, 0.0);

	FILE *fp;
	fp = fopen(".\\_preprocessed.txt", "rt");
	int i, j;
	unsigned char pixel;

	for (j=0; j<60; j++)
	{
		for (i=0; i<42; i++)
		{
			fscanf(fp, "%d ", &pixel);
			iplPutPixel(procimg2, i, j, &pixel);
		}
	}

	//cvvShowImage("Result Image", procimg2);

	CvSeq* chain = 0; 
	double area,max_area;
	int num_conts;
	CvMemStorage *storage;
	//CvSeq *contMayor;

	storage = cvCreateMemStorage();

	// 1. find contours
	num_conts = cvFindContours(procimg2, storage, &chain, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//if (chain==0) { cvReleaseMemStorage(&storage); return; }

	// 2. search all contours to find max. sized contours
	max_area=0;
	CvSeq *contour=NULL;
	int count = 0;
	for( CvSeq* c = chain; c != 0; c = c->h_next )
	{
		count++;

		//cvDrawContours(procimg3, c, 255, 255, 0, CV_FILLED);
		cvDrawContours(procimg3, c, (255/num_conts)*count, (255/num_conts)*count, 0, CV_FILLED);

		//area = fabs(cvContourArea(c));
		//if (area > max_area) { max_area=area; contour=c; }
	}
	//if (contour==NULL) { cvReleaseMemStorage(&storage); return; }

	//contMayor=contour;
	//cvDrawContours(procimg3, contMayor, 255, 255, 0, CV_FILLED);

	cvvShowImage("Result Image", procimg3);

	cvReleaseMemStorage(&storage);

	fclose(fp);

	cvReleaseImage(&procimg3);
	cvReleaseImage(&procimg2);
}

void CPicDoc::OnHsTest() 
{
	/*
	iplHandSignal hand_signal;

	hand_signal.iplTestHS();	
	*/
}
