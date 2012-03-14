// CmdSF.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdSF.h"

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
// CCmdSF

IMPLEMENT_DYNCREATE(CCmdSF, CFormView)

CCmdSF::CCmdSF()
	: CFormView(CCmdSF::IDD)
{
	//{{AFX_DATA_INIT(CCmdSF)
	m_Method = _T("ANGLE");
	//}}AFX_DATA_INIT
}

CCmdSF::~CCmdSF()
{
}

void CCmdSF::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdSF)
	DDX_CBString(pDX, IDC_SF_FEAT, m_Method);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdSF, CFormView)
	//{{AFX_MSG_MAP(CCmdSF)
	ON_BN_CLICKED(IDC_SF_EXT, OnSfExt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdSF diagnostics

#ifdef _DEBUG
void CCmdSF::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdSF::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdSF message handlers
void CCmdSF::OnSfExt() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	face->FRTKstatfeat(image, NULL, m_Method);
}