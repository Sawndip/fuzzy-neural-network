#if !defined(AFX_DATAGENERATOR_H__AA80AEE5_978A_4145_BDD7_0573C980D8AF__INCLUDED_)
#define AFX_DATAGENERATOR_H__AA80AEE5_978A_4145_BDD7_0573C980D8AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataGenerator.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataGenerator dialog

class CDataGenerator : public CDialog
{
// Construction
public:
	CDataGenerator(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataGenerator)
	enum { IDD = IDD_DATA_GEN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataGenerator)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataGenerator)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAGENERATOR_H__AA80AEE5_978A_4145_BDD7_0573C980D8AF__INCLUDED_)
