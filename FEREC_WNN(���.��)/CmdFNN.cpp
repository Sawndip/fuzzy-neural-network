// CmdFNN.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdFNN.h"

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
// CCmdFNN

IMPLEMENT_DYNCREATE(CCmdFNN, CFormView)

CCmdFNN::CCmdFNN()
	: CFormView(CCmdFNN::IDD)
{
	//{{AFX_DATA_INIT(CCmdFNN)
	m_Ep = 1e4;
	m_Err = 1e-2;
	m_Lr = 0.3f;
	m_Method = _T("TRAIN");
	m_Sp1 = 1e-3;
	m_Sp2 = 1e-2;
	//}}AFX_DATA_INIT
}

CCmdFNN::~CCmdFNN()
{
}

void CCmdFNN::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdFNN)
	DDX_Text(pDX, IDC_FNN_EP, m_Ep);
	DDX_Text(pDX, IDC_FNN_ERR, m_Err);
	DDX_Text(pDX, IDC_FNN_LR, m_Lr);
	DDX_CBString(pDX, IDC_FNN_METHOD, m_Method);
	DDX_Text(pDX, IDC_FNN_SP1, m_Sp1);
	DDX_Text(pDX, IDC_FNN_SP2, m_Sp2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdFNN, CFormView)
	//{{AFX_MSG_MAP(CCmdFNN)
	ON_BN_CLICKED(IDC_REC_FNN, OnRecFnn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdFNN diagnostics

#ifdef _DEBUG
void CCmdFNN::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdFNN::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdFNN message handlers

void CCmdFNN::OnRecFnn() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;

	float param[] = {m_Ep, m_Err, m_Lr, m_Sp1, m_Sp2};

	face->FRTKrecFNN(m_Method, param);
}