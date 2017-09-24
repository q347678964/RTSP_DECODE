#ifndef OPENCV_H
#define OPENCV_H
#include "FormatChange.h"

class opencv : public FormatChange{

public:

public:
	opencv();
	~opencv();
	//UI
	void opencv::Printf(CString Context);
	//
	void opencv::opencv_init(void);

	void opencv::opencv_stop(void);

	void opencv::opencv_showRGB(int width ,int height, unsigned char *rgbdata);
};
#endif