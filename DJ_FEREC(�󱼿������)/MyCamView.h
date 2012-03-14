#if !defined(AFX_MYCAMVIEW_H__EE9CBFA5_1381_4913_B644_2742FF75C7D1__INCLUDED_)
#define AFX_MYCAMVIEW_H__EE9CBFA5_1381_4913_B644_2742FF75C7D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyCamView.h : header file
//

#pragma comment(lib,"vfw32.lib")

#include ".\\Common\\Camera.h"

/////////////////////////////////////////////////////////////////////////////
// CMyCamView view

class CMyCamView : public CView
{
protected:
	CMyCamView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyCamView)

// Attributes
public:
	CCamera& Camera() { return m_camera; }

	void CameraOn();
	void CameraOff();
	void CameraFormat();
	void CameraSetting();
	//void Grab();

	CEdit m_pEdit;
	CButton m_pButton;

protected:
	CCamera m_camera;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyCamView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyCamView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyCamView)
	afx_msg void OnDestroy();
	afx_msg void OnButtonClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCAMVIEW_H__EE9CBFA5_1381_4913_B644_2742FF75C7D1__INCLUDED_)
