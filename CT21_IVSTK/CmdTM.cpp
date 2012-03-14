// CmdTM.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdTM.h"

#include "MainFrm.h"
#include "IVSTK.h"
#include "SoftComputingWndDoc.h"
#include "highgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdTM

IMPLEMENT_DYNCREATE(CCmdTM, CFormView)

CCmdTM::CCmdTM()
	: CFormView(CCmdTM::IDD)
{
	//{{AFX_DATA_INIT(CCmdTM)
	m_method = _T("SQDIFF");
	m_part = _T("FACE");
	m_MatchDegreeVal = _T("");
	//}}AFX_DATA_INIT
}

CCmdTM::~CCmdTM()
{
}

void CCmdTM::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdTM)
	DDX_Control(pDX, IDC_MATCH_DEGREE, m_MatchDegree);
	DDX_CBString(pDX, IDC_TM_METHOD2, m_method);
	DDX_CBString(pDX, IDC_TM_METHOD, m_part);
	DDX_Text(pDX, IDC_MATCH_DEGREE_VAL, m_MatchDegreeVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdTM, CFormView)
	//{{AFX_MSG_MAP(CCmdTM)
	ON_BN_CLICKED(IDC_TEMP_MATCH, OnTempMatch)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_MATCH_DEGREE, OnReleasedcaptureMatchDegree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdTM diagnostics

#ifdef _DEBUG
void CCmdTM::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdTM::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdTM message handlers
void CCmdTM::OnTempMatch() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;

	int degree = (int)(255.0*atof((const char*)m_MatchDegreeVal));

	face->IVSTKtempmatch(image, NULL, m_method, m_part, degree);
}

void CCmdTM::OnReleasedcaptureMatchDegree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_MatchDegreeVal.Format("%.2f", ((float)m_MatchDegree.GetPos()/255.0f));

	*pResult = 0;

	UpdateData(FALSE);
}

void CCmdTM::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	m_MatchDegree.SetRange(0, 255);

	UpdateData(FALSE);	
}
