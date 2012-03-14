#if !defined(AFX_DGENDLG_H__0F29592B_F737_4439_B89C_FE39DF9FE158__INCLUDED_)
#define AFX_DGENDLG_H__0F29592B_F737_4439_B89C_FE39DF9FE158__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DGenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DGenDlg dialog

class DGenDlg : public CDialog
{
// Construction
public:
	DGenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DGenDlg)
	enum { IDD = IDD_DATA_GEN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DGenDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DGENDLG_H__0F29592B_F737_4439_B89C_FE39DF9FE158__INCLUDED_)
