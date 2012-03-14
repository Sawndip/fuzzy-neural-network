// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "ImageBaseView.h"
#include "MyCamView.h"

#include "MainFrm.h"
#include "cv.h"
#include "highgui.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	CRect rect;
	GetClientRect(rect);

	int sizex = (int)((float)rect.Width()*0.5f);
	int sizey = (int)((float)rect.Width()*0.5*0.75);
	int sizez = (int)((float)rect.Width()/7.0f);

    if (!m_wndSplitter.CreateStatic(this, 2, 1))
		return FALSE;

	if (!m_wndSplitter2.CreateStatic(&m_wndSplitter, 1, 2, 
		WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 0)))
		return FALSE;

	if (!m_wndSplitter3.CreateStatic(&m_wndSplitter, 1, 7, 
		WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(1, 0)))
		return FALSE;

	if (!m_wndSplitter2.CreateView(0, 0,
		RUNTIME_CLASS(CMyCamView), CSize(0, 0), pContext))
		return FALSE;

	if (!m_wndSplitter2.CreateView(0, 1,
		RUNTIME_CLASS(CImageBaseView), CSize(0, 0), pContext))
		return FALSE;

	m_wndSplitter2.SetColumnInfo(0, sizex, 0);
	m_wndSplitter.SetRowInfo(0, sizey, 0);


	for (int i=0; i<7; i++)
	{
		if (!m_wndSplitter3.CreateView(0, i, RUNTIME_CLASS(CImageBaseView), CSize(sizez-5, 120), pContext))
			return FALSE;
	}
	
	return TRUE;
}

CImageBaseView* CMainFrame::GetImageBaseView()
{
    return (CImageBaseView*)m_wndSplitter2.GetPane(0,1);
}

CMyCamView* CMainFrame::GetMyCamView()
{
    return (CMyCamView*)m_wndSplitter2.GetPane(0,0);
}

CImageBaseView* CMainFrame::GetEmotionalImageView(int i)
{
    return (CImageBaseView*)m_wndSplitter3.GetPane(0,i);
}

/*
void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString emotion;

	switch(nChar)
	{
	case '1':
		emotion.Format("happy.jpg");
		break;
	case '2':
		emotion.Format("sad.jpg");
		break;
	case '3':
		emotion.Format("fear.jpg");
		break;
	case '4':
		emotion.Format("angry.jpg");
		break;
	case '5':
		emotion.Format("surprise.jpg");
		break;
	case '6':
		emotion.Format("disgust.jpg");
		break;
	case '7':
		emotion.Format("neutral.jpg");
		break;
	default:
		break;
	}

	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(nChar-49);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	str = str + emotion;

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
	
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}
*/

/*
void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString emotion;

	switch(nChar)
	{
	case '0':		
		break;
	case '1':
		emotion.Format("happy.jpg");
		break;
	case '2':
		emotion.Format("sad.jpg");
		break;
	case '3':
		emotion.Format("fear.jpg");
		break;
	case '4':
		emotion.Format("angry.jpg");
		break;
	case '5':
		emotion.Format("surprise.jpg");
		break;
	case '6':
		emotion.Format("disgust.jpg");
		break;
	case '7':
		emotion.Format("neutral.jpg");
		break;
	default:
		break;
	}

	CTime time = CTime::GetCurrentTime();
	CString str;

	CMainFrame* mainframe = (CMainFrame*)AfxGetMainWnd();
	CMyCamView* camview = (CMyCamView*)mainframe->GetMyCamView();
	CImageBaseView* imgview = (CImageBaseView*)mainframe->GetEmotionalImageView(nChar-49);
	str.Format(".\\%04d%02d%02d%02d%02d%02d_", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
	str = str + emotion;

	IPLIMAGE image = ((camview->Camera()).GetFrame()).GetImage();
	cvvSaveImage(str, image);

	imgview->SetImagePlane(1,1);
	imgview->AddImage(0,0, image);
	imgview->ShowImage();
	
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/