// SoftComputingWndDoc.cpp : implementation of the CSoftComputingWndDoc class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"

#include "SoftComputingWndDoc.h"

#include "ImageBaseView.h"
#include "MainFrm.h"

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
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_VIEW_FOOT, OnViewFoot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftComputingWndDoc construction/destruction

CSoftComputingWndDoc::CSoftComputingWndDoc()
{
	// TODO: add one-time construction code here

}

CSoftComputingWndDoc::~CSoftComputingWndDoc()
{
	m_frtk.IVSTKend();
	m_frtk1.IVSTKend();
}

BOOL CSoftComputingWndDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

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
/*
BOOL CSoftComputingWndDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	CString filetype = lpszPathName;

	if ((filetype.Find(".jpg") > -1) 
		|| (filetype.Find(".JPG") > -1) 
		|| (filetype.Find(".gif") > -1)
		|| (filetype.Find(".GIF") > -1)
		|| (filetype.Find(".bmp") > -1)
		|| (filetype.Find(".BMP") > -1)
		|| (filetype.Find(".tif") > -1)
		|| (filetype.Find(".TIF") > -1))
	{
		m_image = cvvLoadImage(lpszPathName);

		CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
		m_frtk.imgview = (CImageBaseView*)mainframe->GetImageBaseView();

		for (int i=0; i<4; i++) m_frtk.debug[i] = (CImageBaseView*)mainframe->GetDebugImage(i);

		m_frtk.msgview = (CLogView*)mainframe->GetDJMessage();
		m_frtk.IVSTKinit(m_image);
	}

	return TRUE;
}

BOOL CSoftComputingWndDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	IPLIMAGE image = cvCreateImage(cvSize(m_image->width, m_image->height), IPL_DEPTH_8U, 3);
	iplCopy(m_image, image);

	cvvSaveImage(lpszPathName, image);	
	
	return CDocument::OnSaveDocument(lpszPathName);
}
*/
void CSoftComputingWndDoc::OnFileOpen() 
{
	CFileDialog *pDlg = new CFileDialog( TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, 
		"입력영상 *.jpg;*.gif;*.bmp;*.tif|*.jpg;*.gif;*.bmp;*.tif|모든파일 *.*|*.*|", NULL );

	TCHAR* m_pszFile = new TCHAR[2048];
	m_pszFile[0] = '\0';
	pDlg->m_ofn.lpstrFile = m_pszFile;
	pDlg->m_ofn.nMaxFile = 2048;

	if (pDlg->DoModal())
	{
		POSITION pos = pDlg->GetStartPosition();
		
		CString filename = pDlg->GetNextPathName(pos);
		CString filename1 = pDlg->GetNextPathName(pos);

		if ((filename != "") && (filename1 != ""))
		{
			m_image = cvvLoadImage(filename);
			m_image1 = cvvLoadImage(filename1);

			CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
			m_frtk.imgview = (CImageBaseView*)mainframe->GetImageBaseView(0);
			m_frtk1.imgview = (CImageBaseView*)mainframe->GetImageBaseView(1);

			m_frtk.imgview->SetImagePlane(1,1);
			m_frtk.imgview->ShowSingleImage(m_image);

			m_frtk1.imgview->SetImagePlane(1,1);
			m_frtk1.imgview->ShowSingleImage(m_image1);

			for (int i=0; i<4; i++) 
			{
				m_frtk.debug[i] = (CImageBaseView*)mainframe->GetDebugImage(i);
				m_frtk1.debug[i] = (CImageBaseView*)mainframe->GetDebugImage(i);
			}

			m_frtk.msgview = (CLogView*)mainframe->GetDJMessage();
			m_frtk1.msgview = (CLogView*)mainframe->GetDJMessage();
			m_frtk.IVSTKinit(m_image);
			m_frtk1.IVSTKinit(m_image1);
		}
	}
}

// 20030607
// foot sim.
void CSoftComputingWndDoc::OnViewFoot() 
{
	int i, j, k;
	CString filename;
	IPLIMAGE fimage[15];
	IPLIMAGE aimage[6];
	FILE *fp;

	float refPixel, curPixel, ratio;

	for (i=0; i<6; i++)
		aimage[i] = cvCreateImage(cvSize(40,80), IPL_DEPTH_8U, 1);

	for (i=0; i<15; i++)
		fimage[i] = cvCreateImage(cvSize(40,80), IPL_DEPTH_8U, 1);

	fp = fopen(".\\bFoot\\bFoot1.dat", "wt");

	for (i=0; i<5; i++)
	{
		filename.Format(".\\bFoot\\1_%d_normal.jpg", i);
		IPLIMAGE loadimage = cvvLoadImage(filename);

		cvvConvertImage(loadimage, fimage[i]);
		
		iplSet(aimage[0], 0);
		iplNot(fimage[i], aimage[0]);
		iplSet(aimage[2], 0);
		iplThreshold(aimage[0], aimage[2], 128);

		iplSet(fimage[i], 0);
		iplCopy(aimage[2], fimage[i]);

		filename.Format(".\\bFoot\\%ds.jpg", i+1);
		cvvSaveImage(filename, fimage[i]);
	}

	for (i=0; i<5; i++)
	{
		refPixel = cvCountNonZero(fimage[i]);

		for (j=0; j<5; j++)
		{
			iplSet(aimage[1], 0);
			iplAnd(fimage[i], fimage[j], aimage[1]);

			curPixel = cvCountNonZero(aimage[1]);

			ratio = curPixel/refPixel;

			fprintf(fp, "%.3f ", ratio);
		}

		fprintf(fp, "\n");
	}

	fclose(fp);

	for (i=0; i<15; i++)
		cvReleaseImage(&(fimage[i]));
	
	for (i=0; i<6; i++)
		cvReleaseImage(&(aimage[i]));
}
