// histoEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "histoEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// histoEnv property page

IMPLEMENT_DYNCREATE(histoEnv, CPropertyPage)

histoEnv::histoEnv() : CPropertyPage(histoEnv::IDD)
{
	//{{AFX_DATA_INIT(histoEnv)
	//}}AFX_DATA_INIT
}

histoEnv::~histoEnv()
{
}

void histoEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(histoEnv)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(histoEnv, CPropertyPage)
	//{{AFX_MSG_MAP(histoEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// histoEnv message handlers
