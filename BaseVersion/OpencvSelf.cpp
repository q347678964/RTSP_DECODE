#include "stdafx.h"
#include "resource.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "opencv.h"
#include "OpencvSelf.h"

opencvself::opencvself()
{

}
opencvself::~opencvself()
{

}

float opencvself::BinBlackPixelRate(IplImage *BinImage)
{
#define BLACK_PIXEL 255
#define WITHE_PIXEL 0

	unsigned int width = BinImage->width;
	unsigned int height = BinImage->height;
	unsigned int channel = BinImage->nChannels;
	unsigned int depth = BinImage->depth;
	unsigned int BlackPixelCounter = 0;
	float BlackRate = 0.0;
	if(depth != IPL_DEPTH_8U || channel != 1){
		return 0;
	}else{
		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				unsigned char Pixel = *(BinImage->imageData+i*width+j);
				BlackPixelCounter = (Pixel==BLACK_PIXEL)?BlackPixelCounter+1:BlackPixelCounter;
			}
		}

		BlackRate = BlackPixelCounter*100.0/(width*height);
		return BlackRate;
	}
}

unsigned int opencvself::BinBlackPixelCounter(IplImage *BinImage)
{
#define BLACK_PIXEL 255
#define WITHE_PIXEL 0

	unsigned int width = BinImage->width;
	unsigned int height = BinImage->height;
	unsigned int channel = BinImage->nChannels;
	unsigned int depth = BinImage->depth;
	unsigned int BlackPixelCounter = 0;

	if(depth != IPL_DEPTH_8U || channel != 1){
		return 0;
	}else{
		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				unsigned char Pixel = *(BinImage->imageData+i*width+j);
				BlackPixelCounter = (Pixel==BLACK_PIXEL)?BlackPixelCounter+1:BlackPixelCounter;
			}
		}
		return BlackPixelCounter;
	}
}