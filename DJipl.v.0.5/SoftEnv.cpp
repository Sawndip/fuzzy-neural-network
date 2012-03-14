// SoftEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "SoftEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoftEnv property page

IMPLEMENT_DYNCREATE(SoftEnv, CPropertyPage)

SoftEnv::SoftEnv() : CPropertyPage(SoftEnv::IDD)
{
	//{{AFX_DATA_INIT(SoftEnv)
	m_Learn = -1;
	m_Activation = -1;
	m_rate = 0.0f;
	m_wgt = 0.0f;
	m_train = 0;
	//}}AFX_DATA_INIT
}

SoftEnv::~SoftEnv()
{
}

void SoftEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SoftEnv)
	DDX_Radio(pDX, IDC_RADIO1, m_Learn);
	DDX_Radio(pDX, IDC_RADIO9, m_Activation);
	DDX_Text(pDX, IDC_EDIT1, m_rate);
	DDX_Text(pDX, IDC_EDIT2, m_wgt);
	DDX_Text(pDX, IDC_EDIT3, m_train);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SoftEnv, CPropertyPage)
	//{{AFX_MSG_MAP(SoftEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SoftEnv message handlers
