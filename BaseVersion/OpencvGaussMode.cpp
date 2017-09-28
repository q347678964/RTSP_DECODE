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
#include "BinaryAreaCombine.h"

IplImage *g_pGaussImage = NULL;
IplImage *g_pGaussScaleImage = NULL;
IplImage *g_pForeScaleImage = NULL;

CvGaussBGModel *g_GaussModel;
BinaryAreaCombine AreaCombine;

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
		pOpencvGaussMode->g_HandlingFlag = 1;

		cvUpdateBGStatModel(g_pGaussScaleImage,(CvBGStatModel*)g_GaussModel,0);//���¸�˹

		cvErode(g_GaussModel->foreground, g_pForeScaleImage, NULL,1); //��ʴ  

		cvErode(g_pForeScaleImage, g_pForeScaleImage, NULL,1); //��ʴ  
		//memcpy(g_pForeScaleImage->imageData,g_GaussModel->foreground->imageData,g_pForeScaleImage->height*g_pForeScaleImage->width);

		cvShowImage("Gauss��˹ǰ������", g_pForeScaleImage);

		{
#if 0
			AreaCombine.AreaCombine(g_pForeScaleImage);
#else
			#define SortNum 10
			unsigned int SortMaxtoMin[SortNum];
			unsigned int RealAreaNum;
			AreaCombine.AreaCombine(g_pForeScaleImage);
			RealAreaNum = AreaCombine.AreaMaxToMinSort(SortMaxtoMin,SortNum);
			IMAGEAREA AreaInfo;
			for(int i=0;i<RealAreaNum;i++){
				AreaCombine.GetAreaInfo(&AreaInfo,SortMaxtoMin[i]);
				cvRectangle(g_pGaussScaleImage,  
				cvPoint(AreaInfo.StartX,AreaInfo.StartY),  
				cvPoint(AreaInfo.EndX,AreaInfo.EndY),  
				CV_RGB(255,0,0));    
			}
			cvShowImage("Gauss��˹Դ����", g_pGaussScaleImage);
#endif
		}

		pOpencvGaussMode->g_BlackPixelRate = pOpencvGaussMode->BinBlackPixelRate(g_pForeScaleImage);		//��ȡ�ڵ�ĸ���

		pOpencvGaussMode->g_HandlingFlag = 0;
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
	this->g_HandlingFlag = 0;
	this->g_LoseFrameCounter = 0;
	cvNamedWindow("Gauss��˹ǰ������",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Gauss��˹Դ����",CV_WINDOW_AUTOSIZE);

	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

	AreaCombine.Start();

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
	while(this->g_HandlingFlag)
		Sleep(500);

	AreaCombine.Stop();

	this->g_StartFlag = 0;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;
	this->g_LoseFrameCounter = 0;
	cvDestroyWindow("Gauss��˹Դ����");
	cvDestroyWindow("Gauss��˹ǰ������");
	cvReleaseBGStatModel((CvBGStatModel**)&g_GaussModel);
	ReleaseIplImage(g_pGaussImage);	
	ReleaseIplImage(g_pGaussScaleImage);
	ReleaseIplImage(g_pForeScaleImage);

}

void OpencvGaussMode::Handle(IplImage *ParamImage)
{
	if(this->g_StartFlag == 0)
		return;

	if(this->g_HandlingFlag){	//���ڴ����������֡
		g_LoseFrameCounter++;
		return;
	}

	unsigned int width = ParamImage->width;
	unsigned height = ParamImage->height;

	this->g_FrameCounter++;

	if(g_FrameCounter == 1){		//��һ֡����������ͼ��
		g_pGaussImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
		g_pGaussScaleImage = cvCreateImage(cvSize(width/GAUSS_SCALE,height/GAUSS_SCALE),IPL_DEPTH_8U ,3);
		g_pForeScaleImage = cvCreateImage(cvGetSize(g_pGaussScaleImage), IPL_DEPTH_8U, 1);  

        CvGaussBGStatModelParams params; 
        params.win_size = 2000;//CV_BGFG_MOG_WINDOW_SIZE;             //Learning rate = 1/win_size;
        params.bg_threshold = 0.7;//CV_BGFG_MOG_BACKGROUND_THRESHOLD;         //�ж��Ƿ�Ϊ���������ֵ��0~1Խ�󣬱���Խ��
        params.weight_init = 0.3;               //��ʼ��Ȩ��1/3
        params.variance_init = CV_BGFG_MOG_SIGMA_INIT;//CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;               //��ʼ������
        params.minArea = 1; //��С���,�������������ȥ��ģ�������Ŀ�������������С�����minAreaʱ,�Ͱ���������ȥ�����������ȥ��СĿ��
        params.n_gauss = 3;//CV_BGFG_MOG_NGAUSSIANS;                    //��˹�ֲ���������3(R/G/B)
        params.std_threshold = 2.5;//CV_BGFG_MOG_STD_THRESHOLD;    //2.5
        memcpy(g_pGaussImage->imageData,ParamImage->imageData,width*height*3);
        cvResize(g_pGaussImage,g_pGaussScaleImage);
        g_GaussModel = (CvGaussBGModel*)cvCreateGaussianBGModel(g_pGaussScaleImage,0);   //Create Mode

	}else{

		if(g_pGaussImage == NULL)
			return ;

	#if 1
		//if(this->g_FrameCounter % 1 == 0){		//ÿ60֡������һ��Candy���
			memcpy(g_pGaussImage->imageData,ParamImage->imageData,width*height*3);
			cvResize(g_pGaussImage,g_pGaussScaleImage);
			//cvShowImage("Gauss��˹Դ����", g_pGaussScaleImage);
			ReleaseSemaphore(hSemaphore, 1, NULL);
		//}
	#endif
		if(cvWaitKey(2)=='A'){

		}
	}
}

