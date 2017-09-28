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
#include "OpencvMotionDetection.h"

HANDLE hSemaphore;
static int g_ThreadCandy = 70;
IplImage *g_pSrcImage = NULL;
IplImage *g_pCannySrcImg = NULL;
IplImage *g_pCannyImg1st = NULL;
IplImage *g_pCannyImg2nd = NULL;
IplImage *g_pCannyImg3rd = NULL;
IplImage *g_pCandyImgErode = NULL;

OpencvMotionDetection::OpencvMotionDetection()
{

}
OpencvMotionDetection::~OpencvMotionDetection()
{

}


void OpencvMotionDetection::SaveImage(IplImage *Image)
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


DWORD WINAPI CandyThread(LPVOID pParam)
{
	OpencvMotionDetection *pOpencvMotionDetection = (OpencvMotionDetection*)(pParam);

	while(pOpencvMotionDetection->g_StartFlag){
		WaitForSingleObject(hSemaphore, INFINITE);

		pOpencvMotionDetection->g_HandlingFlag = 1;

		cvCanny(g_pCannySrcImg, g_pCannyImg1st, g_ThreadCandy, g_ThreadCandy * 3, 3);  //提取边缘
		cvAbsDiff(g_pCannyImg1st,g_pCannyImg2nd,g_pCannyImg3rd);			//与上一帧做图像绝对值减法g_pCannyImg3rd = abs(g_pCannyImg1st - g_pCannyImg2nd）
		memcpy(g_pCannyImg2nd->imageData,g_pCannyImg1st->imageData,g_pCannyImg1st->width*g_pCannyImg1st->height);//更新当前帧g_pCannyImg2nd = g_pCannyImg1st

		//cvErode( g_pCannyImg3rd,g_pCandyImgErode, NULL,1); //腐蚀  
		//cvSmooth(g_pCannyImg3rd,g_pCandyImgErode,CV_BLUR,3,1); //平均滤波
		cvSmooth(g_pCannyImg3rd,g_pCandyImgErode,CV_MEDIAN,3,1);  //  中值滤波

		cvShowImage("MotionDetect窗口", g_pCandyImgErode);

		pOpencvMotionDetection->g_BlackPixelRate = pOpencvMotionDetection->BinBlackPixelRate(g_pCandyImgErode);		//获取黑点的概率

		//TRACE("BlackPixelRate = %f",pOpencvMotionDetection->g_BlackPixelRate);

		if(pOpencvMotionDetection->g_BlackPixelRate>BLACK_PIXEL_RATE){		//黑点数超过百分之一判定为motion detect
			pOpencvMotionDetection->g_SaveImageCounter++;
			pOpencvMotionDetection->SaveImage(g_pCannySrcImg);
		}

		pOpencvMotionDetection->g_HandlingFlag = 0;
	}
	return 0;
}
/*
void on_trackbar(int threshold)  
{  
   g_ThreadCandy = threshold;
} 
*/
void OpencvMotionDetection::Start(void)
{
	this->g_StartFlag = 1;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;
	this->g_HandlingFlag = 0;
	cvNamedWindow("MotionDetect窗口",CV_WINDOW_AUTOSIZE);

	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	
	AfxBeginThread((AFX_THREADPROC)CandyThread,this,THREAD_PRIORITY_HIGHEST);
	//int nThresholdEdge = 1;  
    //cvCreateTrackbar("Candy阈值",CFG_OPENCV_RTSP_WIN, &nThresholdEdge, 100, on_trackbar);	//增加边缘检测阈值到窗体内，回调函数是on_trackbar
	//on_trackbar(1); 
}

void OpencvMotionDetection::ReleaseIplImage(IplImage* &IplIma)
{
	if(IplIma!=NULL){
		cvReleaseImage(&IplIma);
		IplIma = NULL;
	}
}

void OpencvMotionDetection::Stop(void)
{
	while(this->g_HandlingFlag)	//等待处理完成
		Sleep(500);

	this->g_StartFlag = 0;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;


	cvDestroyWindow("MotionDetect窗口");

	ReleaseIplImage(g_pSrcImage);	
	ReleaseIplImage(g_pCannySrcImg);
	ReleaseIplImage(g_pCannyImg1st);
	ReleaseIplImage(g_pCannyImg2nd);
	ReleaseIplImage(g_pCannyImg3rd);
	ReleaseIplImage(g_pCandyImgErode);

}
#define MOTION_DETECT_SCALE 2
void OpencvMotionDetection::Handle(IplImage *ParamImage)
{
	if(this->g_StartFlag == 0)
		return;

	if(this->g_HandlingFlag)	//正在处理中，丢掉该帧
		return ;

	unsigned int width = ParamImage->width;
	unsigned height = ParamImage->height;
	this->g_FrameCounter++;

	if(g_FrameCounter == 1){		//第一帧数据来创建图像
		g_pSrcImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
		g_pCannySrcImg = cvCreateImage(cvSize(width/MOTION_DETECT_SCALE,height/MOTION_DETECT_SCALE), IPL_DEPTH_8U, 3);  
		g_pCannyImg1st = cvCreateImage(cvSize(width/MOTION_DETECT_SCALE,height/MOTION_DETECT_SCALE), IPL_DEPTH_8U, 1); 
		g_pCannyImg2nd = cvCreateImage(cvSize(width/MOTION_DETECT_SCALE,height/MOTION_DETECT_SCALE), IPL_DEPTH_8U, 1); 
		g_pCannyImg3rd = cvCreateImage(cvSize(width/MOTION_DETECT_SCALE,height/MOTION_DETECT_SCALE), IPL_DEPTH_8U, 1); 
		g_pCandyImgErode = cvCreateImage(cvSize(width/MOTION_DETECT_SCALE,height/MOTION_DETECT_SCALE), IPL_DEPTH_8U, 1);
		
		memcpy(g_pSrcImage->imageData,ParamImage->imageData,width*height*3);
		cvResize(g_pSrcImage,g_pCannySrcImg);
		cvCanny(g_pCannySrcImg, g_pCannyImg1st, g_ThreadCandy, g_ThreadCandy * 3, 3);  

	}else{

		if(g_pSrcImage == NULL)
			return ;

		memcpy(g_pSrcImage->imageData,ParamImage->imageData,width*height*3);
		cvResize(g_pSrcImage,g_pCannySrcImg);
	#if 1
		if(this->g_FrameCounter % 10 == 0){		//每5帧，更新一次Candy检测
			ReleaseSemaphore(hSemaphore, 1, NULL);
		}
	#endif
		if(cvWaitKey(10)=='A'){

		}
	}
}

