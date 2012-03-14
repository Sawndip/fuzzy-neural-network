#if !defined(AFX_FNNDIALOG_H__33E751A0_AA2A_4A8E_AFFD_69E6922F9824__INCLUDED_)
#define AFX_FNNDIALOG_H__33E751A0_AA2A_4A8E_AFFD_69E6922F9824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FNNdialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FNNdialog dialog

class FNNdialog : public CDialog
{
// Construction
public:
	FNNdialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FNNdialog)
	enum { IDD = IDD_FNN };
	int		m_epoch;
	float	m_error_criteria;
	float	m_learning_rate;
	BOOL	m_io_histogram;
	BOOL	m_rule_combination;
	float	m_SLP1;
	float	m_SLP2;
	CString	m_strNodes;
	BOOL	m_SaveRFR;
	BOOL	m_SaveSI;
	BOOL	m_KillNodes;
	BOOL	m_JustInputs;
	CString	m_strInputs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FNNdialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FNNdialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FNNDIALOG_H__33E751A0_AA2A_4A8E_AFFD_69E6922F9824__INCLUDED_)
