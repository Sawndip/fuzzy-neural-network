// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "LogView.h"
#include "ImageBaseView.h"

#include "MainFrm.h"

#include "CmdColor.h"
#include "CmdFilter.h"
#include "CmdFourier.h"
#include "CmdProjExt.h"
#include "CmdBlob.h"
#include "CmdCoord.h"
#include "CmdGF.h"
#include "CmdSF.h"
#include "CmdTM.h"
#include "CmdTh.h"
#include "CmdVer.h"
#include "CmdAlu.h"
#include "CmdNN.h"
#include "CmdFMMNN.h"
#include "CmdFNN.h"
#include "CmdEigen.h"
#include "CmdDepth.h"
#include "CmdMotion.h"

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
	ON_COMMAND(ID_BLOB, OnBlob)
	ON_COMMAND(ID_COLOR, OnColor)
	ON_COMMAND(ID_COORD, OnCoord)
	ON_COMMAND(ID_DEPTH, OnDepth)
	ON_COMMAND(ID_EDGE, OnEdge)
	ON_COMMAND(ID_FEREC, OnFerec)
	ON_COMMAND(ID_FMMNN, OnFmmnn)
	ON_COMMAND(ID_FNN, OnFnn)
	ON_COMMAND(ID_MOTION, OnMotion)
	ON_COMMAND(ID_NN, OnNn)
	ON_COMMAND(ID_OCCLUSION, OnOcclusion)
	ON_COMMAND(ID_PURSUIT, OnPursuit)
	ON_COMMAND(ID_VERGENCE, OnVergence)
	ON_COMMAND(ID_ALU, OnAlu)
	ON_COMMAND(ID_TH, OnTh)
	ON_COMMAND(ID_PROJ, OnProj)
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
	for (int i=0; i<6; i++)
		m_wndSplit[i] = new CSplitterWnd;

	CRect rect;
	GetClientRect(rect);

    m_wndSplit[0]->CreateStatic(this,2,1);
	m_wndSplit[1]->CreateStatic(m_wndSplit[0], 1, 3, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplit[0]->IdFromRowCol(0,0));
	m_wndSplit[2]->CreateStatic(m_wndSplit[1], 4, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplit[1]->IdFromRowCol(0,2));
	m_wndSplit[3]->CreateStatic(m_wndSplit[0], 1, 2, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplit[0]->IdFromRowCol(1,0));
	m_wndSplit[4]->CreateStatic(m_wndSplit[3], 2, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplit[3]->IdFromRowCol(0,0));
	m_wndSplit[5]->CreateStatic(m_wndSplit[4], 1, 4, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplit[4]->IdFromRowCol(1,0));

	m_wndSplit[1]->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(320,240),pContext);
	m_wndSplit[1]->CreateView(0,1,RUNTIME_CLASS(CImageBaseView),CSize(320,240),pContext);

	m_wndSplit[2]->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndSplit[2]->CreateView(1,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndSplit[2]->CreateView(2,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndSplit[2]->CreateView(3,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);

	m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdColor),CSize(160,120),pContext);

	m_wndSplit[4]->CreateView(0,0,RUNTIME_CLASS(CLogView),CSize(160,120),pContext);

	m_wndSplit[5]->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndSplit[5]->CreateView(0,1,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndSplit[5]->CreateView(0,2,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndSplit[5]->CreateView(0,3,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);

	m_wndSplit[0]->SetRowInfo(0, rect.Width()/28*9, 0);
	m_wndSplit[1]->SetColumnInfo(0, rect.Width()/7*3, 0);
	m_wndSplit[1]->SetColumnInfo(1, rect.Width()/7*3, 0);

	m_wndSplit[2]->SetRowInfo(0, rect.Width()/102*9 - 5, 0);
	m_wndSplit[2]->SetRowInfo(1, rect.Width()/102*9 - 5, 0);
	m_wndSplit[2]->SetRowInfo(2, rect.Width()/102*9 - 5, 0);

	m_wndSplit[3]->SetColumnInfo(0, rect.Width()/2, 0);

	m_wndSplit[5]->SetColumnInfo(0, rect.Width()/8 - 5, 0);
	m_wndSplit[5]->SetColumnInfo(1, rect.Width()/8 - 5, 0);
	m_wndSplit[5]->SetColumnInfo(2, rect.Width()/8 - 5, 0);
	m_wndSplit[4]->SetRowInfo(0, rect.Width()/28*7, 0);

	/*
	m_wndWhole->SetColumnInfo(0, rect.Width()/2, 0);
	
	
	m_wndDebugImage->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndDebugImage->SetColumnInfo(0, rect.Width()/8, 0);
	m_wndDebugImage->SetColumnInfo(1, rect.Width()/8, 0);
	m_wndDebugImage->SetColumnInfo(2, rect.Width()/8, 0);
	
	m_wndMemory->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndMemory->SetColumnInfo(0, rect.Width()/8, 0);
	m_wndMemory->SetColumnInfo(1, rect.Width()/8, 0);
	m_wndMemory->SetColumnInfo(2, rect.Width()/8, 0);
	*/
	
	return TRUE;
}

CImageBaseView* CMainFrame::GetImageBaseView(int num)
{
    return (CImageBaseView*)m_wndSplit[1]->GetPane(0,num);
}

CLogView* CMainFrame::GetDJMessage()
{
    return (CLogView*)m_wndSplit[4]->GetPane(0,0);
}

CView* CMainFrame::GetControlView()
{
    return (CView*)m_wndSplit[3]->GetPane(0,1);
}

CImageBaseView* CMainFrame::GetDebugImage(int num)
{
	int cx, cy;

	switch(num)
	{
		case 0:
			cx = 0; cy = 0;
			break;
		case 1:
			cx = 1; cy = 0;
			break;
		case 2:
			cx = 2; cy = 0;
			break;
		case 3:
			cx = 3; cy = 0;
			break;
		default:
			cx = 0; cy = 0;
			break;
	}

    return (CImageBaseView*)m_wndSplit[2]->GetPane(cx,cy);
}

CImageBaseView* CMainFrame::GetMemoryImage(int num)
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
		default:
			cx = 0; cy = 0;
			break;
	}

    return (CImageBaseView*)m_wndSplit[5]->GetPane(cx,cy);
}

void CMainFrame::SwitchView(ViewType nType)
{
	CView* pOldView=(CView*)m_wndSplit[3]->GetPane(0,1);
	
	CCreateContext context;
	context.m_pCurrentDoc=pOldView->GetDocument();
	pOldView->ShowWindow(SW_HIDE);

	switch(nType)
	{
		case COLORCONVERT:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdColor),CSize(0,0),&context);
			((CCmdColor*)GetControlView())->OnInitialUpdate();
			break;
		case FILTERING:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdFilter),CSize(0,0),&context);
			((CCmdFilter*)GetControlView())->OnInitialUpdate();
			break;
		case FOURIER:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdFourier),CSize(0,0),&context);
			((CCmdFourier*)GetControlView())->OnInitialUpdate();
			break;
		case PROJ:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdProjExt),CSize(0,0),&context);
			((CCmdProjExt*)GetControlView())->OnInitialUpdate();
			break;
		case BLOB:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdBlob),CSize(0,0),&context);
			((CCmdBlob*)GetControlView())->OnInitialUpdate();
			break;
		case COORD:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdCoord),CSize(0,0),&context);
			((CCmdCoord*)GetControlView())->OnInitialUpdate();
			break;
		case GEO:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdGF),CSize(0,0),&context);
			((CCmdGF*)GetControlView())->OnInitialUpdate();
			break;
		case STAT:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdSF),CSize(0,0),&context);
			((CCmdSF*)GetControlView())->OnInitialUpdate();
			break;
		case TEMP:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdTM),CSize(0,0),&context);
			((CCmdTM*)GetControlView())->OnInitialUpdate();
			break;
		case TH:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdTh),CSize(0,0),&context);
			((CCmdTh*)GetControlView())->OnInitialUpdate();
			break;
		case VER:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdVer),CSize(0,0),&context);
			((CCmdVer*)GetControlView())->OnInitialUpdate();
			break;
		case ALU:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdAlu),CSize(0,0),&context);
			((CCmdAlu*)GetControlView())->OnInitialUpdate();
			break;
		case FMMNN:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdFMMNN),CSize(0,0),&context);
			((CCmdFMMNN*)GetControlView())->OnInitialUpdate();
			break;
		case FNN:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdFNN),CSize(0,0),&context);
			((CCmdFNN*)GetControlView())->OnInitialUpdate();
			break;
		case NN:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdNN),CSize(0,0),&context);
			((CCmdNN*)GetControlView())->OnInitialUpdate();
			break;
		case EIGEN:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdEigen),CSize(0,0),&context);
			((CCmdEigen*)GetControlView())->OnInitialUpdate();
			break;
		case MOTION:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdMotion),CSize(0,0),&context);
			((CCmdMotion*)GetControlView())->OnInitialUpdate();
			break;
		case DEPTH:
			m_wndSplit[3]->DeleteView(0,1);
			m_wndSplit[3]->CreateView(0,1,RUNTIME_CLASS(CCmdDepth),CSize(0,0),&context);
			((CCmdDepth*)GetControlView())->OnInitialUpdate();
			break;
		default:
			break;
	}

	m_wndSplit[3]->RecalcLayout();
}

/*
void CMainFrame::OnFouTx() 
{
	SwitchView(FOURIER);
}

void CMainFrame::OnTemplateMatching() 
{
	SwitchView(TEMP);
}

void CMainFrame::OnStatFeat() 
{
	SwitchView(STAT);
}

void CMainFrame::OnGeoFeat() 
{
	SwitchView(GEO);
}

void CMainFrame::OnGeoEst() 
{
	SwitchView(VER);
}

void CMainFrame::OnFaceVer() 
{
	SwitchView(VER);
}

void CMainFrame::OnCompTemplate() 
{
	SwitchView(TEMP);
}

void CMainFrame::OnEigenface() 
{
	SwitchView(EIGEN);
}
*/

void CMainFrame::OnBlob() 
{
	SwitchView(BLOB);
}

void CMainFrame::OnColor() 
{
	SwitchView(COLORCONVERT);
}

void CMainFrame::OnCoord() 
{
	SwitchView(COORD);
}

void CMainFrame::OnDepth() 
{
	SwitchView(DEPTH);
}

void CMainFrame::OnEdge() 
{
	SwitchView(FILTERING);
}

void CMainFrame::OnFerec() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnFmmnn() 
{
	SwitchView(FMMNN);
}

void CMainFrame::OnFnn() 
{
	SwitchView(FNN);
}

void CMainFrame::OnMotion() 
{
	SwitchView(MOTION);
}

void CMainFrame::OnNn() 
{
	SwitchView(NN);	
}

void CMainFrame::OnOcclusion() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnPursuit() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnVergence() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnAlu() 
{
	SwitchView(ALU);
}

void CMainFrame::OnTh() 
{
	SwitchView(TH);
}

void CMainFrame::OnProj() 
{
	SwitchView(PROJ);
}
