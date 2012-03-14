#if !defined(AFX_CMDTM_H__7E9D7CA3_A0A8_4B23_92C4_88BF1461A411__INCLUDED_)
#define AFX_CMDTM_H__7E9D7CA3_A0A8_4B23_92C4_88BF1461A411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmdTM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmdTM form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CCmdTM : public CFormView
{
protected:
	CCmdTM();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCmdTM)

// Form Data
public:
	//{{AFX_DATA(CCmdTM)
	enum { IDD = IDD_CMD_TEMPLATE_MATCHING };
	CSliderCtrl	m_MatchDegree;
	CString	m_method;
	CString	m_part;
	CString	m_MatchDegreeVal;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCmdTM)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCmdTM();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CCmdTM)
	afx_msg void OnTempMatch();
	afx_msg void OnReleasedcaptureMatchDegree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDTM_H__7E9D7CA3_A0A8_4B23_92C4_88BF1461A411__INCLUDED_)
