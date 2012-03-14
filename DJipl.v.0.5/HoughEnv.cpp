// HoughEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "HoughEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HoughEnv property page

IMPLEMENT_DYNCREATE(HoughEnv, CPropertyPage)

HoughEnv::HoughEnv() : CPropertyPage(HoughEnv::IDD)
{
	//{{AFX_DATA_INIT(HoughEnv)
	m_nTh = 0;
	m_strAng = _T("");
	//}}AFX_DATA_INIT
}

HoughEnv::~HoughEnv()
{
}

void HoughEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HoughEnv)
	DDX_Text(pDX, IDC_TH, m_nTh);
	DDX_Text(pDX, IDC_ANGLE, m_strAng);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HoughEnv, CPropertyPage)
	//{{AFX_MSG_MAP(HoughEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HoughEnv message handlers
