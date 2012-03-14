///////////////////////////////////////////////////////////////////////
//chart.h, Juan Carlos Sanchez
///////////////////////////////////////////////////////////////////////

#if !defined(GRAF_H_10101010)
#define GRAF_H_10101010

// \n  |     /\\                /\\\n  |\\_/   \\       /\\_/    \\        _/\\\n  |          \\__/             \\__/      \\/\n  |___________________________

#define CTLOFFSET 0
#define SCROLL_VALUE 1000.0 

#define WM_NEWHORZLIMITS (CTLOFFSET + WM_USER + 701)
#define WM_NEWVERTLIMITS (CTLOFFSET + WM_USER + 702)

#define GS_NOTIFY	0x00000001L
#define LS_HORZAUTO 0x00010000
#define LS_POINT	0x00020000

#ifdef _AFXDLL

#define BEGIN_MESSAGE_MAP_TEMPLATES(thetemplate, theClass, baseClass) \
	thetemplate const AFX_MSGMAP* PASCAL theClass::_GetBaseMessageMap() \
		{ return &baseClass::messageMap; } \
	thetemplate const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	thetemplate AFX_COMDAT AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
	{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
	thetemplate AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
	{ \

#else

#define BEGIN_MESSAGE_MAP_TEMPLATES(thetemplate, theClass, baseClass) \
	thetemplate const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	thetemplate AFX_COMDAT AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
	{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \
	thetemplate AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
	{ \

#endif //_AFXDLL


#pragma warning (disable: 4786 4788)

#include <afxwin.h>
#include <math.h>
#include <map>
#include <set>


using namespace std;


template<class T >
class CScrollChart: public CWnd
{

protected:
	
typedef map<T, T> MAPDATA;

typedef struct  {
	MAPDATA m_mapData;
	COLORREF crColor;
	int nStyle;
	int nWidth;
	BOOL bIsVisible;
	BOOL bIsAuto;
	BOOL bPoints;
	T nAutoHorzInc;
} stuctLine;

typedef map<UINT, stuctLine> MAPLINE;

	T m_MaxValue;
	T m_MinValue;
	T m_pointIni;
	T m_MaxHzVal;
	T m_MinHzVal;
	T m_nHzRange;

	MAPLINE m_mapLines;
	COLORREF m_ColorBk;	
	BOOL m_bGrid;
	BOOL m_bNorify;
	UINT m_nHorzDiv;
	UINT m_nVertDiv;
	UINT m_nMaxPoints;	
	set<UINT> m_setLinesID;

public:
	CScrollChart(): CWnd(),  m_pointIni(0,0),   m_nHorzDiv(7), m_nVertDiv(14), m_nMaxPoints(0),
		m_mapLines(), m_setLinesID(), m_MaxHzVal(1000), m_nHzRange(1000), m_MinHzVal(0)  {}

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	BOOL AddLine(UINT nID, COLORREF crColor, DWORD nStyle = PS_SOLID, 
					int nWidth = 1,BOOL bVisible = TRUE, int nAutoInc = 10);

	BOOL AddData(UINT nID, T nX, T nY, BOOL bDraw = TRUE);
	BOOL AddData(UINT nID, T nY, BOOL bDraw = TRUE);
	COLORREF SetBkColor(COLORREF cr );
	COLORREF GetBkColor() const;
	COLORREF SetLineColor(UINT nID, COLORREF cr );
	COLORREF GetLineColor(UINT nID);
	void ShowLine(UINT nID, BOOL value = TRUE, BOOL bDraw = TRUE); 
	void ShowPoints(UINT nID, BOOL value = TRUE, BOOL bDraw = TRUE); 
	void SetGrid(BOOL value = TRUE);
	T SetHorzRange(T nRange);
	T SetAutoinc(UINT nID, T nInc);
	UINT SetHorzDiv(UINT nDiv, BOOL bDraw = TRUE);
	UINT GetHorzDiv() const {return m_nHorzDiv;}
	UINT SetVertDiv(UINT nDiv, BOOL bDraw = TRUE);
	UINT GetVertDiv() const {return m_nVertDiv;}
	UINT SetMaxPoints(UINT nValue);
	UINT GetMaxPoints() const { return m_nMaxPoints;}
	int SetLineStyle(UINT nID, int nStile, BOOL bDraw = TRUE);
	int GetLineStyle(UINT nID);
	int SetLineWidth(UINT nID, int nWidth, BOOL bDraw = TRUE);
	int GetLineWidth(UINT nID);


	
protected:
	afx_msg void OnPaint();
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	CPoint LPtoDP(T X, T Y);
	CPoint LPtoDP(pair<T, T> pnt) {return LPtoDP(pnt.first, pnt.second);}
	BOOL bIsFirstPoint();	
	void UpdateScrollInfo();
	void SendNewLimits();
	void Normalize();
	BOOL ErasePoints();
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP_TEMPLATES(template<class T>, CScrollChart<T> , CWnd)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


template<class T> 
void CScrollChart<T>::OnPaint( )
{
	
	CRect RectClient;
	CPen *penLine;
	CPen *oldPen;

	CPaintDC lop(this);
	GetClientRect(RectClient);
	CDC PaintDc;
	CBitmap bitm;
	PaintDc.CreateCompatibleDC(&lop);

	bitm.CreateCompatibleBitmap(&lop, RectClient.Width(), RectClient.Height());
    CBitmap* pOldBitmap = PaintDc.SelectObject (&bitm);

	PaintDc.SetBkMode(OPAQUE);
	PaintDc.SetBkColor(m_ColorBk);
	PaintDc.SetViewportOrg(0, 0);

	CBrush bursh(m_ColorBk);
	PaintDc.FillRect(RectClient, &bursh);	
    
	if (m_bGrid)
	{

		penLine = new CPen(PS_SOLID, 1, RGB(0, 128, 0));
		oldPen = PaintDc.SelectObject(penLine);
		if (m_nVertDiv > 0)
		{
			for (int n = RectClient.Height() / m_nVertDiv; n < RectClient.Height ();
						n += RectClient.Height() / m_nVertDiv)
			{	
				PaintDc.MoveTo(0, n);
				PaintDc.LineTo(RectClient.Width(), n);
			}
		}
		if (m_nHorzDiv > 0)
		{
			for (int n = m_pointIni + ((m_nHzRange / m_nHorzDiv) - fmod(m_pointIni,m_nHzRange));
					n < m_pointIni + m_nHzRange; 
					n += (m_nHzRange / m_nHorzDiv))
			{						
				int nInipoint=LPtoDP(n, 0).x;
				PaintDc.MoveTo(nInipoint, 0);
				PaintDc.LineTo(nInipoint, RectClient.Width ());
			}		
		}
		PaintDc.SelectObject(oldPen);
		delete(penLine);
	}  //Grid

	if (!m_mapLines.empty())
	{
		MAPLINE::const_iterator iterLine = m_mapLines.begin();
		while ((iterLine) != m_mapLines.end())
		{

			if (!(*iterLine).second.m_mapData.empty())
				{

				if ((*iterLine).second.bIsVisible)
				{
					CPoint pointDraw;
					CPoint pointOld;
					CSize sizePoint( 2+((*iterLine).second.nWidth),
									2+((*iterLine).second.nWidth) );
					CBrush brushPoint;

					brushPoint.CreateSolidBrush((*iterLine).second.crColor);
					penLine = new CPen((*iterLine).second.nStyle, 
									(*iterLine).second.nWidth, 
									(*iterLine).second.crColor);
					oldPen = PaintDc.SelectObject(penLine);
					CBrush* oldBrush = PaintDc.SelectObject(&brushPoint);
					MAPDATA::const_iterator iter = (*iterLine).second.m_mapData.lower_bound(m_pointIni);
					if (iter != (*iterLine).second.m_mapData.begin()) --iter;
					pointDraw = LPtoDP((*iter));
					PaintDc.MoveTo(pointDraw);
					pointOld = pointDraw;
					iter++;
					while ( (iter != (*iterLine).second.m_mapData.upper_bound(m_nHzRange + m_pointIni))
							&& (iter != (*iterLine).second.m_mapData.end()) )
						{
							pointDraw = LPtoDP((*iter));
							PaintDc.LineTo(pointDraw);

							if ((*iterLine).second.bPoints) 
								PaintDc.Ellipse(CRect( pointOld - sizePoint,
													pointOld + sizePoint) );
							pointOld = pointDraw;
							++iter;
						}
					if ((*iterLine).second.bPoints) 
						PaintDc.Ellipse(CRect( pointOld - sizePoint,
											pointOld + sizePoint) );
					if (iter != (*iterLine).second.m_mapData.end()) 
						{
							pointOld=LPtoDP(*iter);
							PaintDc.LineTo(pointOld);
							if ((*iterLine).second.bPoints) 
								PaintDc.Ellipse( CRect( pointOld - sizePoint,
												pointOld + sizePoint) );
						}
					PaintDc.SelectObject(oldPen);
					PaintDc.SelectObject(oldBrush);
					delete(penLine);
				}
				}
			iterLine++;
		}
		
	} // not empty
	lop.BitBlt(0,0,RectClient.Width(), RectClient.Height(), &PaintDc, 0, 0, SRCCOPY);
	PaintDc.SelectObject(pOldBitmap);
}

template<class T> 
BOOL CScrollChart<T>::AddData(UINT nID, T nY, BOOL bDraw)
{
	if ( (m_setLinesID.find(nID) == m_setLinesID.end()) || (!(m_mapLines[nID].bIsAuto)) )
		return FALSE;
	if (m_mapLines[nID].m_mapData.empty())
		return AddData(nID, 0, nY, bDraw);
	return AddData(nID, 
		(*(--m_mapLines[nID].m_mapData.end())).first + m_mapLines[nID].nAutoHorzInc,
		nY, bDraw);
}
	
template<class T> 
BOOL CScrollChart<T>::AddData(UINT nID, T nX, T nY, BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end())
		return FALSE;
	BOOL bScrollModi = FALSE;
	if (bIsFirstPoint())
	{
		m_MaxValue = nY;
		m_MinValue = nY;
		
		SendNewLimits();

	} else 
	{
	
		if (!m_mapLines[nID].m_mapData.empty())
		{
	

			if ( ((*(--(m_mapLines[nID].m_mapData.end()))).first >= m_pointIni) &&
				 ((*(--(m_mapLines[nID].m_mapData.end()))).first <= m_pointIni + m_nHzRange) &&
				 (nX > m_pointIni + m_nHzRange * .9) )
			{
			//Move init
				m_pointIni = nX - (m_nHzRange * .9);
				bScrollModi = TRUE;
			}
		}
	
		if ( (!bScrollModi) && (nX > m_MaxHzVal) && (nX>m_nHzRange) )
		{
			//right-out
			m_MaxHzVal = nX + m_nHzRange * .1;
			bScrollModi = TRUE;
		}

		if ( (!bScrollModi) && (nX < m_MinHzVal) )
		{
			//left
			m_MinHzVal = nX;
			bScrollModi = TRUE; 
		}

		if (nY > m_MaxValue)
		{
			m_MaxValue = nY;
			SendNewLimits();
		}
		if (nY < m_MinValue) 
		{
			m_MinValue = nY;
			SendNewLimits();
		}
	} //else fisrt point	

	m_mapLines[nID].m_mapData[nX] = nY;

	if (ErasePoints()) bScrollModi = TRUE;

	if (bScrollModi) Normalize();

	if (bDraw) Invalidate();

	if (bScrollModi) UpdateScrollInfo();

	return TRUE;
}

template<class T> 
BOOL CScrollChart<T>::AddLine(UINT nID, COLORREF crColor, DWORD  nStyle, int nWidth, BOOL bVisible, int nAutoInc)
{
	if (!(m_setLinesID.insert(nID).second)) return FALSE;
	if (!((0x0000FFFF) & nStyle)) nStyle |= PS_SOLID;
	stuctLine structLineTmp;
	structLineTmp.bIsAuto = ((LS_HORZAUTO & nStyle)? TRUE : FALSE);
	structLineTmp.bPoints = ((LS_POINT & nStyle)? TRUE : FALSE);
	structLineTmp.nAutoHorzInc = nAutoInc;
	structLineTmp.crColor = crColor;
	structLineTmp.nStyle = ((0x0000FFFF) & nStyle);
	structLineTmp.nWidth = nWidth;
	structLineTmp.bIsVisible = bVisible;
	m_mapLines[nID] = structLineTmp;
	return TRUE;
}

template<class T> 
BOOL CScrollChart<T>::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= WS_CHILD;
	dwStyle |= WS_HSCROLL;
	if (dwStyle & GS_NOTIFY) m_bNorify=TRUE; else m_bNorify=FALSE;
	dwStyle &= ~(GS_NOTIFY);
	BOOL nresult = CWnd::Create(NULL, NULL,dwStyle, rect, pParentWnd, nID, NULL);
	m_ColorBk = RGB(0,0,0);

	m_bGrid = TRUE;
	m_MinValue = 0;	
	//m_MaxValue = m_nHzRange;
	m_MaxHzVal = m_nHzRange;
	m_MaxValue = 0;
	m_nHorzDiv = 20;
	m_nVertDiv = 10;
	UpdateScrollInfo();
	return nresult;
}

template<class T> 
void CScrollChart<T>::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	float nIntPos;
	switch (nSBCode) {

	case SB_LINELEFT:
		if (m_pointIni < 1) return;
		m_pointIni -= (m_nHzRange * .01);
		break;

	case SB_LINERIGHT:
		m_pointIni += (m_nHzRange * .01);
		break;

	case SB_PAGEUP:
		m_pointIni -= m_nHzRange / 2;
		if ( m_pointIni < 0 ) m_pointIni = 0;
		break;

	case SB_PAGERIGHT:
		m_pointIni += m_nHzRange / 2;
		break;

	case SB_THUMBTRACK:
		
		nIntPos=( (int)nPos / (SCROLL_VALUE) );
		
		m_pointIni = ( (m_MaxHzVal - m_MinHzVal ) * nIntPos) + m_MinHzVal;
		
		break;
	default:
		return;
		}
	if ((m_pointIni > m_MaxHzVal - m_nHzRange)) m_pointIni = m_MaxHzVal - m_nHzRange;
	if ( m_pointIni < m_MinHzVal ) m_pointIni = m_MinHzVal;
	Invalidate();
	UpdateScrollInfo();

}

template<class T> 
COLORREF CScrollChart<T>::SetBkColor(COLORREF cr )
{
	COLORREF crOld = m_ColorBk;
	m_ColorBk = cr;
	Invalidate();
	return crOld;
}

template<class T> 
COLORREF CScrollChart<T>::GetBkColor() const
{
	return m_ColorBk;
}

template<class T> 
COLORREF CScrollChart<T>::SetLineColor(UINT nID, COLORREF cr )
{
	COLORREF crOld;
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return NULL;
	crOld = m_mapLines[nID].crColor;
	m_mapLines[nID].crColor = cr;
	Invalidate();
	return crOld;
}

template<class T> 
COLORREF CScrollChart<T>::GetLineColor(UINT nID) 
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return NULL;
	return m_mapLines[nID].crColor;
}

template<class T> 
int CScrollChart<T>::SetLineStyle(UINT nID, int nStyle,BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	int nResult = m_mapLines[nID].nStyle;
	m_mapLines[nID].nStyle = nStyle;
	if (bDraw) Invalidate();
	return nResult;
}

template<class T> 
int CScrollChart<T>::GetLineStyle(UINT nID)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	return m_mapLines[nID].nStyle;
}

template<class T> 
int CScrollChart<T>::SetLineWidth(UINT nID, int nWidth,BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	int nResult = m_mapLines[nID].nWidth;
	m_mapLines[nID].nWidth = nWidth;
	if (bDraw) Invalidate();
	return nResult;
}

template<class T> 
int CScrollChart<T>::GetLineWidth(UINT nID)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	return m_mapLines[nID].nWidth;
}

template<class T> 
T CScrollChart<T>::SetAutoinc(UINT nID,T nInc)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	T nTmp = m_mapLines[nID].nAutoHorzInc;
	m_mapLines[nID].nAutoHorzInc = nInc;
	return nTmp;

}

template<class T> 
void CScrollChart<T>::ShowLine(UINT nID, BOOL value, BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return;
	m_mapLines[nID].bIsVisible = value;
	if (bDraw) Invalidate();
}

template<class T> 
void CScrollChart<T>::ShowPoints(UINT nID, BOOL value, BOOL bDraw)
{	
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return;
	m_mapLines[nID].bPoints = value;
	if (bDraw) Invalidate();
}

template<class T> 
UINT CScrollChart<T>::SetMaxPoints(UINT nValue)
{
	UINT nTemp = m_nMaxPoints;
	m_nMaxPoints = nValue;
	if (nTemp != nValue) 
	{
		
		ErasePoints();
		Invalidate();
	}
	return nTemp;
}

template<class T> 
void CScrollChart<T>::SetGrid(BOOL value)
{
	m_bGrid = value;
	Invalidate();
}

template<class T> 
UINT CScrollChart<T>::SetHorzDiv(UINT nDiv, BOOL bDraw)
{
	UINT nTmp = m_nHorzDiv;
	m_nHorzDiv = nDiv;
	if (bDraw) Invalidate();
	return nTmp;
}

template<class T> 
UINT CScrollChart<T>::SetVertDiv(UINT nDiv, BOOL bDraw)
{
	UINT nTmp = m_nVertDiv;
	m_nVertDiv = nDiv;
	if (bDraw) Invalidate();
	return nTmp;
}

template<class T> 
CPoint CScrollChart<T>::LPtoDP(T X,T Y)
{
	int nY;
	int nX;
	CRect rectClient;
	GetClientRect(rectClient);
	nY = ((rectClient.Height() * Y * .95) / m_MaxValue);
	nX = ((rectClient.Width() * (X - m_pointIni)) / m_nHzRange);
	return CPoint(nX, rectClient.Height() - nY);
}

template<class T> 
T CScrollChart<T>::SetHorzRange(T nRange)
{
	T nTmp = m_nHzRange;
	m_nHzRange = nRange;
	if (m_MaxHzVal < m_nHzRange) m_MaxHzVal = m_nHzRange * 1.1;
	//Normalize();
	UpdateScrollInfo();
	return nTmp;
}

template<class T> 
BOOL CScrollChart<T>::bIsFirstPoint()
{
	MAPLINE::const_iterator iterLine = m_mapLines.begin();
	while ( (iterLine) != m_mapLines.end() )
	{
		if ( !(*iterLine).second.m_mapData.empty() ) 
			return FALSE;
		++iterLine;
	}
	return TRUE;
}

template<class T> 
void CScrollChart<T>::UpdateScrollInfo()
{
	SCROLLINFO si;
	si.fMask = SIF_ALL;
	si.nPos = SCROLL_VALUE * ( (m_pointIni - m_MinHzVal) / ( m_MaxHzVal - m_MinHzVal ) );;
	si.nMin = 0;
	si.nMax = SCROLL_VALUE;
	si.nPage = SCROLL_VALUE * ( m_nHzRange / ( m_MaxHzVal - m_MinHzVal ) );	
	SetScrollInfo(SB_HORZ, &si, TRUE);
	SendNewLimits();
}

template<class T> 
BOOL CScrollChart<T>::ErasePoints()
{
	
	if (!m_nMaxPoints) return FALSE;
	BOOL bResult = FALSE;
	MAPLINE::iterator iterLine = m_mapLines.begin();
	while ((iterLine) != m_mapLines.end())
	{
		while ( ( (*iterLine).second.m_mapData.size() ) > m_nMaxPoints ) 
		{
			(*iterLine).second.m_mapData.erase((*iterLine).second.m_mapData.begin() );
			bResult = TRUE;
		}		
		++iterLine;
	}

	if (bResult)
	{
		BOOL bFirst=FALSE;	
		iterLine = m_mapLines.begin();
		while ( (!bFirst) && ((iterLine) != m_mapLines.end()) )
		{
			if ( !((*iterLine).second.m_mapData.empty()) )  
			{
				m_MinHzVal = ( (*((*iterLine).second.m_mapData.begin()) ).first);
				bFirst = TRUE;
			}
			++iterLine;
		}
		
		while ((iterLine) != m_mapLines.end())
		{
			if ( !((*iterLine).second.m_mapData.empty()) )  
			{
				if  ( m_MinHzVal > ( (*((*iterLine).second.m_mapData.begin() )).first) ) 	
					m_MinHzVal = ( (*((*iterLine).second.m_mapData.begin() )).first);
			}
			++iterLine;
		}
	}
	return bResult;
}

template<class T> 
void CScrollChart<T>::SendNewLimits()
{
	if(!m_bNorify) return;
	GetParent()->PostMessage (WM_NEWHORZLIMITS,
							(WPARAM)((T) m_pointIni), 
							(LPARAM)((T) (m_pointIni + m_nHzRange)) );
	GetParent()->PostMessage (WM_NEWVERTLIMITS, 
							(WPARAM)&((T) m_MinValue), (LPARAM)&((T) m_MaxValue));
}

template<class T> 
void CScrollChart<T>::Normalize()
{
		if ( m_pointIni < m_MinHzVal ) m_pointIni = m_MinHzVal;
		if ( m_MaxHzVal < m_pointIni + m_nHzRange) m_MaxHzVal = m_pointIni + m_nHzRange;
}

#endif 


  
