// CmdAlu.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdAlu.h"

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
// CCmdAlu

IMPLEMENT_DYNCREATE(CCmdAlu, CFormView)

CCmdAlu::CCmdAlu()
	: CFormView(CCmdAlu::IDD)
{
	//{{AFX_DATA_INIT(CCmdAlu)
	m_Method = _T("AND");
	m_Const = _T("0.0");
	m_Image1 = TRUE;
	m_Image2 = TRUE;
	m_Image3 = FALSE;
	//}}AFX_DATA_INIT
}

CCmdAlu::~CCmdAlu()
{
}

void CCmdAlu::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdAlu)
	DDX_CBString(pDX, IDC_OPERATOR, m_Method);
	DDX_CBString(pDX, IDC_CONST, m_Const);
	DDX_Check(pDX, IDC_CHECK1, m_Image1);
	DDX_Check(pDX, IDC_CHECK2, m_Image2);
	DDX_Check(pDX, IDC_CHECK3, m_Image3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdAlu, CFormView)
	//{{AFX_MSG_MAP(CCmdAlu)
	ON_BN_CLICKED(IDC_ACT_ALU, OnActAlu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdAlu diagnostics

#ifdef _DEBUG
void CCmdAlu::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdAlu::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdAlu message handlers
void CCmdAlu::OnActAlu() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	BOOL flag[3] = {m_Image1, m_Image2, m_Image3};
	face->IVSTKalu(image, NULL, m_Method, flag, (float)atof(m_Const));
}
