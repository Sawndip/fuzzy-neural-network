// PicFrm.cpp : implementation of the CPicFrame class
//

#include "stdafx.h"
#include "Pic.h"

#include "PicFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicFrame

IMPLEMENT_DYNCREATE(CPicFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CPicFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CPicFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicFrame construction/destruction

CPicFrame::CPicFrame()
{
	// TODO: add member initialization code here
	
}

CPicFrame::~CPicFrame()
{
}

BOOL CPicFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPicFrame diagnostics

#ifdef _DEBUG
void CPicFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CPicFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPicFrame message handlers
