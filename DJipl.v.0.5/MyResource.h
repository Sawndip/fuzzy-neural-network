#ifndef	my_resource_h
#define my_resource_h	1

// projection result의 길이 조정(128 pixel)
#define	MY_PROJ_TOP	128

// USB 카메라를 이용한 동영상 처리 방법
#define	PROCESSOR_PAUSED			0
#define	PROCESSOR_SIMPLE_VIEWER		1
#define	PROCESSOR_IMAGE_FILTER		2
#define	PROCESSOR_MOTION_DETECTOR	3
#define	PROCESSOR_WHITE_DETECTOR	4
#define	PROCESSOR_FACE_DETECTOR		5
#define	PROCESSOR_BACKGROUND_EXCEPTION		6

#define	CP_SIZE_X	(640)
#define	CP_SIZE_Y	(490)
#define	LP_SIZE_X	(32)
#define	LP_SIZE_Y	(64)

#define	NUM_BUFFERS	(10)
#define NUM_FILTERS	(4)
#define	CE21	(2147483648)

#define TIME_CHECK	(0)

#define	DJ_IPL_IN_BOUND	0
#define	DJ_IPL_OUT_OF_BOUND	1
#define	DJ_IPL_GT 2
#define	DJ_IPL_LT 3
#define	DJ_IPL_OTSU_H 4
#define	DJ_IPL_OTSU_L 5

#define	RAD_DEG	(M_PI/(180.0))
#define	DEG_RAD	((180.0)/M_PI)

#define CHECK_TIME_START if(QueryPerformanceFrequency((_LARGE_INTEGER*)&freq)) {QueryPerformanceCounter((_LARGE_INTEGER*)&start);
#define CHECK_TIME_END(a,b) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((double)(end - start)/freq*1000); b=TRUE; } else b=false;

#endif