// CmdEigen.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdEigen.h"

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
// CCmdEigen

IMPLEMENT_DYNCREATE(CCmdEigen, CFormView)

CCmdEigen::CCmdEigen()
	: CFormView(CCmdEigen::IDD)
{
	//{{AFX_DATA_INIT(CCmdEigen)
	m_Method = _T("TRAIN");
	m_Ep = 0.0f;
	m_Err = 0.0f;
	//}}AFX_DATA_INIT
}

CCmdEigen::~CCmdEigen()
{
}

void CCmdEigen::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdEigen)
	DDX_CBString(pDX, IDC_EIGEN_METHOD, m_Method);
	DDX_Text(pDX, IDC_EIGEN_EP, m_Ep);
	DDX_Text(pDX, IDC_EIGEN_ERR, m_Err);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdEigen, CFormView)
	//{{AFX_MSG_MAP(CCmdEigen)
	ON_BN_CLICKED(IDC_REC_EIGEN, OnRecEigen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdEigen diagnostics

#ifdef _DEBUG
void CCmdEigen::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdEigen::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdEigen message handlers

void CCmdEigen::OnRecEigen() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	//CString str = m_source + '2' + m_result;

	float param[] = {m_Ep, m_Err};
	face->IVSTKeigenobject(image, NULL, m_Method, param);
}
