#ifndef OPENCV_MOTION_DETECTION_H
#define OPENCV_MOTION_DETECTION_H

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <opencv2/opencv.hpp>


#include "FormatChange.h"
#include "OpencvSelf.h"

#define BLACK_PIXEL_RATE 0.5

class OpencvMotionDetection : public FormatChange,public opencvself{

public:
	bool g_HandlingFlag;
	bool g_StartFlag;
	unsigned long g_FrameCounter;
	unsigned long g_SaveImageCounter;
	float g_BlackPixelRate;
	unsigned long g_LoseFrameCounter;
public:
	OpencvMotionDetection();
	~OpencvMotionDetection();
	
	void OpencvMotionDetection::Start(void);

	void OpencvMotionDetection::ReleaseIplImage(IplImage* &IplIma);
	void OpencvMotionDetection::Stop(void);

	void OpencvMotionDetection::Handle(IplImage *SrcImage);
	void OpencvMotionDetection::SaveImage(IplImage *Image);
};
#endif