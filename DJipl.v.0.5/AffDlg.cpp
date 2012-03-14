// AffDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "AffDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAffDlg property page

IMPLEMENT_DYNCREATE(CAffDlg, CPropertyPage)

CAffDlg::CAffDlg() : CPropertyPage(CAffDlg::IDD)
{
	//{{AFX_DATA_INIT(CAffDlg)
	m_fDeg = 0.0f;
	m_fPx = 0.0f;
	m_fPy = 0.0f;
	m_fZx = 1.0f;
	m_fZy = 1.0f;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAffDlg::~CAffDlg()
{
}

void CAffDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAffDlg)
	DDX_Text(pDX, IDC_ANG, m_fDeg);
	DDX_Text(pDX, IDC_PX, m_fPx);
	DDX_Text(pDX, IDC_PY, m_fPy);
	DDX_Text(pDX, IDC_ZX, m_fZx);
	DDX_Text(pDX, IDC_ZY, m_fZy);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAffDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CAffDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAffDlg message handlers
