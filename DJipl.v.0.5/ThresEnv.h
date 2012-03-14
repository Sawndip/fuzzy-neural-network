#if !defined(AFX_THRESENV_H__5F24BEA9_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_THRESENV_H__5F24BEA9_32B5_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThresEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ThresEnv dialog

class ThresEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(ThresEnv)

// Construction
public:
	ThresEnv();
	~ThresEnv();

	int m_nMaxPos;
	int m_nMinPos;

	int GetMaxPos() { return m_nMaxPos;}
	int GetMinPos() { return m_nMinPos;}

// Dialog Data
	//{{AFX_DATA(ThresEnv)
	enum { IDD = IDD_TH2 };
	CEdit	m_editMin;
	CEdit	m_editMax;
	CSliderCtrl	m_slideMin;
	CSliderCtrl	m_slideMax;
	int		m_nRange;
	float	m_otsup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(ThresEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(ThresEnv)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THRESENV_H__5F24BEA9_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
