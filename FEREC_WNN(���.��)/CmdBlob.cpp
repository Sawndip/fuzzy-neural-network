// CmdBlob.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdBlob.h"

#include "MainFrm.h"
#include "FRTK.h"
#include "SoftComputingWndDoc.h"
//#include "highgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdBlob

IMPLEMENT_DYNCREATE(CCmdBlob, CFormView)

CCmdBlob::CCmdBlob()
	: CFormView(CCmdBlob::IDD)
{
	//{{AFX_DATA_INIT(CCmdBlob)
	m_Method = _T("MAX_BLOB");
	m_Morph = _T("NONE");
	m_MorphNum = 1;
	//}}AFX_DATA_INIT
}

CCmdBlob::~CCmdBlob()
{
}

void CCmdBlob::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdBlob)
	DDX_CBString(pDX, IDC_BLOB_METHOD, m_Method);
	DDX_CBString(pDX, IDC_MORPH, m_Morph);
	DDX_Text(pDX, IDC_MORPH_NUM, m_MorphNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdBlob, CFormView)
	//{{AFX_MSG_MAP(CCmdBlob)
	ON_BN_CLICKED(IDC_BLOB_ANAL, OnBlobAnal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdBlob diagnostics

#ifdef _DEBUG
void CCmdBlob::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdBlob::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdBlob message handlers

void CCmdBlob::OnBlobAnal() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	face->FRTKblobanal(image, NULL, m_Method, m_Morph, m_MorphNum);
}