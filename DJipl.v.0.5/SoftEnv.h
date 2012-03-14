#if !defined(AFX_SOFTENV_H__A52CDC09_4B95_411B_9CC5_E3466AFADFB9__INCLUDED_)
#define AFX_SOFTENV_H__A52CDC09_4B95_411B_9CC5_E3466AFADFB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SoftEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SoftEnv dialog

class SoftEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(SoftEnv)

// Construction
public:
	SoftEnv();
	~SoftEnv();

// Dialog Data
	//{{AFX_DATA(SoftEnv)
	enum { IDD = IDD_SOFT_COMPUTING };
	int		m_Learn;
	int		m_Activation;
	float	m_rate;
	float	m_wgt;
	int		m_train;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SoftEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SoftEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTENV_H__A52CDC09_4B95_411B_9CC5_E3466AFADFB9__INCLUDED_)
