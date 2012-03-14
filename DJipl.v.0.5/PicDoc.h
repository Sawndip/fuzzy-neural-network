// ImageDoc.h : interface of the CPicDoc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __IMAGEDOC_H__
#define __IMAGEDOC_H__

#include "Image.h"
#include ".\\IPL\\background.h"
#include "histogram.h"
#include "cv.h"
#include "IplFace.h"

#define	NUM_BUFFER	(6)
//#define	_DEBUG_

class Hist1d;

class CPicDoc : public CDocument
{
protected: // create from serialization only
	CPicDoc();
	DECLARE_DYNCREATE(CPicDoc)

// Attributes
public:
	BOOL m_bRegionSelectMode;
	BOOL m_bProcessable;
	CRectTracker m_Tracker;		//영역 선택
	UINT		m_ProcessorMode;
	BOOL m_bViewBlob;
	BOOL m_bWhiteDetect;
	Hist1d m_Myhist;
	CString loadedfile;
	
	IplLUT *plut;
	IplLUT lut;
	IplROI *roi;
											
// Operations
public:
	CmyImage m_Image;
	//void	ProcessImage(LPBITMAPINFO lpBi);
	void	ProcessImage();
	
	BOOL m_Back;

	// image buffer for displaying
	IplImage *dispCPimg;	// CP image
	IplImage *dispLPimg;	// LP image
	IplImage *dispBGimg;	// background image
	IplImage *dispFCimg;	// face component image
	IplImage *backCPimg;	// background CP image
	IplImage *dispRBGimg;	// background CP image

	// image buffer for files
	IplImage *fileimg;

	// useless until now!
	Background bgImage;		// class for background difference

	// size of windows: CP(cartesian plane), LP(log-polar plane)
	CvSize CPsize;
	CvSize LPsize;

	// image buffer for processing
	IplImage *procCPCimg[NUM_BUFFER];
	IplImage *procCPGimg[NUM_BUFFER];
	IplImage *maskCPimg[NUM_BUFFER];
	IplImage *procLPCimg[NUM_BUFFER];
	IplImage *procLPGimg[NUM_BUFFER];
	IplImage *procCPFimg[NUM_BUFFER];
	IplImage *procLPFimg[NUM_BUFFER];
	IplImage *procCPSFimg[NUM_BUFFER];
	
	IplImage *xmap;
	IplImage *ymap;
	IplColorTwist *mytwist;

	CIplFace face;

	void AllocImage();
	void DeallocImage();
	void GetCenteredFace(IplImage *Simg, IplImage *Dimg, double *cx, double *cy, double *cr, CvRect *rect);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPicDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	/*
	BOOL ApplyFilter(LPBITMAPINFO lpBi);
	BOOL RunDetector(LPBITMAPINFO lpBi);
	BOOL BackGroundException(LPBITMAPINFO lpBi);
	*/
	BOOL FaceDetector();
	BOOL WhiteDetector();

// Generated message map functions
protected:
	//{{AFX_MSG(CPicDoc)
	afx_msg void OnEditCopy();
	afx_msg void OnBright();
	afx_msg void OnContrast();
	afx_msg void OnDark();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnContrastdown();
	afx_msg void OnTurnleft();
	afx_msg void OnTurnright();
	afx_msg void OnTurnvertical();
	afx_msg void OnTurnhorizontal();
	afx_msg void OnRegionselect();
	afx_msg void OnUpdateRegionselect(CCmdUI* pCmdUI);
	afx_msg void OnDuplicate();
	afx_msg void OnInvert();
	afx_msg void OnFaceLft();
	afx_msg void OnAdjustThreshold();
	afx_msg void OnFaceLp();
	afx_msg void OnFilterHlac();
	afx_msg void OnRotateAffine();
	afx_msg void OnFaceColor();
	afx_msg void OnFaceRcer();
	afx_msg void OnFilteringMorp();
	afx_msg void OnFiltProj();
	afx_msg void OnMomentFeature();
	afx_msg void OnFaceGabor();
	afx_msg void OnHough();
	afx_msg void OnFaceFind();
	afx_msg void OnFaceRlp();
	afx_msg void OnFaceLpcenter();
	afx_msg void OnNn();
	afx_msg void OnFaceFl();
	afx_msg void OnFaceColorSpec();
	afx_msg void OnDynamicOpt();
	afx_msg void OnFaceComp();
	afx_msg void OnFaceNn();
	afx_msg void OnFaceStudy();
	afx_msg void OnDynamicEdge();
	afx_msg void OnDynamicMotion();
	afx_msg void OnDynamicBackgnd();
	afx_msg void OnDynamicFace();
	afx_msg void OnDynamicWhite();
	afx_msg void OnDynamicView();
	afx_msg void OnViewBlob();
	afx_msg void OnViewWhite();
	afx_msg void OnDynamicGetback();
	afx_msg void OnFiltEdge();
	afx_msg void OnFiltHisto();
	afx_msg void OnFaceDtm();
	afx_msg void OnFaceCt214();
	afx_msg void OnEditJinwoo();
	afx_msg void OnHsTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CWait
{
protected:
	CPicDoc *m_pDoc;

public:
	CWait(CPicDoc *pDoc);
	virtual ~CWait();
};

/////////////////////////////////////////////////////////////////////////////
#endif
