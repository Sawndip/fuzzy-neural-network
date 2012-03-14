// histEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "histEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// histEnv property page

IMPLEMENT_DYNCREATE(histEnv, CPropertyPage)

histEnv::histEnv() : CPropertyPage(histEnv::IDD)
{
	//{{AFX_DATA_INIT(histEnv)
	m_method = -1;
	//}}AFX_DATA_INIT
}

histEnv::~histEnv()
{
}

void histEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(histEnv)
	DDX_Radio(pDX, IDC_RADIO1, m_method);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(histEnv, CPropertyPage)
	//{{AFX_MSG_MAP(histEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// histEnv message handlers
