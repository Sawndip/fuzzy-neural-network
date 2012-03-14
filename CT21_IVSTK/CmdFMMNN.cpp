// CmdFMMNN.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdFMMNN.h"

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
// CCmdFMMNN

IMPLEMENT_DYNCREATE(CCmdFMMNN, CFormView)

CCmdFMMNN::CCmdFMMNN()
	: CFormView(CCmdFMMNN::IDD)
{
	//{{AFX_DATA_INIT(CCmdFMMNN)
	m_Method = _T("TRAIN");
	m_R = 4.0f;
	m_Theta = 0.3f;
	//}}AFX_DATA_INIT
}

CCmdFMMNN::~CCmdFMMNN()
{
}

void CCmdFMMNN::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdFMMNN)
	DDX_CBString(pDX, IDC_FMMNN_METHOD, m_Method);
	DDX_Text(pDX, IDC_FMMNN_R, m_R);
	DDX_Text(pDX, IDC_FMMNN_THETA, m_Theta);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdFMMNN, CFormView)
	//{{AFX_MSG_MAP(CCmdFMMNN)
	ON_BN_CLICKED(IDC_REC_FMMNN, OnRecFmmnn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdFMMNN diagnostics

#ifdef _DEBUG
void CCmdFMMNN::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdFMMNN::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdFMMNN message handlers

void CCmdFMMNN::OnRecFmmnn() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;

	float param[] = {m_R, m_Theta};

	face->IVSTKrecFMMNN(m_Method, param);
}
