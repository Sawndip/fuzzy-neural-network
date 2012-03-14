#if !defined(AFX_EDGEENV_H__E2293E0E_B8AA_4823_B62A_68FA6D9E6EC7__INCLUDED_)
#define AFX_EDGEENV_H__E2293E0E_B8AA_4823_B62A_68FA6D9E6EC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// edgeEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// edgeEnv dialog

class edgeEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(edgeEnv)

// Construction
public:
	edgeEnv();
	~edgeEnv();

// Dialog Data
	//{{AFX_DATA(edgeEnv)
	enum { IDD = IDD_EDGE };
	int		m_edge;
	int		m_Ksize;
	int		m_p1;
	int		m_p2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(edgeEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(edgeEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDGEENV_H__E2293E0E_B8AA_4823_B62A_68FA6D9E6EC7__INCLUDED_)
