// CmdGF.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdGF.h"

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
// CCmdGF

IMPLEMENT_DYNCREATE(CCmdGF, CFormView)

CCmdGF::CCmdGF()
	: CFormView(CCmdGF::IDD)
{
	//{{AFX_DATA_INIT(CCmdGF)
	m_source = _T("MOUTH");
	m_result = _T("MOUTH");
	//}}AFX_DATA_INIT
}

CCmdGF::~CCmdGF()
{
}

void CCmdGF::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdGF)
	DDX_CBString(pDX, IDC_GF_LOCAL, m_source);
	DDX_CBString(pDX, IDC_GF_GLOBAL, m_result);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdGF, CFormView)
	//{{AFX_MSG_MAP(CCmdGF)
	ON_BN_CLICKED(IDC_GF_EXT, OnGfExt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdGF diagnostics

#ifdef _DEBUG
void CCmdGF::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdGF::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdGF message handlers
void CCmdGF::OnGfExt() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	CFileDialog *pDlg = new CFileDialog( TRUE, NULL, NULL, OFN_READONLY, 
		"¾ó±¼¸ðµ¨ *.dat|*.dat|¸ðµçÆÄÀÏ *.*|*.*|", NULL );

	if (pDlg->DoModal())
	{
		CString filename = pDlg->GetFileName();

		if (filename != "")
		{
			CIVSTKface facecomp;
			facecomp.load(filename);

			face->IVSTKgeofeat(image, NULL, m_source, m_result, facecomp);
		}
	}
}