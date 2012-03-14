// FaceEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "FaceEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFaceEnv property page

IMPLEMENT_DYNCREATE(CFaceEnv, CPropertyPage)

CFaceEnv::CFaceEnv() : CPropertyPage(CFaceEnv::IDD)
{
	//{{AFX_DATA_INIT(CFaceEnv)
	m_nMethod = 0;
	m_dmg = 0.0;
	m_dmr = 0.0;
	m_dsrr = 0.0;
	m_dsgg = 0.0;
	m_nidx1 = FALSE;
	m_nidx2 = FALSE;
	m_nidx3 = FALSE;
	m_dth = 0.0;
	m_dWave = 0.0;
	//}}AFX_DATA_INIT
}

CFaceEnv::~CFaceEnv()
{
}

void CFaceEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFaceEnv)
	DDX_Radio(pDX, IDC_RADIO1, m_nMethod);
	DDX_Text(pDX, IDC_MG, m_dmg);
	DDX_Text(pDX, IDC_MR, m_dmr);
	DDX_Text(pDX, IDC_SRR, m_dsrr);
	DDX_Text(pDX, IDC_SGG, m_dsgg);
	DDX_Check(pDX, IDC_R, m_nidx1);
	DDX_Check(pDX, IDC_G, m_nidx2);
	DDX_Check(pDX, IDC_B, m_nidx3);
	DDX_Text(pDX, IDC_TH, m_dth);
	DDX_Text(pDX, IDC_WAVE2, m_dWave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFaceEnv, CPropertyPage)
	//{{AFX_MSG_MAP(CFaceEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFaceEnv message handlers
