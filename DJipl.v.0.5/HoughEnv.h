#if !defined(AFX_HOUGHENV_H__9C578DE0_4726_11D4_B765_00E0294BE14D__INCLUDED_)
#define AFX_HOUGHENV_H__9C578DE0_4726_11D4_B765_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoughEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HoughEnv dialog

class HoughEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(HoughEnv)

// Construction
public:
	HoughEnv();
	~HoughEnv();

// Dialog Data
	//{{AFX_DATA(HoughEnv)
	enum { IDD = IDD_HOUGH };
	BYTE	m_nTh;
	CString	m_strAng;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(HoughEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(HoughEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOUGHENV_H__9C578DE0_4726_11D4_B765_00E0294BE14D__INCLUDED_)
