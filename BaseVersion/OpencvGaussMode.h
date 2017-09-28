#ifndef OPENCV_GAUSS_MODE_H
#define OPENCV_GAUSS_MODE_H

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <opencv2/opencv.hpp>


#include "FormatChange.h"
#include "OpencvSelf.h"

class OpencvGaussMode : public FormatChange,public opencvself{

public:
	bool g_HandlingFlag;
	HANDLE hSemaphore;

	bool g_StartFlag;
	unsigned long g_FrameCounter;
	unsigned long g_SaveImageCounter;
	float g_BlackPixelRate;
	unsigned long g_LoseFrameCounter;
public:
	OpencvGaussMode();
	~OpencvGaussMode();
	
	void OpencvGaussMode::Start(void);

	void OpencvGaussMode::ReleaseIplImage(IplImage* &IplIma);
	void OpencvGaussMode::Stop(void);

	void OpencvGaussMode::Handle(IplImage *SrcImage);
	void OpencvGaussMode::SaveImage(IplImage *Image);
};
#endif