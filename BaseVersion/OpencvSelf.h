#ifndef OPENCVSELF_H
#define OPENCVSELF_H
#include "FormatChange.h"
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <opencv2/opencv.hpp>

class opencvself : public FormatChange{

public:
	opencvself();
	~opencvself();
	float opencvself::BinBlackPixelRate(IplImage *BinImage);
	unsigned int opencvself::BinBlackPixelCounter(IplImage *BinImage);
};
#endif