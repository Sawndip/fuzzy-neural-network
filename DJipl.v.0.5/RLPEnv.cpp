// RLPEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "RLPEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RLPEnv property page

IMPLEMENT_DYNCREATE(RLPEnv, CPropertyPage)

RLPEnv::RLPEnv() : CPropertyPage(RLPEnv::IDD)
{
	//{{AFX_DATA_INIT(RLPEnv)
	m_nH = 240;
	m_nW = 320;
	//}}AFX_DATA_INIT
}

RLPEnv::~RLPEnv()
{
}

void RLPEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RLPEnv)
	DDX_Text(pDX, IDC_H, m_nH);
	DDX_Text(pDX, IDC_W, m_nW);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RLPEnv, CPropertyPage)
	//{{AFX_MSG_MAP(RLPEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RLPEnv message handlers
