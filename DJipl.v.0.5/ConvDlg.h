#if !defined(AFX_CONVDLG_H__5F24BEA3_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
#define AFX_CONVDLG_H__5F24BEA3_32B5_11D4_B764_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConvDlg dialog

class CConvDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CConvDlg)

// Construction
public:
	CConvDlg();
	~CConvDlg();

// Dialog Data
	//{{AFX_DATA(CConvDlg)
	enum { IDD = IDD_COLOR1 };
	int		m_nCS;
	BOOL	m_nAll;
	BOOL	m_nB;
	BOOL	m_nG;
	BOOL	m_nR;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CConvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CConvDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVDLG_H__5F24BEA3_32B5_11D4_B764_00E0294BE14D__INCLUDED_)
