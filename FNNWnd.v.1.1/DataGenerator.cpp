// DataGenerator.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "DataGenerator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataGenerator dialog


CDataGenerator::CDataGenerator(CWnd* pParent /*=NULL*/)
	: CDialog(CDataGenerator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDataGenerator)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDataGenerator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataGenerator)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataGenerator, CDialog)
	//{{AFX_MSG_MAP(CDataGenerator)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataGenerator message handlers
