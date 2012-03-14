// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__ECE2AB31_1155_4ED3_850D_F8411BAF622D__INCLUDED_)
#define AFX_MAINFRM_H__ECE2AB31_1155_4ED3_850D_F8411BAF622D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ImageBaseView.h"
#include "LogView.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	enum ViewType{LEARN, TEST};
	void SwitchView(ViewType nType);
    CSplitterWnd *m_wndWhole;
	CSplitterWnd *m_wndKernel;
	CSplitterWnd *m_wndCoeff;
	CSplitterWnd *m_wndControl;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CImageBaseView* GetImageBaseView();
	CLogView* GetDJMessage();
	CImageBaseView* GetKernelImage(int num);
	CImageBaseView* GetCoeffImage(int num);
	CView* GetControlView();

	CToolBar    m_wndToolBar;
	CStatusBar  m_wndStatusBar;

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWnnLearning();
	afx_msg void OnWnnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__ECE2AB31_1155_4ED3_850D_F8411BAF622D__INCLUDED_)
