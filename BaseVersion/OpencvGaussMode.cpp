#include "stdafx.h"
#include "resource.h"

#include "opencv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"

#include <iostream>
using namespace std;

#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include <opencv2/opencv.hpp>
#include <cvaux.h>//��������ͷ�ļ�  
#include "OpencvGaussMode.h"

IplImage *g_pGaussImage = NULL;
IplImage *g_pGaussScaleImage = NULL;
IplImage *g_pForeScaleImage = NULL;

CvGaussBGModel *g_GaussModel;

#define GAUSS_SCALE 1

OpencvGaussMode::OpencvGaussMode()
{

}

OpencvGaussMode::~OpencvGaussMode()
{

}


void OpencvGaussMode::SaveImage(IplImage *Image)
{
	CString TempCString;
	CString DirCString;
	CString TimeCString = FormatChange::GetTimeCString();

	DirCString.Format(_T("../Output/%s"),TimeCString);
	if (!PathIsDirectory(DirCString))  
	{  
		::CreateDirectory(DirCString, NULL);//����Ŀ¼,���еĻ���Ӱ��  
	}  

	TempCString.Format(_T("../Output/%s/%u.jpg"),TimeCString,this->g_SaveImageCounter);
	char *SavePath = FormatChange::CStringToChar(TempCString);
	cvSaveImage(SavePath,Image,0);
}


DWORD WINAPI GaussThread(LPVOID pParam)
{
	OpencvGaussMode *pOpencvGaussMode = (OpencvGaussMode*)(pParam);

	while(pOpencvGaussMode->g_StartFlag){
		WaitForSingleObject(pOpencvGaussMode->hSemaphore, INFINITE);

		EnterCriticalSection(&pOpencvGaussMode->g_CS);		// �����ٽ���

		cvUpdateBGStatModel(g_pGaussScaleImage,(CvBGStatModel*)g_GaussModel,0);//���¸�˹

		memcpy(g_pForeScaleImage->imageData,g_GaussModel->foreground->imageData,g_pForeScaleImage->height*g_pForeScaleImage->width);

		cvShowImage("Gauss��˹����", g_pForeScaleImage);

		pOpencvGaussMode->g_BlackPixelRate = pOpencvGaussMode->BinBlackPixelRate(g_pForeScaleImage);		//��ȡ�ڵ�ĸ���

		LeaveCriticalSection(&pOpencvGaussMode->g_CS);		// �뿪�ٽ���
	}
	return 0;
}
/*
void on_trackbar(int threshold)  
{  
   g_ThreadCandy = threshold;
} 
*/
void OpencvGaussMode::Start(void)
{
	this->g_StartFlag = 1;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;

	cvNamedWindow("Gauss��˹����",CV_WINDOW_AUTOSIZE);

	InitializeCriticalSection(&g_CS);	

	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	
	AfxBeginThread((AFX_THREADPROC)GaussThread,this,THREAD_PRIORITY_NORMAL);
	//int nThresholdEdge = 1;  
    //cvCreateTrackbar("Candy��ֵ",CFG_OPENCV_RTSP_WIN, &nThresholdEdge, 100, on_trackbar);	//���ӱ�Ե�����ֵ�������ڣ��ص�������on_trackbar
	//on_trackbar(1); 
}

void OpencvGaussMode::ReleaseIplImage(IplImage* &IplIma)
{
	if(IplIma!=NULL){
		cvReleaseImage(&IplIma);
		IplIma = NULL;
	}
}

void OpencvGaussMode::Stop(void)
{
	EnterCriticalSection(&g_CS);		// �����ٽ���

	this->g_StartFlag = 0;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;


	cvDestroyWindow("Gauss��˹����");

	ReleaseIplImage(g_pGaussImage);	
	ReleaseIplImage(g_pGaussScaleImage);
	ReleaseIplImage(g_pForeScaleImage);

	LeaveCriticalSection(&g_CS);		// �뿪�ٽ���
}

void OpencvGaussMode::Handle(IplImage *ParamImage)
{
	EnterCriticalSection(&g_CS);		// �����ٽ���
	unsigned int width = ParamImage->width;
	unsigned height = ParamImage->height;

	if(this->g_StartFlag == 0)
		return;

	this->g_FrameCounter++;

	if(g_FrameCounter == 1){		//��һ֡����������ͼ��
		g_pGaussImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
		g_pGaussScaleImage = cvCreateImage(cvSize(width/GAUSS_SCALE,height/GAUSS_SCALE),IPL_DEPTH_8U ,3);
		g_pForeScaleImage = cvCreateImage(cvGetSize(g_pGaussScaleImage), IPL_DEPTH_8U, 1);  

        CvGaussBGStatModelParams params; 
        params.win_size = CV_BGFG_MOG_WINDOW_SIZE;             //Learning rate = 1/win_size;
        params.bg_threshold = CV_BGFG_MOG_BACKGROUND_THRESHOLD;         //��˹�ֲ�Ȩ��֮�͵���ֵ.0.333*R(��˹)+0.333*G(��˹)+0.333*B(��˹)
        params.weight_init = CV_BGFG_MOG_WEIGHT_INIT;               //��ʼ��Ȩ��1/3
        params.variance_init = CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;               //��ʼ������
        params.minArea = CV_BGFG_MOG_MINAREA; //��С���,�������������ȥ��ģ�������Ŀ�������������С�����minAreaʱ,�Ͱ���������ȥ�����������ȥ��СĿ��
        params.n_gauss = CV_BGFG_MOG_NGAUSSIANS;                    //��˹�ֲ���������3(R/G/B)
        params.std_threshold = CV_BGFG_MOG_STD_THRESHOLD;    //2.5
        memcpy(g_pGaussImage->imageData,ParamImage->imageData,width*height*3);
        cvResize(g_pGaussImage,g_pGaussScaleImage);
        g_GaussModel = (CvGaussBGModel*)cvCreateGaussianBGModel(g_pGaussScaleImage,&params);   //Create Mode

	}else{

		if(g_pGaussImage == NULL)
			return ;


	#if 1
		if(this->g_FrameCounter % 10 == 0){		//ÿ60֡������һ��Candy���
			memcpy(g_pGaussImage->imageData,ParamImage->imageData,width*height*3);
			cvResize(g_pGaussImage,g_pGaussScaleImage);
			ReleaseSemaphore(hSemaphore, 1, NULL);
		}
	#endif
		if(cvWaitKey(10)=='A'){

		}
	}
	LeaveCriticalSection(&g_CS);		// �뿪�ٽ���
}

