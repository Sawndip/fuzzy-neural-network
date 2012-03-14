#if !defined(AFX_GABORENV_H__5F24BEA8_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_GABORENV_H__5F24BEA8_32B5_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GaborEnv.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GaborEnv dialog

class GaborEnv : public CPropertyPage
{
	DECLARE_DYNCREATE(GaborEnv)

// Construction
public:
	GaborEnv();
	~GaborEnv();

// Dialog Data
	//{{AFX_DATA(GaborEnv)
	enum { IDD = IDD_GABOR2 };
	double	m_dTheta;
	double	m_dWave;
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(GaborEnv)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(GaborEnv)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GABORENV_H__5F24BEA8_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
