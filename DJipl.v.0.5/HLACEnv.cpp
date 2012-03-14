// HLACEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "HLACEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HLACEnv property page

IMPLEMENT_DYNCREATE(HLACEnv, CPropertyPage)

HLACEnv::HLACEnv() : CPropertyPage(HLACEnv::IDD)
{
	//{{AFX_DATA_INIT(HLACEnv)
	m_nLPu = 60;
	m_nLPv = 90;
	m_nXf = 1;
	m_nYf = 1;
	m_nFlag = 0;
	m_nNf = 1;
	m_nNnf = 1;
	//}}AFX_DATA_INIT
}

HLACEnv::~HLACEnv()
{
}

void HLACEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HLACEnv)
	DDX_Text(pDX, IDC_U, m_nLPu);
	DDX_Text(pDX, IDC_V, m_nLPv);
	DDX_Text(pDX, IDC_XF, m_nXf);
	DDX_Text(pDX, IDC_YF, m_nYf);
	DDX_Radio(pDX, IDC_RADIO1, m_nFlag);
	DDX_Text(pDX, IDC_NF, m_nNf);
	DDX_Text(pDX, IDC_NNF, m_nNnf);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HLACEnv, CPropertyPage)
	//{{AFX_MSG_MAP(HLACEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HLACEnv message handlers
