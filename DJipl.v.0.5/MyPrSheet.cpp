// MyPrSheet.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "MyPrSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPrSheet

IMPLEMENT_DYNAMIC(CMyPrSheet, CPropertySheet)

CMyPrSheet::CMyPrSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMyPrSheet::CMyPrSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	//1
	AddPage(&m_thr);
	AddPage(&m_conv);
	AddPage(&m_gabor);
	AddPage(&m_affine);
	AddPage(&m_lp);
	//6
	AddPage(&m_morph);
	AddPage(&m_hough);
	AddPage(&m_HLAC);
	AddPage(&m_rlp);
	AddPage(&m_face);
	//11
	AddPage(&m_soft);
	AddPage(&m_proj);
	AddPage(&m_edge);
	AddPage(&m_hist);
}

CMyPrSheet::~CMyPrSheet()
{
}


BEGIN_MESSAGE_MAP(CMyPrSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMyPrSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPrSheet message handlers
