// FNNdialog.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "FNNdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FNNdialog dialog


FNNdialog::FNNdialog(CWnd* pParent /*=NULL*/)
	: CDialog(FNNdialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(FNNdialog)
	m_epoch = 0;
	m_error_criteria = 0.0f;
	m_learning_rate = 0.0f;
	m_io_histogram = FALSE;
	m_rule_combination = FALSE;
	m_SLP1 = 0.0f;
	m_SLP2 = 0.0f;
	m_strNodes = _T("");
	m_SaveRFR = FALSE;
	m_SaveSI = FALSE;
	m_KillNodes = FALSE;
	m_JustInputs = FALSE;
	m_strInputs = _T("");
	//}}AFX_DATA_INIT
}


void FNNdialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FNNdialog)
	DDX_Text(pDX, IDC_EPOCH, m_epoch);
	DDX_Text(pDX, IDC_ERROR_CRITERIA, m_error_criteria);
	DDX_Text(pDX, IDC_LEARNING_RATE, m_learning_rate);
	DDX_Check(pDX, IDC_IO_HISTOGRAM, m_io_histogram);
	DDX_Check(pDX, IDC_RULE_COMBINATION, m_rule_combination);
	DDX_Text(pDX, IDC_SLP_1, m_SLP1);
	DDX_Text(pDX, IDC_SLP_2, m_SLP2);
	DDX_Text(pDX, IDC_KILL_NODES, m_strNodes);
	DDX_Check(pDX, IDC_SAVE_RFR, m_SaveRFR);
	DDX_Check(pDX, IDC_SAVE_SI, m_SaveSI);
	DDX_Check(pDX, IDC_KILL_NODE, m_KillNodes);
	DDX_Check(pDX, IDC_JUST_INPUT, m_JustInputs);
	DDX_Text(pDX, IDC_JUST_INPUTS, m_strInputs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FNNdialog, CDialog)
	//{{AFX_MSG_MAP(FNNdialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FNNdialog message handlers
