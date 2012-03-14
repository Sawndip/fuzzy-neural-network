// CmdDepth.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdDepth.h"

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
// CCmdDepth

IMPLEMENT_DYNCREATE(CCmdDepth, CFormView)

CCmdDepth::CCmdDepth()
	: CFormView(CCmdDepth::IDD)
{
	//{{AFX_DATA_INIT(CCmdDepth)
	m_Method = _T("BIRCHFIELD");
	//}}AFX_DATA_INIT
}

CCmdDepth::~CCmdDepth()
{
}

void CCmdDepth::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdDepth)
	DDX_CBString(pDX, IDC_DEPTH_METHOD, m_Method);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdDepth, CFormView)
	//{{AFX_MSG_MAP(CCmdDepth)
	ON_BN_CLICKED(IDC_ACT_DEPTH, OnActDepth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdDepth diagnostics

#ifdef _DEBUG
void CCmdDepth::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdDepth::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdDepth message handlers
void CCmdDepth::OnActDepth() 
{
	UpdateData(TRUE);

	CMainFrame* main = (CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE leftimg = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	IPLIMAGE rightimg = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image1;

	face->IVSTKdepth(leftimg, rightimg, m_Method);
}
