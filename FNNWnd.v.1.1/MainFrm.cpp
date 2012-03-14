// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "ImageBaseView.h"
#include "LogView.h"
#include "MainFrm.h"

#include "MyChart.h"
#include "MyScrollChart.h"

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
	ON_WM_SIZE()
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
	m_bInitSplitter = FALSE;
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

	int sizex = (int)((float)rect.Width()*0.25f);
	int sizey = (int)((float)rect.Height()*0.6f);

	// TODO: Add your specialized code here and/or call the base class
    m_wndSplitter.CreateStatic(this, 2, 1);
	m_wndSplitter2.CreateStatic(&m_wndSplitter, 1, 3, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 0));
	m_wndSplitter3.CreateStatic(&m_wndSplitter, 1, 2, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(1, 0));
	m_wndSplitter4.CreateStatic(&m_wndSplitter2, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter2.IdFromRowCol(0, 2));

	m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CImageBaseView), CSize(0, 0), pContext);	
	m_wndSplitter2.CreateView(0, 1, RUNTIME_CLASS(CMyChart), CSize(0, 0), pContext);
	m_wndSplitter3.CreateView(0, 0, RUNTIME_CLASS(CLogView), CSize(0, 0), pContext);
	m_wndSplitter3.CreateView(0, 1, RUNTIME_CLASS(CMyScrollChart), CSize(0, 0), pContext);
	m_wndSplitter4.CreateView(0, 0, RUNTIME_CLASS(CMyChart), CSize(0, 0), pContext);
	m_wndSplitter4.CreateView(1, 0, RUNTIME_CLASS(CMyChart), CSize(0, 0), pContext);
    
	m_wndSplitter.SetRowInfo(0,sizey,0);
	m_wndSplitter2.SetColumnInfo(0,sizex,0);
	m_wndSplitter2.SetColumnInfo(1,sizex,0);
	m_wndSplitter2.SetColumnInfo(2,sizex*2,0);
	m_wndSplitter3.SetColumnInfo(0,sizex*2,0);
	m_wndSplitter4.SetRowInfo(0,sizey/2,0);

	m_bInitSplitter = TRUE;

	return TRUE;
}

CImageBaseView* CMainFrame::GetImageBaseView()
{
    return (CImageBaseView*)m_wndSplitter2.GetPane(0,0);
}

CLogView* CMainFrame::GetLogView()
{
    return (CLogView*)m_wndSplitter3.GetPane(0,0);
}

CMyScrollChart* CMainFrame::GetScrollChart()
{
    return (CMyScrollChart*)m_wndSplitter3.GetPane(0,1);
}

CMyChart* CMainFrame::GetChart(int chartnum)
{
    return (CMyChart*)m_wndSplitter4.GetPane(chartnum,0);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	CRect cr;
    GetWindowRect(&cr);
	int sizex = (int)((float)cr.Width()*0.25f);
	int sizey = (int)((float)cr.Height()*0.6f);

    if (  m_bInitSplitter && nType != SIZE_MINIMIZED )
    {
		m_wndSplitter.SetRowInfo(0,sizey,0);
		m_wndSplitter2.SetColumnInfo(0,sizex,0);
		m_wndSplitter2.SetColumnInfo(1,sizex,0);
		m_wndSplitter2.SetColumnInfo(2,sizex*2,0);
		m_wndSplitter3.SetColumnInfo(0,sizex*2,0);
		m_wndSplitter4.SetRowInfo(0,sizey/2,0);

        m_wndSplitter.RecalcLayout();
		m_wndSplitter2.RecalcLayout();
		m_wndSplitter3.RecalcLayout();
		m_wndSplitter4.RecalcLayout();
    }	
}