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
	enum ViewType{COLORCONVERT, FILTERING, FOURIER, PROJ, BLOB, COORD, GEO, STAT, TEMP, TH, VER, ALU, FMMNN, FNN, NN, EIGEN, MOTION, DEPTH};
	void SwitchView(ViewType nType);
    
	CSplitterWnd* m_wndSplit[6];

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

	CImageBaseView* GetImageBaseView(int num = 0);
	CLogView* GetDJMessage();
	CImageBaseView* GetDebugImage(int num);
	CImageBaseView* GetMemoryImage(int num);
	CView* GetControlView();

	CToolBar    m_wndToolBar;
	CStatusBar  m_wndStatusBar;

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFaceVer();
	afx_msg void OnBlob();
	afx_msg void OnColor();
	afx_msg void OnCoord();
	afx_msg void OnDepth();
	afx_msg void OnEdge();
	afx_msg void OnFerec();
	afx_msg void OnFmmnn();
	afx_msg void OnFnn();
	afx_msg void OnMotion();
	afx_msg void OnNn();
	afx_msg void OnOcclusion();
	afx_msg void OnPursuit();
	afx_msg void OnVergence();
	afx_msg void OnAlu();
	afx_msg void OnTh();
	afx_msg void OnProj();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__ECE2AB31_1155_4ED3_850D_F8411BAF622D__INCLUDED_)
