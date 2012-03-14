#if !defined(AFX_HISTOENV_H__DD2BB862_FBAA_4F01_9E29_B0FDDB0125F2__INCLUDED_)
#define AFX_HISTOENV_H__DD2BB862_FBAA_4F01_9E29_B0FDDB0125F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// histoEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// histoEnv dialog

class histoEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(histoEnv)

// Construction
public:
	histoEnv();
	~histoEnv();

// Dialog Data
	//{{AFX_DATA(histoEnv)
	enum { IDD = IDD_DIALOG1 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(histoEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(histoEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTOENV_H__DD2BB862_FBAA_4F01_9E29_B0FDDB0125F2__INCLUDED_)
