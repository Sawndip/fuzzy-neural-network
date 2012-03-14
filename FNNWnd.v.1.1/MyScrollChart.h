#if !defined(AFX_MYSCROLLCHART_H__A85CEEF7_D56B_4787_8612_E887C4BA77DA__INCLUDED_)
#define AFX_MYSCROLLCHART_H__A85CEEF7_D56B_4787_8612_E887C4BA77DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyScrollChart.h : header file
//

#include "chart.h"

/////////////////////////////////////////////////////////////////////////////
// CMyScrollChart view

class CMyScrollChart : public CView
{
protected:
	CMyScrollChart();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyScrollChart)

// Attributes
public:
	CScrollChart<float> mySChart;

	CStatic m_TextHozMax;
	CStatic m_TextHozMin;
	CStatic m_TextVerMax;
	CStatic m_TextVerMin;
	CFont fontText;

// Operations
public:
	void SetScrollChart(int method = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyScrollChart)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyScrollChart();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyScrollChart)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnNewLimits(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSCROLLCHART_H__A85CEEF7_D56B_4787_8612_E887C4BA77DA__INCLUDED_)
