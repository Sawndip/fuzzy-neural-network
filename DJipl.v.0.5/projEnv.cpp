// projEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "projEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// projEnv property page

IMPLEMENT_DYNCREATE(projEnv, CPropertyPage)

projEnv::projEnv() : CPropertyPage(projEnv::IDD)
{
	//{{AFX_DATA_INIT(projEnv)
	weight = FALSE;
	xdir = FALSE;
	ydir = FALSE;
	count = FALSE;
	//}}AFX_DATA_INIT
}

projEnv::~projEnv()
{
}

void projEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(projEnv)
	DDX_Check(pDX, IDC_WEIGHT, weight);
	DDX_Check(pDX, IDC_XDIR, xdir);
	DDX_Check(pDX, IDC_YDIR, ydir);
	DDX_Check(pDX, IDC_WEIGHT2, count);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(projEnv, CPropertyPage)
	//{{AFX_MSG_MAP(projEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// projEnv message handlers
