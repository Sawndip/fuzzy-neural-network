#if !defined(AFX_FACEENV_H__509B72E0_9CB7_11D4_B766_00E0294BE14D__INCLUDED_)
#define AFX_FACEENV_H__509B72E0_9CB7_11D4_B766_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FaceEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFaceEnv dialog

class CFaceEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(CFaceEnv)

// Construction
public:
	CFaceEnv();
	~CFaceEnv();

// Dialog Data
	//{{AFX_DATA(CFaceEnv)
	enum { IDD = IDD_FACE_RECOGNITION };
	int		m_nMethod;
	double	m_dmg;
	double	m_dmr;
	double	m_dsrr;
	double	m_dsgg;
	BOOL	m_nidx1;
	BOOL	m_nidx2;
	BOOL	m_nidx3;
	double	m_dth;
	double	m_dWave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFaceEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFaceEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACEENV_H__509B72E0_9CB7_11D4_B766_00E0294BE14D__INCLUDED_)
