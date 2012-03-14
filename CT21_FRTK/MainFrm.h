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
//#include "MyCamView.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	enum ViewType{COLORCONVERT, FILTERING, FOURIER, PROJ, BLOB, COORD, GEO, STAT, TEMP, TH, VER, ALU, FMMNN, FNN, NN, EIGEN};
	void SwitchView(ViewType nType);
    CSplitterWnd *m_wndWhole;
	CSplitterWnd *m_wndDebugImage;
	CSplitterWnd *m_wndAddedView;
	CSplitterWnd *m_wndMemory;

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
	CImageBaseView* GetDebugImage(int num);
	CImageBaseView* GetMemoryImage(int num);
	CView* GetControlView();
	//CMyCamView* GetCameraView();

	CToolBar    m_wndToolBar;
	CStatusBar  m_wndStatusBar;

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColorConvert();
	afx_msg void OnFiltering();
	afx_msg void OnFouTx();
	afx_msg void OnTemplateMatching();
	afx_msg void OnStatFeat();
	afx_msg void OnProjExt();
	afx_msg void OnGeoFeat();
	afx_msg void OnGeoEst();
	afx_msg void OnFaceVer();
	afx_msg void OnCoordTx();
	afx_msg void OnCompTemplate();
	afx_msg void OnBlobAnal();
	afx_msg void OnPreTh();
	afx_msg void OnRawAlu();
	afx_msg void OnEigenface();
	afx_msg void OnFmmnnCls();
	afx_msg void OnFnnCls();
	afx_msg void OnNnCls();
	/*
	afx_msg void OnVideoFerec();
	afx_msg void OnVideoFacialfeature();
	afx_msg void OnVideoFacedetect();
	afx_msg void OnVideoCamformat();
	afx_msg void OnVideoCamonoff();
	afx_msg void OnVideoCamsetting();
	*/
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__ECE2AB31_1155_4ED3_850D_F8411BAF622D__INCLUDED_)
