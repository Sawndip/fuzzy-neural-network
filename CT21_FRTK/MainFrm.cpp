// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "LogView.h"
#include "ImageBaseView.h"
#include "SoftComputingWndDoc.h"

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

//#include "MyCamView.h"

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
	ON_COMMAND(ID_COLOR_CONVERSION, OnColorConvert)
	ON_COMMAND(ID_IMG_FILTERING, OnFiltering)
	ON_COMMAND(ID_FOU_TX, OnFouTx)
	ON_COMMAND(ID_TEMPLATE_MATCHING, OnTemplateMatching)
	ON_COMMAND(ID_STAT_FEAT, OnStatFeat)
	ON_COMMAND(ID_PROJ_EXT, OnProjExt)
	ON_COMMAND(ID_GEO_FEAT, OnGeoFeat)
	ON_COMMAND(ID_GEO_EST, OnGeoEst)
	ON_COMMAND(ID_COORD_TX, OnCoordTx)
	ON_COMMAND(ID_COMP_TEMPLATE, OnCompTemplate)
	ON_COMMAND(ID_BLOB_ANAL, OnBlobAnal)
	ON_COMMAND(ID_PRE_TH, OnPreTh)
	ON_COMMAND(ID_RAW_ALU, OnRawAlu)
	ON_COMMAND(ID_EIGENFACE, OnEigenface)
	ON_COMMAND(ID_FMMNN_CLS, OnFmmnnCls)
	ON_COMMAND(ID_FNN_CLS, OnFnnCls)
	ON_COMMAND(ID_NN_CLS, OnNnCls)
/*
	ON_COMMAND(ID_VIDEO_FEREC, OnVideoFerec)
	ON_COMMAND(ID_VIDEO_FACIALFEATURE, OnVideoFacialfeature)
	ON_COMMAND(ID_VIDEO_FACEDETECT, OnVideoFacedetect)
	ON_COMMAND(ID_VIDEO_CAMFORMAT, OnVideoCamformat)
	ON_COMMAND(ID_VIDEO_CAMONOFF, OnVideoCamonoff)
	ON_COMMAND(ID_VIDEO_CAMSETTING, OnVideoCamsetting)
	*/
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
	m_wndDebugImage = new CSplitterWnd;
	m_wndAddedView = new CSplitterWnd;
	m_wndMemory = new CSplitterWnd;

	CRect rect;
	GetClientRect(rect);

    m_wndWhole->CreateStatic(this,2,2);
	m_wndDebugImage->CreateStatic(m_wndWhole, 2, 2, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndWhole->IdFromRowCol(0,1));
	m_wndAddedView->CreateStatic(m_wndWhole, 2, 1, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndWhole->IdFromRowCol(1,0));
	m_wndMemory->CreateStatic(m_wndAddedView, 1, 4, 
		WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndAddedView->IdFromRowCol(1,0));

    m_wndWhole->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(320,240),pContext);
	//m_wndWhole->CreateView(0,0,RUNTIME_CLASS(CMyCamView),CSize(320,240),pContext);

	m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdColor),CSize(100,100),pContext);
	
	m_wndDebugImage->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndDebugImage->CreateView(0,1,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndDebugImage->CreateView(1,0,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);
	m_wndDebugImage->CreateView(1,1,RUNTIME_CLASS(CImageBaseView),CSize(160,120),pContext);

	m_wndAddedView->CreateView(0,0,RUNTIME_CLASS(CLogView),CSize(160,120),pContext);

	m_wndMemory->CreateView(0,0,RUNTIME_CLASS(CImageBaseView),CSize(80,60),pContext);
	m_wndMemory->CreateView(0,1,RUNTIME_CLASS(CImageBaseView),CSize(80,60),pContext);
	m_wndMemory->CreateView(0,2,RUNTIME_CLASS(CImageBaseView),CSize(80,60),pContext);
	m_wndMemory->CreateView(0,3,RUNTIME_CLASS(CImageBaseView),CSize(80,60),pContext);

	m_wndWhole->SetColumnInfo(0, rect.Width()/2, 0);
	m_wndWhole->SetRowInfo(0, rect.Width()/8*3, 0);
	m_wndDebugImage->SetColumnInfo(0, rect.Width()/4, 0);
	m_wndDebugImage->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndAddedView->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndMemory->SetRowInfo(0, rect.Width()/16*3, 0);
	m_wndMemory->SetColumnInfo(0, rect.Width()/8, 0);
	m_wndMemory->SetColumnInfo(1, rect.Width()/8, 0);
	m_wndMemory->SetColumnInfo(2, rect.Width()/8, 0);
	
	return TRUE;
}

/*
CMyCamView* CMainFrame::GetCameraView()
{
	return (CMyCamView*)m_wndWhole->GetPane(0,0);
}
*/

CImageBaseView* CMainFrame::GetImageBaseView()
{
    return (CImageBaseView*)m_wndWhole->GetPane(0,0);
}

CLogView* CMainFrame::GetDJMessage()
{
    return (CLogView*)m_wndAddedView->GetPane(0,0);
}

CView* CMainFrame::GetControlView()
{
    return (CView*)m_wndWhole->GetPane(1,1);
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
			cx = 0; cy = 1;
			break;
		case 2:
			cx = 1; cy = 0;
			break;
		case 3:
			cx = 1; cy = 1;
			break;
		default:
			cx = 0; cy = 0;
			break;
	}

    return (CImageBaseView*)m_wndDebugImage->GetPane(cx,cy);
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

    return (CImageBaseView*)m_wndMemory->GetPane(cx,cy);
}

void CMainFrame::SwitchView(ViewType nType)
{
	CView* pOldView=(CView*)m_wndWhole->GetPane(1,1);
	
	CCreateContext context;
	context.m_pCurrentDoc=pOldView->GetDocument();
	pOldView->ShowWindow(SW_HIDE);

	switch(nType)
	{
		case COLORCONVERT:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdColor),CSize(0,0),&context);
			((CCmdColor*)GetControlView())->OnInitialUpdate();
			break;
		case FILTERING:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdFilter),CSize(0,0),&context);
			((CCmdFilter*)GetControlView())->OnInitialUpdate();
			break;
		case FOURIER:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdFourier),CSize(0,0),&context);
			((CCmdFourier*)GetControlView())->OnInitialUpdate();
			break;
		case PROJ:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdProjExt),CSize(0,0),&context);
			((CCmdProjExt*)GetControlView())->OnInitialUpdate();
			break;
		case BLOB:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdBlob),CSize(0,0),&context);
			((CCmdBlob*)GetControlView())->OnInitialUpdate();
			break;
		case COORD:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdCoord),CSize(0,0),&context);
			((CCmdCoord*)GetControlView())->OnInitialUpdate();
			break;
		case GEO:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdGF),CSize(0,0),&context);
			((CCmdGF*)GetControlView())->OnInitialUpdate();
			break;
		case STAT:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdSF),CSize(0,0),&context);
			((CCmdSF*)GetControlView())->OnInitialUpdate();
			break;
		case TEMP:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdTM),CSize(0,0),&context);
			((CCmdTM*)GetControlView())->OnInitialUpdate();
			break;
		case TH:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdTh),CSize(0,0),&context);
			((CCmdTh*)GetControlView())->OnInitialUpdate();
			break;
		case VER:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdVer),CSize(0,0),&context);
			((CCmdVer*)GetControlView())->OnInitialUpdate();
			break;
		case ALU:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdAlu),CSize(0,0),&context);
			((CCmdAlu*)GetControlView())->OnInitialUpdate();
			break;
		case FMMNN:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdFMMNN),CSize(0,0),&context);
			((CCmdFMMNN*)GetControlView())->OnInitialUpdate();
			break;
		case FNN:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdFNN),CSize(0,0),&context);
			((CCmdFNN*)GetControlView())->OnInitialUpdate();
			break;
		case NN:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdNN),CSize(0,0),&context);
			((CCmdNN*)GetControlView())->OnInitialUpdate();
			break;
		case EIGEN:
			m_wndWhole->DeleteView(1,1);
			m_wndWhole->CreateView(1,1,RUNTIME_CLASS(CCmdEigen),CSize(0,0),&context);
			((CCmdEigen*)GetControlView())->OnInitialUpdate();
			break;
		default:
			break;
	}

	m_wndWhole->RecalcLayout();
}

void CMainFrame::OnColorConvert() 
{
	SwitchView(COLORCONVERT);
}

void CMainFrame::OnFiltering() 
{
	SwitchView(FILTERING);
}
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

void CMainFrame::OnProjExt() 
{
	SwitchView(PROJ);
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

void CMainFrame::OnCoordTx() 
{
	SwitchView(COORD);
}

void CMainFrame::OnCompTemplate() 
{
	SwitchView(TEMP);
}

void CMainFrame::OnBlobAnal() 
{
	SwitchView(BLOB);
}

void CMainFrame::OnPreTh() 
{
	SwitchView(TH);
}

void CMainFrame::OnRawAlu() 
{
	SwitchView(ALU);
}

void CMainFrame::OnEigenface() 
{
	SwitchView(EIGEN);
}

void CMainFrame::OnFmmnnCls() 
{
	SwitchView(FMMNN);
}

void CMainFrame::OnFnnCls() 
{
	SwitchView(FNN);
}

void CMainFrame::OnNnCls() 
{
	SwitchView(NN);	
}

/*
void CMainFrame::OnVideoFerec() 
{
	CFRTK* face = &((CSoftComputingWndDoc*)GetActiveDocument())->m_frtk;
	CMyCamView* camview = (CMyCamView*)((CSoftComputingWndDoc*)GetCameraView());
	IPLIMAGE image = (camview->Camera()).GetFrame().GetImage();

	face->FRTKferec(image, NULL, "", NULL);
}

void CMainFrame::OnVideoFacialfeature() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnVideoFacedetect() 
{
}

void CMainFrame::OnVideoCamformat() 
{
	CMyCamView* camview = GetCameraView();

	camview->CameraFormat();
}

void CMainFrame::OnVideoCamonoff() 
{
	CMyCamView* camview = GetCameraView();

	if (!(camview->Camera()).IsRunning())
	{	
		camview->CameraOn();
		
		((CSoftComputingWndDoc*)GetActiveDocument())->InitCam();
	}
	else
	{
		camview->CameraOff();
		((CSoftComputingWndDoc*)GetActiveDocument())->CloseCam();
	}
}

void CMainFrame::OnVideoCamsetting() 
{
	CMyCamView* camview = GetCameraView();

	camview->CameraSetting();
}
*/