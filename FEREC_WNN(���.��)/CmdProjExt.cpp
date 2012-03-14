// CmdProjExt.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdProjExt.h"

#include "MainFrm.h"
#include "FRTK.h"
#include "SoftComputingWndDoc.h"
#include "highgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdProjExt

IMPLEMENT_DYNCREATE(CCmdProjExt, CFormView)

CCmdProjExt::CCmdProjExt()
	: CFormView(CCmdProjExt::IDD)
{
	//{{AFX_DATA_INIT(CCmdProjExt)
	m_Dir = _T("Horiz");
	//}}AFX_DATA_INIT
}

CCmdProjExt::~CCmdProjExt()
{
}

void CCmdProjExt::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdProjExt)
	DDX_CBString(pDX, IDC_PROJ_DIR, m_Dir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdProjExt, CFormView)
	//{{AFX_MSG_MAP(CCmdProjExt)
	ON_BN_CLICKED(IDC_PROJ_EX, OnProjEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdProjExt diagnostics

#ifdef _DEBUG
void CCmdProjExt::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdProjExt::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdProjExt message handlers
void CCmdProjExt::OnProjEx() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	face->FRTKprojext(image, NULL, m_Dir);
}
