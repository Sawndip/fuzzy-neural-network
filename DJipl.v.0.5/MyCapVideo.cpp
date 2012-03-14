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

	// 캡쳐 윈도우를 만들어서 부모 윈도우 위에다가 붙이자.
	// m_hWnd는 새로 만든 윈도우의 핸들이고
	// hWnd는 부모의 윈도우 핸들이다.
	m_hWnd = capCreateCaptureWindow("", WS_CHILD | WS_VISIBLE, 
		0, 0, IMAGEX, IMAGEY, hWnd, 0);

	//=========================================
	// 여기서부터 비디오 드라이버 연결
	//=========================================
	
	// 드라이버들을 찾아서 연결해 준다.
	for(UINT uIndex = 0; uIndex < 10; uIndex ++)
	{
		if(capDriverConnect(m_hWnd, uIndex)) // 첫번째 쓸 수 있는 것으로 연결한다.
			break;
	}
	if(uIndex >= 10) // 드라이버는 10개까지 밖에 없다.
	{
		// x됐다. 컴퓨터 리부팅해야 한다.
		AfxMessageBox("비디오 드라이버를 연결할 수 없습니다.");
		capDriverDisconnect(m_hWnd);
		return FALSE;
	}

	// 프리뷰의 갱신율을 설정한다.
	capPreviewRate(m_hWnd, PREVIEW_RATE);

	// 드라이버가 연결되었으니까 이제 CAPDRIVERCAP 구조체를 얻어온다.
	// 왜? 카메라가 오버레이를 지원한다면 그게 더 빠르니까
	// 무슨 말이냐고? 오버레이가 되면 비디오 메모리 안 쓴다.
	// 비디오 메모리 사용하면 캡쳐 윈도우가 화면에서 안 보이면 캡쳐 안한다.
	CAPDRIVERCAPS drvcaps;
	if(!capDriverGetCaps(m_hWnd, &drvcaps, sizeof(CAPDRIVERCAPS)))
		memset(&drvcaps, 0, sizeof(CAPDRIVERCAPS));
	if(drvcaps.fHasOverlay)
		capOverlay(m_hWnd, TRUE); // 오버레이로 보여준다.
	else
		capPreview(m_hWnd, TRUE);

	// 드라이버에게 팔레트 생성하게 한다.
	capPaletteAuto(m_hWnd, 5, 256);

	// 콜백함수 선언한다. 한 프레임 캡쳐될 때마다 불려질 함수를 설정해 준다.
	capSetCallbackOnFrame(m_hWnd, (LPVOID)MakeProcInstance((FARPROC)capFrameCallback, AfxGetInstanceHandle()));

	// 캡쳐 파라미터를 설정한다.
	// 왜? 그냥 내 입맛에 맞게 설정. 여기서 설정한 거 말고도 많이 있으니 각자 입맛에 맞게 바꾸자.
	CAPTUREPARMS param;
	capCaptureGetSetup(m_hWnd, &param, sizeof(CAPTUREPARMS));
	param.fYield = TRUE; // 캡쳐를 분리된 백그라운드 스레드로 처리한다.
	param.fCaptureAudio = FALSE; // 오디오 캡쳐는 안한다.
	param.fAbortLeftMouse = FALSE; // 캡쳐할때 마우스 버튼 누르면 멈추게 하는 동작을 막는다.
	param.fAbortRightMouse = FALSE;
	capCaptureSetSetup(m_hWnd, &param, sizeof(CAPTUREPARMS));

	// 여기서부터는 캡쳐된 영상을 저장하기 위한 비트맵 설정
	// 왜? 영상 크기를 내 맘대로 바꾸려고
	DWORD dwSize = capGetVideoFormatSize(m_hWnd); // 포맷 크기 받아온다.
	HANDLE hbi = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, dwSize); // 메모리 잡는다. 메모리 핸들이 리턴된다.
	LPBITMAPINFO lpbi = (LPBITMAPINFO)GlobalLock(hbi); // 메모리 핸들로부터 실제 메모리 주소를 받아온다.
	capGetVideoFormat(m_hWnd, lpbi, dwSize); // 그 메모리에 비디오 포맷 정보를 넣는다.
	lpbi->bmiHeader.biWidth = IMAGEX; // 영상 크기 바꾼다.
	lpbi->bmiHeader.biHeight = IMAGEY;
	lpbi->bmiHeader.biSizeImage = IMAGEX*IMAGEY*3;
	capSetVideoFormat(m_hWnd, lpbi, dwSize); // 비디오 포맷 설정한다.
	GlobalUnlock(hbi); // 메모리 주소를 풀어준다.
	GlobalFree(hbi); // 메모리 핸들 해제

	// 캡쳐 시작
	return Capture();
}

void CMyCapVideo::Close()
{
	// 디바이스와 연결을 끊는다.
	capDriverDisconnect(m_hWnd);
}

// global image
// 10.11.01
GrayImage R(IMAGEX, IMAGEY);
BOOL m_bInit = FALSE;

// 화면 캡쳐가 이루어지면 불려지는 콜백함수이다.
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
		capGrabFrameNoStop(m_hWnd); // 영상을 캡쳐한다.
		return TRUE;
	}
	else
	{
		// 영상캡쳐를 정지한다.
		capCaptureStop(m_hWnd);
		return FALSE;
	}
}
