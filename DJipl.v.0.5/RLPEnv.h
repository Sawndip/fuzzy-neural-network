#if !defined(AFX_RLPENV_H__AE2A2480_417A_11D4_B766_00E0294BE14D__INCLUDED_)
#define AFX_RLPENV_H__AE2A2480_417A_11D4_B766_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RLPEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RLPEnv dialog

class RLPEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(RLPEnv)

// Construction
public:
	RLPEnv();
	~RLPEnv();

// Dialog Data
	//{{AFX_DATA(RLPEnv)
	enum { IDD = IDD_RLP };
	int		m_nH;
	int		m_nW;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(RLPEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(RLPEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RLPENV_H__AE2A2480_417A_11D4_B766_00E0294BE14D__INCLUDED_)
