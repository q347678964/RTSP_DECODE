#ifndef FFMPEG_H
#define FFMPEG_H
#include "FormatChange.h"

extern "C"{
#include "libavutil/frame.h"
}
class ffmpeg : public FormatChange{

public:
	unsigned long g_FrameCounter;
	int g_StartRecvRTSPFlag;
	CString g_URLCString;
public:
	ffmpeg();
	~ffmpeg();
	//UI
	void ffmpeg::UpdateFrameInfo(unsigned int Num);
	void ffmpeg::Printf(CString Context);
	//
	void ffmpeg::ffmpeg_init(void);
	void ffmpeg::ffmpeg_end(void);
	void ffmpeg::ffmpeg_start(CString URLCString);
	void ffmpeg::SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int index, int bpp);
};
#endif