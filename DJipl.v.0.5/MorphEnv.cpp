// MorphEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "MorphEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MorphEnv property page

IMPLEMENT_DYNCREATE(MorphEnv, CPropertyPage)

MorphEnv::MorphEnv() : CPropertyPage(MorphEnv::IDD)
{
	//{{AFX_DATA_INIT(MorphEnv)
	m_nMorph = 2;
	//}}AFX_DATA_INIT
}

MorphEnv::~MorphEnv()
{
}

void MorphEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MorphEnv)
	DDX_Radio(pDX, IDC_RADIO1, m_nMorph);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MorphEnv, CPropertyPage)
	//{{AFX_MSG_MAP(MorphEnv)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MorphEnv message handlers
