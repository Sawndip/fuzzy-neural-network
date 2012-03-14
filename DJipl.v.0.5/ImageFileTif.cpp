#include "stdafx.h"
#include "Image.h"
#include "ImageFileTiff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseImage



BOOL CmyImage::SaveTIF(LPCTSTR lpszFileName)
{

	return TRUE;
}

BOOL CmyImage::LoadTIF(LPCTSTR lpszFileName)
{
	m_hImage = LoadTIFFinDIB((LPSTR)lpszFileName);
	InitDIB(FALSE);
	return TRUE;
}