#if !defined(AFX_AFFDLG_H__5F24BEA6_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_AFFDLG_H__5F24BEA6_32B5_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AffDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAffDlg dialog

class CAffDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CAffDlg)

// Construction
public:
	CAffDlg();
	~CAffDlg();

// Dialog Data
	//{{AFX_DATA(CAffDlg)
	enum { IDD = IDD_AFFINE1 };
	float	m_fDeg;
	float	m_fPx;
	float	m_fPy;
	float	m_fZx;
	float	m_fZy;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAffDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAffDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFFDLG_H__5F24BEA6_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
