// CmdColor.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdColor.h"

#include "MainFrm.h"
#include "FRTK.h"
#include "SoftComputingWndDoc.h"
#include "highgui.h"
#include "MyCamView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdColor

IMPLEMENT_DYNCREATE(CCmdColor, CFormView)

CCmdColor::CCmdColor()
	: CFormView(CCmdColor::IDD)
{
	//{{AFX_DATA_INIT(CCmdColor)
	m_source = _T("RGB");
	m_result = _T("HLS");
	//}}AFX_DATA_INIT
}

CCmdColor::~CCmdColor()
{
}

void CCmdColor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdColor)
	DDX_CBString(pDX, IDC_SOURCE, m_source);
	DDX_CBString(pDX, IDC_RESULT, m_result);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdColor, CFormView)
	//{{AFX_MSG_MAP(CCmdColor)
	ON_BN_CLICKED(IDC_ACT_CONVERT, OnActConvert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdColor diagnostics

#ifdef _DEBUG
void CCmdColor::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdColor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdColor message handlers

void CCmdColor::OnActConvert() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	//CMyCamView* camview = (CMyCamView*)((CSoftComputingWndDoc*)main->GetCameraView());
	//IPLIMAGE image = (camview->Camera()).GetFrame().GetImage();

	CString str = m_source + '2' + m_result;

	//face.FRTKrgb2hls(image, NULL);
	face->FRTKcolorconvert(image, NULL, str);
}
