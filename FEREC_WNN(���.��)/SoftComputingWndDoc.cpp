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
	m_frtk.FRTKend();
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

//		for (int i=0; i<4; i++) m_frtk.debug[i] = (CImageBaseView*)mainframe->GetDebugImage(i);

		m_frtk.msgview = (CLogView*)mainframe->GetDJMessage();
		m_frtk.FRTKinit(m_image);
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
