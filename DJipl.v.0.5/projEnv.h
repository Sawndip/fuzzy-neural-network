#if !defined(AFX_PROJENV_H__B000E7DD_FED6_4E82_AEF0_79B30A4DFA79__INCLUDED_)
#define AFX_PROJENV_H__B000E7DD_FED6_4E82_AEF0_79B30A4DFA79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// projEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// projEnv dialog

class projEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(projEnv)

// Construction
public:
	projEnv();
	~projEnv();

// Dialog Data
	//{{AFX_DATA(projEnv)
	enum { IDD = IDD_PROJECTION };
	BOOL	weight;
	BOOL	xdir;
	BOOL	ydir;
	BOOL	count;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(projEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(projEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJENV_H__B000E7DD_FED6_4E82_AEF0_79B30A4DFA79__INCLUDED_)
