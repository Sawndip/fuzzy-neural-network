// edgeEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "edgeEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// edgeEnv property page

IMPLEMENT_DYNCREATE(edgeEnv, CPropertyPage)

edgeEnv::edgeEnv() : CPropertyPage(edgeEnv::IDD)
{
	//{{AFX_DATA_INIT(edgeEnv)
	m_edge = -1;
	m_Ksize = 0;
	m_p1 = 0;
	m_p2 = 0;
	//}}AFX_DATA_INIT
}

edgeEnv::~edgeEnv()
{
}

void edgeEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(edgeEnv)
	DDX_Radio(pDX, IDC_RADIO1, m_edge);
	DDX_Text(pDX, IDC_KERNEL, m_Ksize);
	DDX_Text(pDX, IDC_P1, m_p1);
	DDX_Text(pDX, IDC_P2, m_p2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(edgeEnv, CPropertyPage)
	//{{AFX_MSG_MAP(edgeEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// edgeEnv message handlers
