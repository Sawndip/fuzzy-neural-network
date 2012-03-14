// SVV.h: interface for the CSVV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVV_H__3D66F8C0_40AF_11D4_B766_00E0294BE14D__INCLUDED_)
#define AFX_SVV_H__3D66F8C0_40AF_11D4_B766_00E0294BE14D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSVV  
{
private:
	long rho_min;
public:
	long * btrIY;
	long * btrIX;
	int m_v;
	int m_h;
	int m_u;
	BYTE * LPtrInv(BYTE *Img, int w, int h);
	int m_w;
	long * btrY;
	long * btrX;
	void LPtable(int w, int h, int u, int v);
	BYTE * LPtr(BYTE *Img, int u,int v);
	CSVV(long rho = 1);
	virtual ~CSVV();
};

#endif // !defined(AFX_SVV_H__3D66F8C0_40AF_11D4_B766_00E0294BE14D__INCLUDED_)
