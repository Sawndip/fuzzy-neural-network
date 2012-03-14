// ThresEnv.cpp : implementation file
//

#include "stdafx.h"
#include "Pic.h"
#include "ThresEnv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ThresEnv property page

IMPLEMENT_DYNCREATE(ThresEnv, CPropertyPage)

ThresEnv::ThresEnv() : CPropertyPage(ThresEnv::IDD)
{
	//{{AFX_DATA_INIT(ThresEnv)
	m_nRange = 0;
	m_otsup = 1.0f;
	//}}AFX_DATA_INIT

/*	m_slideMax.SetRange(0,255);
	m_slideMax.SetPos(128);
	m_slideMax.SetTicFreq(1);

	m_slideMin.SetRange(0,255);
	m_slideMin.SetPos(128);
	m_slideMin.SetTicFreq(1);*/

}

ThresEnv::~ThresEnv()
{
}

void ThresEnv::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ThresEnv)
	DDX_Control(pDX, IDC_EMIN, m_editMin);
	DDX_Control(pDX, IDC_EMAX, m_editMax);
	DDX_Control(pDX, IDC_MIN_TH, m_slideMin);
	DDX_Control(pDX, IDC_MAX_TH, m_slideMax);
	DDX_Radio(pDX, IDC_INSIDE, m_nRange);
	DDX_Text(pDX, IDC_OTSU_P, m_otsup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ThresEnv, CPropertyPage)
	//{{AFX_MSG_MAP(ThresEnv)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ThresEnv message handlers

void ThresEnv::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CString strTemp;

	m_nMaxPos = m_slideMax.GetPos();
	strTemp.Format("%d", m_nMaxPos);
	m_editMax.SetWindowText(strTemp);

	m_nMinPos = m_slideMin.GetPos();
	strTemp.Format("%d", m_nMinPos);
	m_editMin.SetWindowText(strTemp);

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL ThresEnv::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_slideMax.SetRange(0,255);
	m_slideMax.SetPos(128);
	m_slideMax.SetTicFreq(1);

	m_slideMin.SetRange(0,255);
	m_slideMin.SetPos(128);
	m_slideMin.SetTicFreq(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
