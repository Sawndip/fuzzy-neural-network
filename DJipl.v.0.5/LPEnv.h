#if !defined(AFX_LPENV_H__2C7D3F00_3B38_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_LPENV_H__2C7D3F00_3B38_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LPEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LPEnv dialog

class LPEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(LPEnv)

// Construction
public:
	LPEnv();
	~LPEnv();

// Dialog Data
	//{{AFX_DATA(LPEnv)
	enum { IDD = IDD_LP };
	int		m_nHoriz;
	int		m_nVert;
	int		m_nMethod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(LPEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(LPEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LPENV_H__2C7D3F00_3B38_11D4_B764_00E0294BE14D__INCLUDED_)
