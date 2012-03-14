#if !defined(AFX_CMDTH_H__6CB7DF23_EFC0_4AA6_A94D_94FFAE8348FB__INCLUDED_)
#define AFX_CMDTH_H__6CB7DF23_EFC0_4AA6_A94D_94FFAE8348FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdTh.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdTh form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdTh : public CFormView
{
protected:
	CCmdTh();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdTh)

// Form Data
public:
	//{{AFX_DATA(CCmdTh)
	enum { IDD = IDD_CMD_THRESHOLD };
	CString m_Method;
	CString m_MaxThr;
	CString m_MinThr;
	CSliderCtrl	m_MaxTh;
	CSliderCtrl	m_MinTh;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdTh)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdTh();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdTh)
	afx_msg void OnActTh();
	afx_msg void OnReleasedcaptureMinTh(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureMaxTh(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDTH_H__6CB7DF23_EFC0_4AA6_A94D_94FFAE8348FB__INCLUDED_)
