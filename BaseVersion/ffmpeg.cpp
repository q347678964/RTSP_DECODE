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
//���졢��������
ffmpeg::ffmpeg(){
	this->g_FrameCounter = 0;
	g_StartRecvRTSPFlag = 0;
}

ffmpeg::~ffmpeg(){

}
/*************************************************************************************/
//���������
void ffmpeg::Printf(CString Context){
	CTime time = CTime::GetCurrentTime();   ///����CTime����
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
//����RGB24��BMP�ļ��ĺ���  
void ffmpeg::SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int index, int bpp)  
{  
    char buf[5] = {0};  
    BITMAPFILEHEADER bmpheader;  
    BITMAPINFOHEADER bmpinfo;  
    FILE *fp;  
  
    char *filename = new char[255];  
  
    //�ļ����·���������Լ����޸�  
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
    bmpinfo.biHeight = -height;  //��Ȼͼ��ᷭת
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

	TempCString.Format(_T("����URL:%s\r\n"),(CString)g_URLString);
	pffmpeg->Printf(TempCString);
	TempCString.Format(_T("���ñ����ļ���:%s\r\n"),(CString)CFG_SAVE_H264_PATH);
	pffmpeg->Printf(TempCString);
	CFile H264File(_T(CFG_SAVE_H264_PATH),CFile::modeCreate | CFile::modeReadWrite);

	TempCString.Format(_T("��URL��ȡһ������AVFormat\r\n"));
	pffmpeg->Printf(TempCString);
	/* ��URL */
    char RTSPUrl[] = "rtsp://192.168.0.103:8888/stream";
	//char RTSPUrl[] = "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
	//if (avformat_open_input(&pAVFormatContext_Input, RTSPUrl, NULL, NULL)!=0)
    if (avformat_open_input(&pAVFormatContext_Input, g_URLString, NULL, NULL)!=0)
    {
        pffmpeg->Printf((CString)("avformat_open_input failed\r\n"));
        return -1;
    }
	/* �ҵ�����Ϣ */
    if (avformat_find_stream_info(pAVFormatContext_Input, NULL)<0)
    {
        pffmpeg->Printf((CString)("avformat_find_stream_info failed\r\n"));
        return -1;
    }
	/* �ҵ���Ƶ�� */
	TempCString.Format(_T("����AVStream���� = %d\r\n"),pAVFormatContext_Input->nb_streams);
	pffmpeg->Printf(TempCString);
    for (unsigned i=0; i<pAVFormatContext_Input->nb_streams; i++)
    {
		TempCString.Format(_T("����(%d)->codec->codec_type = %d\r\n"),i,pAVFormatContext_Input->streams[i]->codec->codec_type);
		pffmpeg->Printf(TempCString);
	}
    
    for (unsigned i=0; i<pAVFormatContext_Input->nb_streams; i++)
    {
        if (pAVFormatContext_Input->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
			TempCString.Format(_T("�ҵ���Ƶ��VStream = (%d)\r\n"),i);
			pffmpeg->Printf(TempCString);
            pAVStream_InputVideo = pAVFormatContext_Input->streams[i];
            break;
        }
    }
    if (pAVStream_InputVideo == NULL)
    {
		pffmpeg->Printf((CString)("�޷��ҵ���Ƶ��\r\n"));
        return -1;
    }
	/* ��ȡ��Ƶ����Ϣ */
    {
		TempCString.Format(_T("������ID:%d,����������:%d\r\n"),pAVStream_InputVideo->codec->codec_id,pAVStream_InputVideo->codec->codec_type);
		pffmpeg->Printf(TempCString);

		TempCString.Format(_T("ʱ�ӻ�׼.num:%d,ʱ�ӻ�׼.den:%d\r\n"),pAVStream_InputVideo->time_base.num,pAVStream_InputVideo->time_base.den);
		pffmpeg->Printf(TempCString);

		VideoWidth = pAVStream_InputVideo->codec->width;
		VideoHeight = pAVStream_InputVideo->codec->height;
		TempCString.Format(_T("��:%d,��:%d\r\n"),pAVStream_InputVideo->codec->width,pAVStream_InputVideo->codec->height);
		pffmpeg->Printf(TempCString);

		TempCString.Format(_T("���ظ�ʽ:%d\r\n"),pAVStream_InputVideo->codec->pix_fmt);
		pffmpeg->Printf(TempCString);
    }
	pffmpeg->Printf((CString)("��ȡ����Ӧ�ı�����AVCodec\r\n"));
	pAVCodecContext_Input = pAVStream_InputVideo->codec;
	/* ͨ��Stream�ҵ������� */

	AVCodec *pAVCodec;
    pAVCodec = avcodec_find_decoder(pAVCodecContext_Input->codec_id);	//AV_CODEC_ID_H264
	pffmpeg->Printf((CString)("�ҵ�VStream��Ӧ�Ľ�����AV_CODEC_ID_H264\r\n"));
	avcodec_open2(pAVCodecContext_Input,pAVCodec,NULL);//����H264������������ͬ�Ľ�����

	AVFrame *pFrameSrc = av_frame_alloc();//ԭʼ����֡����YUV420
	AVFrame *pFrameRGB = av_frame_alloc();;//ת��֡����RGB24
	uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_BGR24, pAVCodecContext_Input->width, pAVCodecContext_Input->height));//����RGB24Buffer
    if( out_buffer == NULL ) {  
        pffmpeg->Printf((CString)("av_mallocʧ��\r\n"));
        exit(1);  
    }  
	avpicture_fill((AVPicture *)pFrameRGB, out_buffer, AV_PIX_FMT_BGR24, pAVCodecContext_Input->width, pAVCodecContext_Input->height);//ָ����Buffer��pFrameRGBʹ��

	struct SwsContext *img_convert_ctx = sws_getContext(pAVCodecContext_Input->width, pAVCodecContext_Input->height, pAVCodecContext_Input->pix_fmt, 
        pAVCodecContext_Input->width, pAVCodecContext_Input->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL); 
	//����ͼ��ת��������H264����֮����YUV420,����������Ҫת��RGB24�ĸ�ʽ
	int frameFinished;
    AVPacket AVPackage_Input;	//���������ʼ��AVPackage�ĸ�ʽ��������H264
    
    while (pffmpeg->g_StartRecvRTSPFlag)
    {

		av_init_packet(&AVPackage_Input);
        if (av_read_frame(pAVFormatContext_Input, &AVPackage_Input) <0 ){	//��ȡһ֡����AVPackage
			pffmpeg->Printf((CString)("av_read_frame��ȡ֡ʧ��\r\n"));
		}else{
			H264File.Write(AVPackage_Input.data,AVPackage_Input.size);
			avcodec_decode_video2(pAVCodecContext_Input, pFrameSrc, &frameFinished, &AVPackage_Input);  //����һ֡AVPackage_Input���ݣ����뵽pFrameSrc,frameFinished=1��ʾ����ɹ�
			if(frameFinished){
				pffmpeg->g_FrameCounter++;

				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrameSrc->data, pFrameSrc->linesize, 0, pAVCodecContext_Input->height, 
				pFrameRGB->data, pFrameRGB->linesize);//����img_convert_ctx�������������֡�����ţ���YUV420֡����RGB24��ʽ��֡

				if(pffmpeg->g_FrameCounter>100&&pffmpeg->g_FrameCounter<120)
					pffmpeg->SaveAsBMP(pFrameRGB, pAVCodecContext_Input->width, pAVCodecContext_Input->height, pffmpeg->g_FrameCounter, 24);  //��RGB24������д��BMP

				//TRACE(_T("Frame %lu\n"), pffmpeg->g_FrameCounter);
			
			}
			
		}
    }

	H264File.Close();
	av_free_packet(&AVPackage_Input);//�ͷ�AVPackage
    avformat_close_input(&pAVFormatContext_Input);//�ر�����AVFormat
	pffmpeg->Printf((CString)("�ͷ�AVPackage,�ر�AVFormat\r\n"));

	pffmpeg->Printf((CString)("RecvRTSPThread�߳̽���\r\n"));
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
