// CmdFilter.cpp : implementation file
//

#include "stdafx.h"
#include "SoftComputingWnd.h"
#include "CmdFilter.h"

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
// CCmdFilter

IMPLEMENT_DYNCREATE(CCmdFilter, CFormView)

CCmdFilter::CCmdFilter()
	: CFormView(CCmdFilter::IDD)
{
	//{{AFX_DATA_INIT(CCmdFilter)
	m_Filtering = _T("CANNY_3x3");
	//}}AFX_DATA_INIT
}

CCmdFilter::~CCmdFilter()
{
}

void CCmdFilter::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdFilter)
	DDX_CBString(pDX, IDC_FILTERING_METHOD, m_Filtering);
	DDX_Control(pDX, IDC_LOW_TH, m_MinTh);
	DDX_Control(pDX, IDC_HIGH_TH, m_MaxTh);
	DDX_Control(pDX, IDC_GABOR_ANGLE, m_GaborAngle);
	DDX_Control(pDX, IDC_GABOR_WAVE, m_GaborWave);
	DDX_Text(pDX, IDC_LOW_TH_VAL, m_MinThr);
	DDX_Text(pDX, IDC_HIGH_TH_VAL, m_MaxThr);
	DDX_Text(pDX, IDC_GABOR_ANGLE_VAL, m_GaborAngleVal);
	DDX_Text(pDX, IDC_GABOR_WAVE_VAL, m_GaborWaveVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdFilter, CFormView)
	//{{AFX_MSG_MAP(CCmdFilter)
	ON_BN_CLICKED(IDC_ACT_FILTERING, OnActFiltering)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_LOW_TH, OnReleasedcaptureMinTh)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_HIGH_TH, OnReleasedcaptureMaxTh)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_GABOR_ANGLE, OnReleasedcaptureGaborAngle)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_GABOR_WAVE, OnReleasedcaptureGaborWave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdFilter diagnostics

#ifdef _DEBUG
void CCmdFilter::AssertValid() const
{
	CFormView::AssertValid();
}

void CCmdFilter::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCmdFilter message handlers

void CCmdFilter::OnActFiltering() 
{
	UpdateData(TRUE);

	CMainFrame* main;
	main=(CMainFrame*)AfxGetMainWnd();

	CIVSTK* face = &((CSoftComputingWndDoc*)main->GetActiveDocument())->m_frtk;
	IPLIMAGE image = ((CSoftComputingWndDoc*)main->GetActiveDocument())->m_image;
	CString str = m_Filtering;

	int minmax[2];

	if (str.Compare("GABOR"))
	{
		minmax[0] = m_MinTh.GetPos();
		minmax[1] = m_MaxTh.GetPos();
	}
	else
	{
		minmax[0] = m_GaborAngle.GetPos();
		minmax[1] = m_GaborWave.GetPos();
	}

	face->IVSTKfiltering(image, NULL, str, minmax);
}

void CCmdFilter::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_MinTh.SetRange(0, 255);
	m_MaxTh.SetRange(0, 255);
	m_GaborAngle.SetRange(0, 3);
	m_GaborWave.SetRange(0, 3);

	UpdateData(FALSE);
}

void CCmdFilter::OnReleasedcaptureMinTh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_MinThr.Format("%03d", m_MinTh.GetPos());

	*pResult = 0;

	UpdateData(FALSE);
}

void CCmdFilter::OnReleasedcaptureMaxTh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_MaxThr.Format("%03d", m_MaxTh.GetPos());

	*pResult = 0;

	UpdateData(FALSE);
}

void CCmdFilter::OnReleasedcaptureGaborAngle(NMHDR* pNMHDR, LRESULT* pResult) 
{
	float angle[4] = {0.0f, 45.0f, -90.0f, -45.0f};

	m_GaborAngleVal.Format("%.0f", angle[m_GaborAngle.GetPos()]);

	*pResult = 0;

	UpdateData(FALSE);
}

void CCmdFilter::OnReleasedcaptureGaborWave(NMHDR* pNMHDR, LRESULT* pResult) 
{
	float wave[4] = {4.5f, 6.0f, 7.5f, 9.0f};

	m_GaborWaveVal.Format("%.1f", wave[m_GaborWave.GetPos()]);

	*pResult = 0;

	UpdateData(FALSE);
}