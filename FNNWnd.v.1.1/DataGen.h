#if !defined(AFX_DATAGEN_H__EBB836F9_5BAF_4E73_84DA_F165E47BB3ED__INCLUDED_)
#define AFX_DATAGEN_H__EBB836F9_5BAF_4E73_84DA_F165E47BB3ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataGen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DataGen dialog

class DataGen : public CDialog
{
// Construction
public:
	DataGen(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DataGen)
	enum { IDD = IDD_DATA_GEN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DataGen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DataGen)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAGEN_H__EBB836F9_5BAF_4E73_84DA_F165E47BB3ED__INCLUDED_)
