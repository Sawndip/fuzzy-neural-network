// CmdFourier.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdFourier.h"

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
// CCmdFourier

IMPLEMENT_DYNCREATE(CCmdFourier, CFormView)

CCmdFourier::CCmdFourier()
	: CFormView(CCmdFourier::IDD)
{
	//{{AFX_DATA_INIT(CCmdFourier)
	m_Fourier = _T("");
	//}}AFX_DATA_INIT
}

CCmdFourier::~CCmdFourier()
{
}

void CCmdFourier::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdFourier)
	DDX_CBString(pDX, IDC_FOURIER, m_Fourier);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdFourier, CFormView)
	//{{AFX_MSG_MAP(CCmdFourier)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdFourier diagnostics

#ifdef _DEBUG
void CCmdFourier::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdFourier::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdFourier message handlers

void CCmdFourier::OnButton1() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	CString str = m_Fourier;

	face->FRTKfourier(image, NULL, str);	
}
