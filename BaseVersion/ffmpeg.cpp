#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"
#include "BaseVersionDlg.h"
#include "ffmpeg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
using namespace std;

extern "C" {
#include <libavcodec/avcodec.h> 
#include <libavdevice/avdevice.h> 
#include <libavformat/avformat.h> 
#include <libavfilter/avfilter.h> 
#include <libavutil/avutil.h> 
#include <libswscale/swscale.h>
}

/*************************************************************************************/
//构造、析构函数
ffmpeg::ffmpeg(){
	this->Printf((CString)"***********************\r\n");
}

ffmpeg::~ffmpeg(){

}
int decode_video_example(int argc, char **argv);
/*************************************************************************************/
//主窗体操作
void ffmpeg::Printf(CString Context){
	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
	CString m_strTime = time.Format("[%H:%M:%S]");

	Context = m_strTime + (CString)("[ffmpeg]") + Context;
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->Printf(Context);
}



static bool bStop = false;
static int FrameCounter = 0;
//保存BMP文件的函数  
void SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int index, int bpp)  
{  
    char buf[5] = {0};  
    BITMAPFILEHEADER bmpheader;  
    BITMAPINFOHEADER bmpinfo;  
    FILE *fp;  
  
    char *filename = new char[255];  
  
    //文件存放路径，根据自己的修改  
    sprintf_s(filename, 255, "%s%d.bmp", "../Output/", index);  
    if( (fp = fopen(filename,"wb+")) == NULL ) {  
        printf ("open file failed!\n");  
        return;  
    }  
  
    bmpheader.bfType = 0x4d42;  
    bmpheader.bfReserved1 = 0;  
    bmpheader.bfReserved2 = 0;  
    bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
    bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;  
  
    bmpinfo.biSize = sizeof(BITMAPINFOHEADER);  
    bmpinfo.biWidth = width;  
    bmpinfo.biHeight = height;  
    bmpinfo.biPlanes = 1;  
    bmpinfo.biBitCount = bpp;  
    bmpinfo.biCompression = BI_RGB;  
    bmpinfo.biSizeImage = (width*bpp+31)/32*4*height;  
    bmpinfo.biXPelsPerMeter = 100;  
    bmpinfo.biYPelsPerMeter = 100;  
    bmpinfo.biClrUsed = 0;  
    bmpinfo.biClrImportant = 0;  
  
    fwrite(&bmpheader, sizeof(bmpheader), 1, fp);  
    fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);  
    fwrite(pFrameRGB->data[0], width*height*bpp/8, 1, fp);  
  
    fclose(fp);  
}  


DWORD WINAPI rtsp2mp4(LPVOID pParam)
{
	CString TempCString;
	ffmpeg *pffmpeg = (ffmpeg*)(pParam);

	AVFormatContext *pAVFormatContext_Input;
	AVCodecContext *pAVCodecContext_Input;
	AVStream *pAVStream_InputVideo;
	int VideoWidth = 0;
	int VideoHeight = 0;

    avcodec_register_all();
    av_register_all();
    avformat_network_init();
	FrameCounter = 0;
    /* should set to NULL so that avformat_open_input() allocate a new one */
    pAVFormatContext_Input = NULL;
	pffmpeg->Printf((CString)("设置URL为rtsp://192.168.0.103:8888/stream\r\n"));
	pffmpeg->Printf((CString)("设置保存文件到Output/1.H264\r\n"));
    //char RTSPUrl[] = "rtsp://192.168.0.103:8888/stream";
	char RTSPUrl[] = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
	CFile H264File(_T("../Output/2.H264"),CFile::modeCreate | CFile::modeReadWrite);

	TempCString.Format(_T("打开URL获取一个输入AVFormatContextr\n"));
	pffmpeg->Printf(TempCString);
	/* 打开URL */
    if (avformat_open_input(&pAVFormatContext_Input, RTSPUrl, NULL, NULL)!=0)
    {
        pffmpeg->Printf((CString)("avformat_open_input failed\r\n"));
        return -1;
    }
	/* 找到流信息 */
    if (avformat_find_stream_info(pAVFormatContext_Input, NULL)<0)
    {
        pffmpeg->Printf((CString)("avformat_find_stream_info failed\r\n"));
        return -1;
    }
	/* 找到视频流 */
	TempCString.Format(_T("码流数量 = %d\r\n"),pAVFormatContext_Input->nb_streams);
	pffmpeg->Printf(TempCString);
    for (unsigned i=0; i<pAVFormatContext_Input->nb_streams; i++)
    {
		TempCString.Format(_T("码流(%d)->codec->codec_type = %d\r\n"),i,pAVFormatContext_Input->streams[i]->codec->codec_type);
		pffmpeg->Printf(TempCString);
	}
    
    for (unsigned i=0; i<pAVFormatContext_Input->nb_streams; i++)
    {
        if (pAVFormatContext_Input->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
			TempCString.Format(_T("找到视频流 = (%d)\r\n"),i);
			pffmpeg->Printf(TempCString);
            pAVStream_InputVideo = pAVFormatContext_Input->streams[i];
            break;
        }
    }
    if (pAVStream_InputVideo == NULL)
    {
		pffmpeg->Printf((CString)("无法找到视频流\r\n"));
        return -1;
    }
	/* 获取视频流信息 */
    {
		TempCString.Format(_T("编码器ID:%d,编码器类型:%d\r\n"),pAVStream_InputVideo->codec->codec_id,pAVStream_InputVideo->codec->codec_type);
		pffmpeg->Printf(TempCString);

		TempCString.Format(_T("time_base.num:%d,time_base.den:%d\r\n"),pAVStream_InputVideo->time_base.num,pAVStream_InputVideo->time_base.den);
		pffmpeg->Printf(TempCString);

		VideoWidth = pAVStream_InputVideo->codec->width;
		VideoHeight = pAVStream_InputVideo->codec->height;
		TempCString.Format(_T("宽:%d,高:%d\r\n"),pAVStream_InputVideo->codec->width,pAVStream_InputVideo->codec->height);
		pffmpeg->Printf(TempCString);

		TempCString.Format(_T("像素格式:%d\r\n"),pAVStream_InputVideo->codec->pix_fmt);
		pffmpeg->Printf(TempCString);
    }
	pAVCodecContext_Input = pAVStream_InputVideo->codec;
	/* 通过Stream找到解码器 */
	pffmpeg->Printf((CString)("查找码流对应的解码器\r\n"));
	AVCodec *pAVCodec;
    pAVCodec = avcodec_find_decoder(pAVCodecContext_Input->codec_id);	//AV_CODEC_ID_H264
	pffmpeg->Printf((CString)("打开解码器\r\n"));
	avcodec_open2(pAVCodecContext_Input,pAVCodec,NULL);

	AVFrame *pFrame_ = av_frame_alloc();// Allocate video frame  
	AVFrame *pFrameRGB = av_frame_alloc();
	uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_BGR24, pAVCodecContext_Input->width, pAVCodecContext_Input->height));
    if( out_buffer == NULL ) {  
        TRACE("av malloc failed!\n");  
        exit(1);  
    }  
	avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_BGR24, pAVCodecContext_Input->width, pAVCodecContext_Input->height);

	struct SwsContext *img_convert_ctx = sws_getContext(pAVCodecContext_Input->width, pAVCodecContext_Input->height, pAVCodecContext_Input->pix_fmt, 
        pAVCodecContext_Input->width, pAVCodecContext_Input->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); 

	int frameFinished;

    AVPacket i_pkt;
    
    while (!bStop)
    {

		av_init_packet(&i_pkt);
        if (av_read_frame(pAVFormatContext_Input, &i_pkt) <0 ){
			pffmpeg->Printf((CString)("av_read_frame failed\r\n"));
			break;
		}else{
			H264File.Write(i_pkt.data,i_pkt.size);
			avcodec_decode_video2(pAVCodecContext_Input, pFrame_, &frameFinished, &i_pkt);  
			if(frameFinished){
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame_->data, pFrame_->linesize, 0, pAVCodecContext_Input->height, 
				pFrameRGB->data, pFrameRGB->linesize);

				if(FrameCounter>100&&FrameCounter<120)
					SaveAsBMP(pFrameRGB, pAVCodecContext_Input->width, pAVCodecContext_Input->height, FrameCounter, 24);  

				TRACE(_T("Frame %d\n"), FrameCounter++);
			
			}
			
		}
    }

	H264File.Close();
	av_free_packet(&i_pkt);
    avformat_close_input(&pAVFormatContext_Input);
	pffmpeg->Printf((CString)("关闭输入流内容AVFormatContext\r\n"));

	pffmpeg->Printf((CString)("rtsp2mp4线程结束\r\n"));
    return 0;
}

void ffmpeg::ffmpeg_end(void)
{
	bStop = true;
}
void ffmpeg::ffmpeg_start(void)
{

}
void ffmpeg::ffmpeg_init(void)
{
	this->Printf((CString)("ffmpeg_init\r\n"));

    bStop = false;

	AfxBeginThread((AFX_THREADPROC)rtsp2mp4,this,THREAD_PRIORITY_HIGHEST);
}
