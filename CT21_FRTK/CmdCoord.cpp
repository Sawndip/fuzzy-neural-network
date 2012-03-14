// CmdCoord.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdCoord.h"

#include "MainFrm.h"
#include "FRTK.h"
#include "SoftComputingWndDoc.h"
#include "highgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdCoord

IMPLEMENT_DYNCREATE(CCmdCoord, CFormView)

CCmdCoord::CCmdCoord()
	: CFormView(CCmdCoord::IDD)
{
	//{{AFX_DATA_INIT(CCmdCoord)
	m_source = _T("CARTESIAN");
	m_result = _T("LOGPOLAR");
	//}}AFX_DATA_INIT
}

CCmdCoord::~CCmdCoord()
{
}

void CCmdCoord::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdCoord)
	DDX_CBString(pDX, IDC_SRC_COORD, m_source);
	DDX_CBString(pDX, IDC_TAR_COORD, m_result);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdCoord, CFormView)
	//{{AFX_MSG_MAP(CCmdCoord)
	ON_BN_CLICKED(IDC_COORD_TX, OnCoordTx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdCoord diagnostics

#ifdef _DEBUG
void CCmdCoord::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdCoord::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdCoord message handlers
//IDC_COORD_TX
void CCmdCoord::OnCoordTx() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	CString str = m_source + '2' + m_result;

	face->FRTKcoordtx(image, NULL, str);
}