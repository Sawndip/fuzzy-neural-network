// GaborEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "GaborEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GaborEnv property page

IMPLEMENT_DYNCREATE(GaborEnv, CPropertyPage)

GaborEnv::GaborEnv() : CPropertyPage(GaborEnv::IDD)
{
	//{{AFX_DATA_INIT(GaborEnv)
	m_dTheta = 0.0;
	m_dWave = 0.0;
// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

GaborEnv::~GaborEnv()
{
}

void GaborEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GaborEnv)
	DDX_Text(pDX, IDC_THETA, m_dTheta);
	DDX_Text(pDX, IDC_WAVE, m_dWave);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GaborEnv, CPropertyPage)
	//{{AFX_MSG_MAP(GaborEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GaborEnv message handlers
