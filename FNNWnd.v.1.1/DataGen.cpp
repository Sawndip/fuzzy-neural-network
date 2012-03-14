// DataGen.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "DataGen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DataGen dialog


DataGen::DataGen(CWnd* pParent /*=NULL*/)
	: CDialog(DataGen::IDD, pParent)
{
	//{{AFX_DATA_INIT(DataGen)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DataGen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DataGen)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DataGen, CDialog)
	//{{AFX_MSG_MAP(DataGen)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DataGen message handlers
