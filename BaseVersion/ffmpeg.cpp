#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"
#include "BaseVersionDlg.h"
#include "ffmpeg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"

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
	this->g_FrameCounter = 0;
	g_StartRecvRTSPFlag = 0;
}

ffmpeg::~ffmpeg(){

}
/*************************************************************************************/
//主窗体操作
void ffmpeg::Printf(CString Context){
	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
	CString m_strTime = time.Format("[%H:%M:%S]");

	Context = m_strTime + (CString)("[ffmpeg]") + Context;
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->Printf(Context);
}

void ffmpeg::UpdateFrameInfo(unsigned int Num)
{
	CString TempCString;
	TempCString.Format(_T("%u"),Num);
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->UIOperationCB(IDC_EDIT_FrameShow,TempCString);
}
//保存RGB24到BMP文件的函数  
void ffmpeg::SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int index, int bpp)  
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
    bmpinfo.biHeight = -height;  //不然图像会翻转
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


DWORD WINAPI RecvRTSPThread(LPVOID pParam)
{
	CString TempCString;
	ffmpeg *pffmpeg = (ffmpeg*)(pParam);

	AVFormatContext *pAVFormatContext_Input;
	AVCodecContext *pAVCodecContext_Input;
	AVStream *pAVStream_InputVideo;
	int VideoWidth = 0;
	int VideoHeight = 0;

	pffmpeg->g_FrameCounter = 0;
    /* should set to NULL so that avformat_open_input() allocate a new one */
    pAVFormatContext_Input = NULL;

	USES_CONVERSION;
	char *g_URLString = T2A(pffmpeg->g_URLCString);

	TempCString.Format(_T("设置URL:%s\r\n"),(CString)g_URLString);
	pffmpeg->Printf(TempCString);
	TempCString.Format(_T("设置保存文件到:%s\r\n"),(CString)CFG_SAVE_H264_PATH);
	pffmpeg->Printf(TempCString);
	CFile H264File(_T(CFG_SAVE_H264_PATH),CFile::modeCreate | CFile::modeReadWrite);

	TempCString.Format(_T("打开URL获取一个输入AVFormat\r\n"));
	pffmpeg->Printf(TempCString);
	/* 打开URL */
    char RTSPUrl[] = "rtsp://192.168.0.103:8888/stream";
	//char RTSPUrl[] = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
	//if (avformat_open_input(&pAVFormatContext_Input, RTSPUrl, NULL, NULL)!=0)
    if (avformat_open_input(&pAVFormatContext_Input, g_URLString, NULL, NULL)!=0)
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
	TempCString.Format(_T("码流AVStream数量 = %d\r\n"),pAVFormatContext_Input->nb_streams);
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
			TempCString.Format(_T("找到视频流VStream = (%d)\r\n"),i);
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

		TempCString.Format(_T("时钟基准.num:%d,时钟基准.den:%d\r\n"),pAVStream_InputVideo->time_base.num,pAVStream_InputVideo->time_base.den);
		pffmpeg->Printf(TempCString);

		VideoWidth = pAVStream_InputVideo->codec->width;
		VideoHeight = pAVStream_InputVideo->codec->height;
		TempCString.Format(_T("宽:%d,高:%d\r\n"),pAVStream_InputVideo->codec->width,pAVStream_InputVideo->codec->height);
		pffmpeg->Printf(TempCString);

		TempCString.Format(_T("像素格式:%d\r\n"),pAVStream_InputVideo->codec->pix_fmt);
		pffmpeg->Printf(TempCString);
    }
	pffmpeg->Printf((CString)("获取流对应的编码器AVCodec\r\n"));
	pAVCodecContext_Input = pAVStream_InputVideo->codec;
	/* 通过Stream找到解码器 */

	AVCodec *pAVCodec;
    pAVCodec = avcodec_find_decoder(pAVCodecContext_Input->codec_id);	//AV_CODEC_ID_H264
	pffmpeg->Printf((CString)("找到VStream对应的解码器AV_CODEC_ID_H264\r\n"));
	avcodec_open2(pAVCodecContext_Input,pAVCodec,NULL);//链接H264编码器类型相同的解码器

	AVFrame *pFrameSrc = av_frame_alloc();//原始解码帧数据YUV420
	AVFrame *pFrameRGB = av_frame_alloc();;//转换帧数据RGB24
	uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_BGR24, pAVCodecContext_Input->width, pAVCodecContext_Input->height));//创建RGB24Buffer
    if( out_buffer == NULL ) {  
        pffmpeg->Printf((CString)("av_malloc失败\r\n"));
        exit(1);  
    }  
	avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_BGR24, pAVCodecContext_Input->width, pAVCodecContext_Input->height);//指定该Buffer给pFrameRGB使用

	struct SwsContext *img_convert_ctx = sws_getContext(pAVCodecContext_Input->width, pAVCodecContext_Input->height, pAVCodecContext_Input->pix_fmt, 
        pAVCodecContext_Input->width, pAVCodecContext_Input->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); 
	//创建图像转换参数，H264解码之后是YUV420,这里我们需要转成RGB24的格式
	int frameFinished;
    AVPacket AVPackage_Input;	//数据最初开始是AVPackage的格式，里面存放H264
    
    while (pffmpeg->g_StartRecvRTSPFlag)
    {

		av_init_packet(&AVPackage_Input);
        if (av_read_frame(pAVFormatContext_Input, &AVPackage_Input) <0 ){	//读取一帧数据AVPackage
			pffmpeg->Printf((CString)("av_read_frame获取帧失败\r\n"));
		}else{
			H264File.Write(AVPackage_Input.data,AVPackage_Input.size);
			avcodec_decode_video2(pAVCodecContext_Input, pFrameSrc, &frameFinished, &AVPackage_Input);  //解码一帧AVPackage_Input数据，放入到pFrameSrc,frameFinished=1表示解码成功
			if(frameFinished){
				pffmpeg->g_FrameCounter++;

				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrameSrc->data, pFrameSrc->linesize, 0, pAVCodecContext_Input->height, 
				pFrameRGB->data, pFrameRGB->linesize);//根据img_convert_ctx句柄参数，进行帧的缩放，从YUV420帧生成RGB24格式的帧

				if(pffmpeg->g_FrameCounter>100&&pffmpeg->g_FrameCounter<120)
					pffmpeg->SaveAsBMP(pFrameRGB, pAVCodecContext_Input->width, pAVCodecContext_Input->height, pffmpeg->g_FrameCounter, 24);  //将RGB24的数据写入BMP

				//TRACE(_T("Frame %lu\n"), pffmpeg->g_FrameCounter);
			
			}
			
		}
    }

	H264File.Close();
	av_free_packet(&AVPackage_Input);//释放AVPackage
    avformat_close_input(&pAVFormatContext_Input);//关闭输入AVFormat
	pffmpeg->Printf((CString)("释放AVPackage,关闭AVFormat\r\n"));

	pffmpeg->Printf((CString)("RecvRTSPThread线程结束\r\n"));
    return 0;
}
DWORD WINAPI UpdateUIThread(LPVOID pParam){
	ffmpeg *pffmpeg = (ffmpeg*)pParam;
	while(pffmpeg->g_StartRecvRTSPFlag){
		pffmpeg->UpdateFrameInfo(pffmpeg->g_FrameCounter);
		Sleep(500);
	}
	return 0;
}

void ffmpeg::ffmpeg_start(CString URLCString)
{

	g_URLCString = URLCString;
	g_StartRecvRTSPFlag = true;
	AfxBeginThread((AFX_THREADPROC)RecvRTSPThread,this,THREAD_PRIORITY_HIGHEST);
	AfxBeginThread((AFX_THREADPROC)UpdateUIThread,this,THREAD_PRIORITY_HIGHEST);
}
void ffmpeg::ffmpeg_end(void)
{
	g_StartRecvRTSPFlag = false;
}
void ffmpeg::ffmpeg_init(void)
{
	this->Printf((CString)("avcodec_register_all\r\n"));
	this->Printf((CString)("av_register_all\r\n"));
	this->Printf((CString)("avformat_network_init\r\n"));

	avcodec_register_all();
    av_register_all();
    avformat_network_init();


}
