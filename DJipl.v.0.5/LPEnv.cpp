// LPEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "LPEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LPEnv property page

IMPLEMENT_DYNCREATE(LPEnv, CPropertyPage)

LPEnv::LPEnv() : CPropertyPage(LPEnv::IDD)
{
	//{{AFX_DATA_INIT(LPEnv)
	m_nHoriz = 64;
	m_nVert = 128;
	m_nMethod = 0;
	//}}AFX_DATA_INIT
}

LPEnv::~LPEnv()
{
}

void LPEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LPEnv)
	DDX_Text(pDX, IDC_UH, m_nHoriz);
	DDX_Text(pDX, IDC_VV, m_nVert);
	DDX_Text(pDX, IDC_METHOD, m_nMethod);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LPEnv, CPropertyPage)
	//{{AFX_MSG_MAP(LPEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LPEnv message handlers
