#include <stdafx.h>
#include "MyCapVideo.h"
#include "Mainfrm.h"
#include <time.h>

#include "Image.h"
#include "cimage.h"
#include "ImgMisc.h"
#include "FaceTool.h"
#include "OpticalFlow.h"

#define FRAMENUM	3

static ColorImage Cap[FRAMENUM];
static int frameNum = 0;
static int f1, f2, f3;
static OpticalFlow opt;

BOOL CMyCapVideo::Init(HWND hWnd)
{
	ColorImage null;
	for (int i=0; i<FRAMENUM; i++)
		Cap[i] = null.black(IMAGEX, IMAGEY, 3);

	// ĸ�� �����츦 ���� �θ� ������ �����ٰ� ������.
	// m_hWnd�� ���� ���� �������� �ڵ��̰�
	// hWnd�� �θ��� ������ �ڵ��̴�.
	m_hWnd = capCreateCaptureWindow("", WS_CHILD | WS_VISIBLE, 
		0, 0, IMAGEX, IMAGEY, hWnd, 0);

	//=========================================
	// ���⼭���� ���� ����̹� ����
	//=========================================
	
	// ����̹����� ã�Ƽ� ������ �ش�.
	for(UINT uIndex = 0; uIndex < 10; uIndex ++)
	{
		if(capDriverConnect(m_hWnd, uIndex)) // ù��° �� �� �ִ� ������ �����Ѵ�.
			break;
	}
	if(uIndex >= 10) // ����̹��� 10������ �ۿ� ����.
	{
		// x�ƴ�. ��ǻ�� �������ؾ� �Ѵ�.
		AfxMessageBox("���� ����̹��� ������ �� �����ϴ�.");
		capDriverDisconnect(m_hWnd);
		return FALSE;
	}

	// �������� �������� �����Ѵ�.
	capPreviewRate(m_hWnd, PREVIEW_RATE);

	// ����̹��� ����Ǿ����ϱ� ���� CAPDRIVERCAP ����ü�� ���´�.
	// ��? ī�޶� �������̸� �����Ѵٸ� �װ� �� �����ϱ�
	// ���� ���̳İ�? �������̰� �Ǹ� ���� �޸� �� ����.
	// ���� �޸� ����ϸ� ĸ�� �����찡 ȭ�鿡�� �� ���̸� ĸ�� ���Ѵ�.
	CAPDRIVERCAPS drvcaps;
	if(!capDriverGetCaps(m_hWnd, &drvcaps, sizeof(CAPDRIVERCAPS)))
		memset(&drvcaps, 0, sizeof(CAPDRIVERCAPS));
	if(drvcaps.fHasOverlay)
		capOverlay(m_hWnd, TRUE); // �������̷� �����ش�.
	else
		capPreview(m_hWnd, TRUE);

	// ����̹����� �ȷ�Ʈ �����ϰ� �Ѵ�.
	capPaletteAuto(m_hWnd, 5, 256);

	// �ݹ��Լ� �����Ѵ�. �� ������ ĸ�ĵ� ������ �ҷ��� �Լ��� ������ �ش�.
	capSetCallbackOnFrame(m_hWnd, (LPVOID)MakeProcInstance((FARPROC)capFrameCallback, AfxGetInstanceHandle()));

	// ĸ�� �Ķ���͸� �����Ѵ�.
	// ��? �׳� �� �Ը��� �°� ����. ���⼭ ������ �� ���� ���� ������ ���� �Ը��� �°� �ٲ���.
	CAPTUREPARMS param;
	capCaptureGetSetup(m_hWnd, &param, sizeof(CAPTUREPARMS));
	param.fYield = TRUE; // ĸ�ĸ� �и��� ��׶��� ������� ó���Ѵ�.
	param.fCaptureAudio = FALSE; // ����� ĸ�Ĵ� ���Ѵ�.
	param.fAbortLeftMouse = FALSE; // ĸ���Ҷ� ���콺 ��ư ������ ���߰� �ϴ� ������ ���´�.
	param.fAbortRightMouse = FALSE;
	capCaptureSetSetup(m_hWnd, &param, sizeof(CAPTUREPARMS));

	// ���⼭���ʹ� ĸ�ĵ� ������ �����ϱ� ���� ��Ʈ�� ����
	// ��? ���� ũ�⸦ �� ����� �ٲٷ���
	DWORD dwSize = capGetVideoFormatSize(m_hWnd); // ���� ũ�� �޾ƿ´�.
	HANDLE hbi = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, dwSize); // �޸� ��´�. �޸� �ڵ��� ���ϵȴ�.
	LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(hbi); // �޸� �ڵ�κ��� ���� �޸� �ּҸ� �޾ƿ´�.
	capGetVideoFormat(m_hWnd, lpbi, dwSize); // �� �޸𸮿� ���� ���� ������ �ִ´�.
	lpbi->bmiHeader.biWidth = IMAGEX; // ���� ũ�� �ٲ۴�.
	lpbi->bmiHeader.biHeight = IMAGEY;
	lpbi->bmiHeader.biSizeImage = IMAGEX*IMAGEY*3;
	capSetVideoFormat(m_hWnd, lpbi, dwSize); // ���� ���� �����Ѵ�.
	GlobalUnlock(hbi); // �޸� �ּҸ� Ǯ���ش�.
	GlobalFree(hbi); // �޸� �ڵ� ����

	// ĸ�� ����
	return Capture();
}

void CMyCapVideo::Close()
{
	// ����̽��� ������ ���´�.
	capDriverDisconnect(m_hWnd);
}

// global image
// 10.11.01
GrayImage R(IMAGEX, IMAGEY);
BOOL m_bInit = FALSE;

// ȭ�� ĸ�İ� �̷������ �ҷ����� �ݹ��Լ��̴�.
LRESULT CALLBACK CMyCapVideo::capFrameCallback(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	CMainFrame *frame = (CMainFrame *)AfxGetMainWnd();

	if (!(frame->process_start))
		return 0;

	//////////////////////////////////////////////////////////////////////////////////
	int i, j, k;
	CString strText;
	
	clock_t start, finish;
	double fps_num;

	// image allocation
	ColorImage A(IMAGEX, IMAGEY, 3);
	GrayImage Z;

	start = clock();

	// 1. captured image -> processable image
	for (j=0; j<3; j++)
	{
		k = i = 0;
		for (i=0; i<IMAGEX*IMAGEY; i++, k += 3)
			*(A.buf[j]+i) = lpVHdr->lpData[j+k];
	}

	// 1.1 image for initial background
	if (!m_bInit)
	{
		GrayImage *T = A.getLumImg();
		R = *(T);
		delete T;
		m_bInit = TRUE;
	}

	// 2. multi-image
	Cap[frameNum] = A;

	switch (frameNum)
	{
		case 0:
			f1 = 2;
			f2 = 0;
			f3 = 1;
			break;
		case 1:
			f1 = 0;
			f2 = 1;
			f3 = 2;
			break;
		case 2:
			f1 = 1;
			f2 = 2;
			f3 = 0;
			break;
	}

	frameNum = f3;

	// 3. real functions
	CFaceTool face(&A);
	
	face.DJsimple_face_detection(Z, &Cap[f2], 0);
	//opt.Horn(*Cap[f2].getLumImg(), *Cap[f1].getLumImg(), &F, 1, 4.0);

	// 4. result image -> showable image
	for (j=0; j<3; j++)
	{
		k = i = 0;
		for (i=0; i<IMAGEX*IMAGEY; i++, k += 3)
		{
			/*
			if (A.d() == 3)	// color image
				lpVHdr->lpData[j+k] = *(A.buf[j]+i);
			else if (A.d() == 1)	// gray image
				lpVHdr->lpData[j+k] = *(A.buf[0]+i);
			else
				lpVHdr->lpData[j+k] *= (*(A.buf[0]+i)/255);
			*/

			lpVHdr->lpData[j+k] = *(Z.buf[0]+i);
		}
	}


	//////////////////////////////////////////////////////////////////////////////////
	finish = clock();
	fps_num = CLOCKS_PER_SEC / (double)(finish - start) ;
   
	strText.Format("fps %.3f", fps_num);
	frame->m_wndStatusBar.SetPaneText(2, strText);

	strText.Format("I2: %03d,%03d I3: %03d,%03d", face.faceTh[0][0], face.faceTh[0][1], face.faceTh[1][0], face.faceTh[1][1]);
	frame->m_wndStatusBar.SetPaneText(3, strText);

	return 0;
}

BOOL CMyCapVideo::Capture(BOOL bCap)
{
	if(bCap)
	{
		capGrabFrameNoStop(m_hWnd); // ������ ĸ���Ѵ�.
		return TRUE;
	}
	else
	{
		// ����ĸ�ĸ� �����Ѵ�.
		capCaptureStop(m_hWnd);
		return FALSE;
	}
}
