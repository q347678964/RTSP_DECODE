#ifndef FFMPEG_H
#define FFMPEG_H

extern "C"{
#include "libavutil/frame.h"
}
class ffmpeg {

public:
	unsigned long g_FrameCounter;
	int g_StartRecvRTSPFlag;
public:
	ffmpeg();
	~ffmpeg();
	void ffmpeg::Printf(CString Context);
	void ffmpeg::ffmpeg_init(void);
	void ffmpeg::ffmpeg_end(void);
	void ffmpeg::ffmpeg_start(void);
	void ffmpeg::SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int index, int bpp);
};
#endif