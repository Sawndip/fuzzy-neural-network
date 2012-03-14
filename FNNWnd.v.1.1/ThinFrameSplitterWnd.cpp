// ThinFrameSplitterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "ThinFrameSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ThinFrameSplitterWnd

IMPLEMENT_DYNCREATE(ThinFrameSplitterWnd, CMDIChildWnd)

ThinFrameSplitterWnd::ThinFrameSplitterWnd()
{
	m_cxSplitter=PANE_BORDER;     // = 1
	m_cySplitter=PANE_BORDER;     // = 1
	m_cxBorderShare=PANE_SHARE;   // = 0
	m_cyBorderShare=PANE_SHARE;   // = 0
	m_cxSplitterGap=PANE_GAP;     // = 1
	m_cySplitterGap=PANE_GAP;     // = 1
}

ThinFrameSplitterWnd::~ThinFrameSplitterWnd()
{
}

BOOL ThinFrameSplitterWnd::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,       // TODO: adjust the number of rows, columns
		CSize(10, 10),  // TODO: adjust the minimum pane size
		pContext);
}


BEGIN_MESSAGE_MAP(ThinFrameSplitterWnd, CMDIChildWnd)
	//{{AFX_MSG_MAP(ThinFrameSplitterWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ThinFrameSplitterWnd message handlers
