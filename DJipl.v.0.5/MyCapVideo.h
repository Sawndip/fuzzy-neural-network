#ifndef _MYCAPVIDEO_H_
#define _MYCAPVIDEO_H_

#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

#define PREVIEW_RATE 30
#define IMAGEX 160
#define IMAGEY 120

#include "OpticalFlow.h"

class CMyCapVideo
{
	HWND m_hWnd;

	static LRESULT CALLBACK capFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr);

public:
	CMyCapVideo() {}
	~CMyCapVideo() {}

	//unsigned char buf[3];
	//unsigned char *m_pData;
	BOOL Init(HWND hWnd);
	void Close();
	BOOL Capture(BOOL bCap = TRUE);
};

#endif