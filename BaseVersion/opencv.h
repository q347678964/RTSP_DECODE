#ifndef OPENCV_H
#define OPENCV_H
#include "FormatChange.h"
#include "OpencvSelf.h"

class opencv : public FormatChange{

public:
	bool g_startflag;
	unsigned int g_FrameCounter;
	opencvself OpencvSelf;
	unsigned int g_SaveImageCounter;
public:
	opencv();
	~opencv();
	//UI
	void opencv::Printf(CString Context);

	void opencv::UpdateJPGInfo(unsigned int Num);

	void opencv::UpdateBlackPixelInfo(unsigned int Num);

	void opencv::opencv_init(void);

	void opencv::opencv_stop(void);

	void opencv::opencv_showRGB(int width ,int height, unsigned char *rgbdata);

	void opencv::ReleaseIplImage(IplImage* &IplIma);
};
#endif