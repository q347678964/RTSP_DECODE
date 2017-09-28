#ifndef OPENCV_ABSMOTION_DETECTION_H
#define OPENCV_ABSMOTION_DETECTION_H

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <opencv2/opencv.hpp>


#include "FormatChange.h"
#include "OpencvSelf.h"

#define BLACK_PIXEL_RATE 0.5

class OpencvAbsDetection : public FormatChange,public opencvself{

public:
	bool g_HandlingFlag;
	bool g_StartFlag;
	unsigned long g_FrameCounter;
	unsigned long g_SaveImageCounter;
	float g_BlackPixelRate;
	unsigned long g_LoseFrameCounter;

	HANDLE hSemaphore;
	IplImage *g_pSrcScaleImage;
	IplImage *g_pCurGrayImage;
	IplImage *g_pLastGrayImage;
	IplImage *g_pAbsGrayImage;
	IplImage *g_pAbsBinImage;

public:
	OpencvAbsDetection();
	~OpencvAbsDetection();
	
	void OpencvAbsDetection::Start(void);

	void OpencvAbsDetection::ReleaseIplImage(IplImage* &IplIma);
	void OpencvAbsDetection::Stop(void);

	void OpencvAbsDetection::Handle(IplImage *SrcImage);
	void OpencvAbsDetection::SaveImage(IplImage *Image);
};
#endif