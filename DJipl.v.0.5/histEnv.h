#if !defined(AFX_HISTENV_H__CDADF691_4CAF_4CA1_81DC_D5831F916EB5__INCLUDED_)
#define AFX_HISTENV_H__CDADF691_4CAF_4CA1_81DC_D5831F916EB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// histEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// histEnv dialog

class histEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(histEnv)

// Construction
public:
	histEnv();
	~histEnv();

// Dialog Data
	//{{AFX_DATA(histEnv)
	enum { IDD = IDD_HIST };
	int		m_method;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(histEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(histEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTENV_H__CDADF691_4CAF_4CA1_81DC_D5831F916EB5__INCLUDED_)
