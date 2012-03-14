// DataGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "DataGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DataGenDlg dialog


DataGenDlg::DataGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DataGenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DataGenDlg)
	m_DataLength = 100;
	m_DataStdv = 0.05f;
	//}}AFX_DATA_INIT
}


void DataGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DataGenDlg)
	DDX_Text(pDX, IDC_DATA_LENGTH, m_DataLength);
	DDX_Text(pDX, IDC_DATA_STDV, m_DataStdv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DataGenDlg, CDialog)
	//{{AFX_MSG_MAP(DataGenDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DataGenDlg message handlers
