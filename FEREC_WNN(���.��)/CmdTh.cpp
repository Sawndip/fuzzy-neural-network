// CmdTh.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdTh.h"

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
// CCmdTh

IMPLEMENT_DYNCREATE(CCmdTh, CFormView)

CCmdTh::CCmdTh()
	: CFormView(CCmdTh::IDD)
{
	//{{AFX_DATA_INIT(CCmdTh)
	m_Method = _T("GT");
	//}}AFX_DATA_INIT
}

CCmdTh::~CCmdTh()
{
}

void CCmdTh::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdTh)
	DDX_CBString(pDX, IDC_TH_METHOD, m_Method);
	DDX_Control(pDX, IDC_MIN_TH, m_MinTh);
	DDX_Control(pDX, IDC_MAX_TH, m_MaxTh);
	DDX_Text(pDX, IDC_MIN_THR, m_MinThr);
	DDX_Text(pDX, IDC_MAX_THR, m_MaxThr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdTh, CFormView)
	//{{AFX_MSG_MAP(CCmdTh)
	ON_BN_CLICKED(IDC_ACT_TH, OnActTh)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_MIN_TH, OnReleasedcaptureMinTh)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_MAX_TH, OnReleasedcaptureMaxTh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdTh diagnostics

#ifdef _DEBUG
void CCmdTh::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdTh::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdTh message handlers
void CCmdTh::OnActTh() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	face->FRTKthreshold(image, NULL, m_Method, m_MinTh.GetPos(), m_MaxTh.GetPos());
}

void CCmdTh::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_MinTh.SetRange(0, 255);
	m_MaxTh.SetRange(0, 255);

	UpdateData(FALSE);
}

void CCmdTh::OnReleasedcaptureMinTh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_MinThr.Format("%03d", m_MinTh.GetPos());

	*pResult = 0;

	UpdateData(FALSE);
}

void CCmdTh::OnReleasedcaptureMaxTh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_MaxThr.Format("%03d", m_MaxTh.GetPos());

	*pResult = 0;

	UpdateData(FALSE);
}