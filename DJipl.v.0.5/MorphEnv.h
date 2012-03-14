#if !defined(AFX_MORPHENV_H__971B0660_3BE4_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_MORPHENV_H__971B0660_3BE4_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MorphEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MorphEnv dialog

class MorphEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(MorphEnv)

// Construction
public:
	MorphEnv();
	~MorphEnv();

// Dialog Data
	//{{AFX_DATA(MorphEnv)
	enum { IDD = IDD_MORPH };
	int		m_nMorph;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(MorphEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(MorphEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MORPHENV_H__971B0660_3BE4_11D4_B764_00E0294BE14D__INCLUDED_)
