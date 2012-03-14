#if !defined(AFX_DATAGENDLG_H__5BAA91BB_1C78_4230_A9FF_6FD37AD6D20C__INCLUDED_)
#define AFX_DATAGENDLG_H__5BAA91BB_1C78_4230_A9FF_6FD37AD6D20C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataGenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DataGenDlg dialog

class DataGenDlg : public CDialog
{
// Construction
public:
	DataGenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DataGenDlg)
	enum { IDD = IDD_DATA_GEN_DLG };
	int		m_DataLength;
	float	m_DataStdv;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DataGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DataGenDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAGENDLG_H__5BAA91BB_1C78_4230_A9FF_6FD37AD6D20C__INCLUDED_)
