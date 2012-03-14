// ConvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "ConvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConvDlg property page

IMPLEMENT_DYNCREATE(CConvDlg, CPropertyPage)

CConvDlg::CConvDlg() : CPropertyPage(CConvDlg::IDD)
{
	//{{AFX_DATA_INIT(CConvDlg)
	m_nCS = 0;
	m_nAll = FALSE;
	m_nB = FALSE;
	m_nG = FALSE;
	m_nR = FALSE;
	//}}AFX_DATA_INIT
}

CConvDlg::~CConvDlg()
{
}

void CConvDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConvDlg)
	DDX_Radio(pDX, IDC_RGB, m_nCS);
	DDX_Check(pDX, IDC_ALL, m_nAll);
	DDX_Check(pDX, IDC_B, m_nB);
	DDX_Check(pDX, IDC_G, m_nG);
	DDX_Check(pDX, IDC_R, m_nR);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConvDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CConvDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConvDlg message handlers
