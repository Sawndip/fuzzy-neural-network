// CmdNN.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdNN.h"

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
// CCmdNN

IMPLEMENT_DYNCREATE(CCmdNN, CFormView)

CCmdNN::CCmdNN()
	: CFormView(CCmdNN::IDD)
{
	//{{AFX_DATA_INIT(CCmdNN)
	m_Ep = 1e4;
	m_Err = 1e-3;
	m_Lr = 0.3f;
	m_Mo = 0.5f;
	m_Method = _T("TRAIN");
	//}}AFX_DATA_INIT
}

CCmdNN::~CCmdNN()
{
}

void CCmdNN::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdNN)
	DDX_Text(pDX, IDC_NN_EP, m_Ep);
	DDX_Text(pDX, IDC_NN_ERR, m_Err);
	DDX_Text(pDX, IDC_NN_LR, m_Lr);
	DDX_Text(pDX, IDC_NN_MO, m_Mo);
	DDX_CBString(pDX, IDC_NN_METHOD, m_Method);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdNN, CFormView)
	//{{AFX_MSG_MAP(CCmdNN)
	ON_BN_CLICKED(IDC_REC_NN, OnRecNn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdNN diagnostics

#ifdef _DEBUG
void CCmdNN::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdNN::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdNN message handlers

void CCmdNN::OnRecNn() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;

	float param[] = {m_Ep, m_Err, m_Lr, m_Mo};

	face->IVSTKrecNN(m_Method, param);
}
