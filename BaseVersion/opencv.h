#ifndef OPENCV_H
#define OPENCV_H
#include "FormatChange.h"
#include "OpencvSelf.h"

class opencv : public FormatChange{

public:
	bool g_StartFlag;
	unsigned int g_FrameCounter;

public:
	opencv();
	~opencv();
	//UI
	void opencv::Printf(CString Context);

	void opencv::UpdateJPGInfo(unsigned int Num);

	void opencv::UpdateBlackPixelRate(float Num);

	void opencv::Start(void);

	void opencv::Stop(void);

	void opencv::HandleImage(int width ,int height, unsigned char *rgbdata);

};
#endif