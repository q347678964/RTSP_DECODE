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
#include <cvaux.h>//必须引此头文件  
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
		::CreateDirectory(DirCString, NULL);//创建目录,已有的话不影响  
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

		cvUpdateBGStatModel(g_pGaussScaleImage,(CvBGStatModel*)g_GaussModel,0);//更新高斯

		cvErode(g_GaussModel->foreground, g_pForeScaleImage, NULL,1); //腐蚀  

		cvErode(g_pForeScaleImage, g_pForeScaleImage, NULL,1); //腐蚀  
		//memcpy(g_pForeScaleImage->imageData,g_GaussModel->foreground->imageData,g_pForeScaleImage->height*g_pForeScaleImage->width);

		cvShowImage("Gauss高斯前景窗口", g_pForeScaleImage);

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
			cvShowImage("Gauss高斯源窗口", g_pGaussScaleImage);
#endif
		}

		pOpencvGaussMode->g_BlackPixelRate = pOpencvGaussMode->BinBlackPixelRate(g_pForeScaleImage);		//获取黑点的概率

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
	cvNamedWindow("Gauss高斯前景窗口",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Gauss高斯源窗口",CV_WINDOW_AUTOSIZE);

	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

	AreaCombine.Start();

	AfxBeginThread((AFX_THREADPROC)GaussThread,this,THREAD_PRIORITY_NORMAL);

	//int nThresholdEdge = 1;  
    //cvCreateTrackbar("Candy阈值",CFG_OPENCV_RTSP_WIN, &nThresholdEdge, 100, on_trackbar);	//增加边缘检测阈值到窗体内，回调函数是on_trackbar
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
	cvDestroyWindow("Gauss高斯源窗口");
	cvDestroyWindow("Gauss高斯前景窗口");
	cvReleaseBGStatModel((CvBGStatModel**)&g_GaussModel);
	ReleaseIplImage(g_pGaussImage);	
	ReleaseIplImage(g_pGaussScaleImage);
	ReleaseIplImage(g_pForeScaleImage);

}

void OpencvGaussMode::Handle(IplImage *ParamImage)
{
	if(this->g_StartFlag == 0)
		return;

	if(this->g_HandlingFlag){	//正在处理，不处理该帧
		g_LoseFrameCounter++;
		return;
	}

	unsigned int width = ParamImage->width;
	unsigned height = ParamImage->height;

	this->g_FrameCounter++;

	if(g_FrameCounter == 1){		//第一帧数据来创建图像
		g_pGaussImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
		g_pGaussScaleImage = cvCreateImage(cvSize(width/GAUSS_SCALE,height/GAUSS_SCALE),IPL_DEPTH_8U ,3);
		g_pForeScaleImage = cvCreateImage(cvGetSize(g_pGaussScaleImage), IPL_DEPTH_8U, 1);  

        CvGaussBGStatModelParams params; 
        params.win_size = 2000;//CV_BGFG_MOG_WINDOW_SIZE;             //Learning rate = 1/win_size;
        params.bg_threshold = 0.7;//CV_BGFG_MOG_BACKGROUND_THRESHOLD;         //判定是否为背景点的阈值，0~1越大，背景越少
        params.weight_init = 0.3;               //初始化权重1/3
        params.variance_init = CV_BGFG_MOG_SIGMA_INIT;//CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;               //初始化方差
        params.minArea = 1; //最小面积,这个参数是用来去噪的，当检测的目标矩形区域的面积小于这个minArea时,就把它当噪音去掉，这个就是去掉小目标
        params.n_gauss = 3;//CV_BGFG_MOG_NGAUSSIANS;                    //高斯分布函数个数3(R/G/B)
        params.std_threshold = 2.5;//CV_BGFG_MOG_STD_THRESHOLD;    //2.5
        memcpy(g_pGaussImage->imageData,ParamImage->imageData,width*height*3);
        cvResize(g_pGaussImage,g_pGaussScaleImage);
        g_GaussModel = (CvGaussBGModel*)cvCreateGaussianBGModel(g_pGaussScaleImage,0);   //Create Mode

	}else{

		if(g_pGaussImage == NULL)
			return ;

	#if 1
		//if(this->g_FrameCounter % 1 == 0){		//每60帧，更新一次Candy检测
			memcpy(g_pGaussImage->imageData,ParamImage->imageData,width*height*3);
			cvResize(g_pGaussImage,g_pGaussScaleImage);
			//cvShowImage("Gauss高斯源窗口", g_pGaussScaleImage);
			ReleaseSemaphore(hSemaphore, 1, NULL);
		//}
	#endif
		if(cvWaitKey(2)=='A'){

		}
	}
}

