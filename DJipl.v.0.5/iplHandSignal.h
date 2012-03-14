// iplHandSignal.h: interface for the iplHandSignal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPLHANDSIGNAL_H__D4111764_7CFE_435B_AEBD_39E3BC3DC2B6__INCLUDED_)
#define AFX_IPLHANDSIGNAL_H__D4111764_7CFE_435B_AEBD_39E3BC3DC2B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IplFace.h"
#include "highgui.h"

class iplHandSignal : public CIplFace  
{
public:
	iplHandSignal();
	virtual ~iplHandSignal();

	CvFont font;
	CvMoments moment;
	CvRect rect;

	void iplTestHS();
	void iplGetHSorientation(IPLIMAGE image, float& max_angle, int& flip_x, int& flip_y, IPLIMAGE image2);
	void iplGetHSpose(IPLIMAGE image, float *gray16, float& xy_ratio, float& finger_ratio);

	IPLIMAGE grabImage, cvtImage, blobImage;
};

#endif // !defined(AFX_IPLHANDSIGNAL_H__D4111764_7CFE_435B_AEBD_39E3BC3DC2B6__INCLUDED_)
