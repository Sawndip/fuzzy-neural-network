// CmdMotion.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdMotion.h"

#include "MainFrm.h"
#include "IVSTK.h"
#include "SoftComputingWndDoc.h"
#include "highgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdMotion

IMPLEMENT_DYNCREATE(CCmdMotion, CFormView)

CCmdMotion::CCmdMotion()
	: CFormView(CCmdMotion::IDD)
{
	//{{AFX_DATA_INIT(CCmdMotion)
	m_Method = _T("HS");
	//}}AFX_DATA_INIT
}

CCmdMotion::~CCmdMotion()
{
}

void CCmdMotion::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdMotion)
	DDX_CBString(pDX, IDC_MOTION_METHOD, m_Method);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdMotion, CFormView)
	//{{AFX_MSG_MAP(CCmdMotion)
	ON_BN_CLICKED(IDC_ACT_MOTION, OnActMotion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdMotion diagnostics

#ifdef _DEBUG
void CCmdMotion::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdMotion::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdMotion message handlers

void CCmdMotion::OnActMotion() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE leftimg = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	IPLIMAGE rightimg = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image1;

	face->IVSTKmotion(leftimg, rightimg, m_Method);
}
