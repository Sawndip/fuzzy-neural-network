#if !defined(AFX_HLACENV_H__4A278A60_56BD_11D4_B765_00E0294BE14D__INCLUDED_)
#define AFX_HLACENV_H__4A278A60_56BD_11D4_B765_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HLACEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HLACEnv dialog

class HLACEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(HLACEnv)

// Construction
public:
	HLACEnv();
	~HLACEnv();

// Dialog Data
	//{{AFX_DATA(HLACEnv)
	enum { IDD = IDD_HLAC };
	int		m_nLPu;
	int		m_nLPv;
	int		m_nXf;
	int		m_nYf;
	int		m_nFlag;
	int		m_nNf;
	int		m_nNnf;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(HLACEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(HLACEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HLACENV_H__4A278A60_56BD_11D4_B765_00E0294BE14D__INCLUDED_)
