// DJMessage.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "DJMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DJMessage

IMPLEMENT_DYNCREATE(DJMessage, CEditView)

DJMessage::DJMessage()
{
}

DJMessage::~DJMessage()
{
}


BEGIN_MESSAGE_MAP(DJMessage, CEditView)
	//{{AFX_MSG_MAP(DJMessage)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DJMessage drawing

void DJMessage::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// DJMessage diagnostics

#ifdef _DEBUG
void DJMessage::AssertValid() const
{
	CEditView::AssertValid();
}

void DJMessage::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// DJMessage message handlers
void DJMessage::MessageView(CString str)
{
	GetEditCtrl().SetWindowText(str);
}
