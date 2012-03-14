// DGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "DGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DGenDlg dialog


DGenDlg::DGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DGenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DGenDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DGenDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DGenDlg, CDialog)
	//{{AFX_MSG_MAP(DGenDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DGenDlg message handlers
