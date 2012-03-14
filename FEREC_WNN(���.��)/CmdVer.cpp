// CmdVer.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdVer.h"

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
// CCmdVer

IMPLEMENT_DYNCREATE(CCmdVer, CFormView)

CCmdVer::CCmdVer()
	: CFormView(CCmdVer::IDD)
{
	//{{AFX_DATA_INIT(CCmdVer)
	m_RightEyeb = 0.2f;
	m_Nose = 0.15f;
	m_Mouth = 0.25f;
	m_LeftEyeb = 0.2f;
	m_Jaw = 0.2f;
	m_InterNm = 0.13f;
	m_InterDist = 0.13f;
	m_ForeHead = 0.2f;
	//}}AFX_DATA_INIT
}

CCmdVer::~CCmdVer()
{
}

void CCmdVer::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdVer)
	DDX_Control(pDX, IDC_FACE_VER, m_FaceBar);
	DDX_Text(pDX, IDC_RIGHT_EYEB, m_RightEyeb);
	DDX_Text(pDX, IDC_NOSE, m_Nose);
	DDX_Text(pDX, IDC_MOUTH, m_Mouth);
	DDX_Text(pDX, IDC_LEFT_EYEB, m_LeftEyeb);
	DDX_Text(pDX, IDC_JAW, m_Jaw);
	DDX_Text(pDX, IDC_INTERNM, m_InterNm);
	DDX_Text(pDX, IDC_INTERDIST, m_InterDist);
	DDX_Text(pDX, IDC_FOREHEAD, m_ForeHead);
	DDX_Text(pDX, IDC_FACE_VER_VAL, m_FaceVer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdVer, CFormView)
	//{{AFX_MSG_MAP(CCmdVer)
	ON_BN_CLICKED(IDC_ACT_VER, OnActVer)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_FACE_VER, OnReleasedcaptureFaceVer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdVer diagnostics

#ifdef _DEBUG
void CCmdVer::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdVer::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdVer message handlers
void CCmdVer::OnActVer() 
{
	UpdateData(TRUE);

	CMainFrame* main = (CMainFrame*)AfxGetMainWnd();

	CFRTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	//face->FRTKverify(image, NULL, ...);		//20030429
}

void CCmdVer::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_FaceBar.SetRange(0, 255);

	UpdateData(FALSE);
}

void CCmdVer::OnReleasedcaptureFaceVer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_FaceVer.Format("%.2f", (float)m_FaceBar.GetPos()/128.0f);

	*pResult = 0;

	UpdateData(FALSE);
}
