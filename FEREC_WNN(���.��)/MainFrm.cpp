// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "LogView.h"
#include "ImageBaseView.h"

#include "MainFrm.h"

#include "CmdColor.h"
#include "CmdFilter.h"

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
	ON_COMMAND(ID_WNN_LEARNING, OnWnnLearning)
	ON_COMMAND(ID_WNN_TEST, OnWnnTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
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

	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 70);
	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 110);
	m_wndStatusBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 130);

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

	cs.cx = 1024;
	cs.cy = 768;

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
	m_wndWhole = new CSplitterWnd;
	m_wndKernel = new CSplitterWnd;
	m_wndCoeff = new CSplitterWnd;
	m_wndControl = new CSplitterWnd;

	CRect rect;
	GetClientRect(rect);

    m_wndWhole->CreateStatic(this,1,4);
	m_wndKernel->CreateStatic(m_wndWhole, 6, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndWhole->IdFromRowCol(0,1));
	m_wndCoeff->CreateStatic(m_wndWhole, 6, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndWhole->IdFromRowCol(0,2));
	m_wndControl->CreateStatic(m_wndWhole, 2, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndWhole->IdFromRowCol(0,3));

    m_wndWhole->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(320,240),pContext);
	//m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdColor),CSize(100,100),pContext);
	
	m_wndKernel->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndKernel->CreateView(1,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndKernel->CreateView(2,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndKernel->CreateView(3,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndKernel->CreateView(4,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndKernel->CreateView(5,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);

	m_wndCoeff->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndCoeff->CreateView(1,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndCoeff->CreateView(2,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndCoeff->CreateView(3,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndCoeff->CreateView(4,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndCoeff->CreateView(5,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);

	m_wndControl->CreateView(0,0,RUNTIME_CLASS(CLogView),CSize(160,120),pContext);
	m_wndControl->CreateView(1,0,RUNTIME_CLASS(CCmdColor),CSize(160,120),pContext);

	/*
	m_wndWhole->SetColumnInfo(0, rect.Width()/2, 0);
	m_wndWhole->SetRowInfo(0, rect.Width()/8*3, 0);
	m_wndDebugImage->SetColumnInfo(0, rect.Width()/4, 0);
	m_wndDebugImage->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndAddedView->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndMemory->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndMemory->SetColumnInfo(0, rect.Width()/8, 0);
	m_wndMemory->SetColumnInfo(1, rect.Width()/8, 0);
	m_wndMemory->SetColumnInfo(2, rect.Width()/8, 0);
	*/
	
	return TRUE;
}

CImageBaseView* CMainFrame::GetImageBaseView()
{
    return (CImageBaseView*)m_wndWhole->GetPane(0,0);
}

CLogView* CMainFrame::GetDJMessage()
{
    return (CLogView*)m_wndControl->GetPane(0,0);
}

CView* CMainFrame::GetControlView()
{
    return (CView*)m_wndControl->GetPane(1,0);
}

CImageBaseView* CMainFrame::GetKernelImage(int num)
{
	int cx, cy;

	switch(num)
	{
		case 0:
			cx = 0; cy = 0;
			break;
		case 1:
			cx = 0; cy = 1;
			break;
		case 2:
			cx = 0; cy = 2;
			break;
		case 3:
			cx = 0; cy = 3;
			break;
		case 4:
			cx = 0; cy = 4;
			break;
		case 5:
			cx = 0; cy = 5;
			break;
		default:
			cx = 0; cy = 0;
			break;
	}

    return (CImageBaseView*)m_wndKernel->GetPane(cx,cy);
}

CImageBaseView* CMainFrame::GetCoeffImage(int num)
{
	int cx, cy;

	switch(num)
	{
		case 0:
			cx = 0; cy = 0;
			break;
		case 1:
			cx = 0; cy = 1;
			break;
		case 2:
			cx = 0; cy = 2;
			break;
		case 3:
			cx = 0; cy = 3;
			break;
		case 4:
			cx = 0; cy = 4;
			break;
		case 5:
			cx = 0; cy = 5;
			break;
		default:
			cx = 0; cy = 0;
			break;
	}

    return (CImageBaseView*)m_wndCoeff->GetPane(cx,cy);
}

void CMainFrame::SwitchView(ViewType nType)
{
	CView* pOldView=(CView*)m_wndControl->GetPane(1,0);
	
	CCreateContext context;
	context.m_pCurrentDoc=pOldView->GetDocument();
	pOldView->ShowWindow(SW_HIDE);

	switch(nType)
	{
		case LEARN:
			m_wndControl->DeleteView(1,0);
			m_wndControl->CreateView(1,0,RUNTIME_CLASS(CCmdColor),CSize(0,0),&context);
			((CCmdColor*)GetControlView())->OnInitialUpdate();
			break;
		case TEST:
			m_wndControl->DeleteView(1,0);
			m_wndControl->CreateView(1,0,RUNTIME_CLASS(CCmdFilter),CSize(0,0),&context);
			((CCmdFilter*)GetControlView())->OnInitialUpdate();
			break;
		default:
			break;
	}

	m_wndWhole->RecalcLayout();
}

void CMainFrame::OnWnnLearning() 
{
	SwitchView(LEARN);	
}

void CMainFrame::OnWnnTest() 
{
	SwitchView(TEST);	
}
