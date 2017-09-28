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
#include "OpencvAbsDetection.h"
#include "BinaryAreaCombine.h"

BinaryAreaCombine AbsAreaCombine;

OpencvAbsDetection::OpencvAbsDetection()
{

}
OpencvAbsDetection::~OpencvAbsDetection()
{

}


void OpencvAbsDetection::SaveImage(IplImage *Image)
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


DWORD WINAPI AbsImageThread(LPVOID pParam)
{
	OpencvAbsDetection *pOpencvAbsDetection = (OpencvAbsDetection*)(pParam);

	while(pOpencvAbsDetection->g_StartFlag){
		WaitForSingleObject(pOpencvAbsDetection->hSemaphore, INFINITE);

		pOpencvAbsDetection->g_HandlingFlag = 1;

		 cvCvtColor(pOpencvAbsDetection->g_pSrcScaleImage,pOpencvAbsDetection->g_pCurGrayImage,CV_BGR2GRAY);

		cvAbsDiff(pOpencvAbsDetection->g_pCurGrayImage,pOpencvAbsDetection->g_pLastGrayImage,pOpencvAbsDetection->g_pAbsGrayImage);			//与上一帧做图像绝对值减法g_pAbsGrayImage = abs(g_pLastGrayImage - g_pCurGrayImage）
		memcpy(pOpencvAbsDetection->g_pLastGrayImage->imageData,pOpencvAbsDetection->g_pCurGrayImage->imageData,pOpencvAbsDetection->g_pCurGrayImage->width*pOpencvAbsDetection->g_pCurGrayImage->height);//更新当前帧g_pLastGrayImage = g_pCurGrayImage

		 cvThreshold(pOpencvAbsDetection->g_pAbsGrayImage, pOpencvAbsDetection->g_pAbsBinImage, 25, 255 , CV_THRESH_BINARY);
		//cvErode( g_pCannyImg3rd,g_pCandyImgErode, NULL,1); //腐蚀  
		//cvSmooth(g_pCannyImg3rd,g_pCandyImgErode,CV_BLUR,3,1); //平均滤波
		//cvSmooth(g_pCannyImg3rd,g_pCandyImgErode,CV_MEDIAN,3,1);  //  中值滤波
#if 1
		#define SortNum 10
		unsigned int SortMaxtoMin[SortNum];
		unsigned int RealAreaNum;
		AbsAreaCombine.AreaCombine(pOpencvAbsDetection->g_pAbsBinImage);
		RealAreaNum = AbsAreaCombine.AreaMaxToMinSort(SortMaxtoMin,SortNum);
		IMAGEAREA AreaInfo;
		for(int i=0;i<RealAreaNum;i++){
			AbsAreaCombine.GetAreaInfo(&AreaInfo,SortMaxtoMin[i]);
			cvRectangle(pOpencvAbsDetection->g_pSrcScaleImage,  
			cvPoint(AreaInfo.StartX,AreaInfo.StartY),  
			cvPoint(AreaInfo.EndX,AreaInfo.EndY),  
			CV_RGB(255,0,0));    
		}
#else
		IMAGEAREA AreaInfo;
		AbsAreaCombine.AreaCombine(pOpencvAbsDetection->g_pAbsBinImage);
		AbsAreaCombine.CombineAllArea(&AreaInfo);
		cvRectangle(pOpencvAbsDetection->g_pSrcScaleImage,  
		cvPoint(AreaInfo.StartX,AreaInfo.StartY),  
		cvPoint(AreaInfo.EndX,AreaInfo.EndY),  
		CV_RGB(255,0,0));  
#endif
		cvShowImage("ABS差值窗口", pOpencvAbsDetection->g_pAbsBinImage);
		cvShowImage("ABS原图", pOpencvAbsDetection->g_pSrcScaleImage);

		pOpencvAbsDetection->g_BlackPixelRate = pOpencvAbsDetection->BinBlackPixelRate(pOpencvAbsDetection->g_pAbsBinImage);		//获取黑点的概率

		//TRACE("BlackPixelRate = %f",pOpencvMotionDetection->g_BlackPixelRate);

		if(pOpencvAbsDetection->g_BlackPixelRate>BLACK_PIXEL_RATE){		//黑点数超过百分之一判定为motion detect
			pOpencvAbsDetection->g_SaveImageCounter++;
			pOpencvAbsDetection->SaveImage(pOpencvAbsDetection->g_pSrcScaleImage);
		}

		pOpencvAbsDetection->g_HandlingFlag = 0;
	}
	return 0;
}
/*
void on_trackbar(int threshold)  
{  
   g_ThreadCandy = threshold;
} 
*/
void OpencvAbsDetection::Start(void)
{
	AbsAreaCombine.Start();
	this->g_StartFlag = 1;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;
	this->g_HandlingFlag = 0;
	cvNamedWindow("ABS差值窗口",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("ABS原图",CV_WINDOW_AUTOSIZE);
	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
	g_pSrcScaleImage = NULL;
	g_pLastGrayImage = NULL;
	g_pCurGrayImage = NULL;
	g_pAbsGrayImage = NULL;
	g_pAbsBinImage = NULL;

	AfxBeginThread((AFX_THREADPROC)AbsImageThread,this,THREAD_PRIORITY_HIGHEST);
	//int nThresholdEdge = 1;  
    //cvCreateTrackbar("Candy阈值",CFG_OPENCV_RTSP_WIN, &nThresholdEdge, 100, on_trackbar);	//增加边缘检测阈值到窗体内，回调函数是on_trackbar
	//on_trackbar(1); 
}

void OpencvAbsDetection::ReleaseIplImage(IplImage* &IplIma)
{
	if(IplIma!=NULL){
		cvReleaseImage(&IplIma);
		IplIma = NULL;
	}
}

void OpencvAbsDetection::Stop(void)
{
	while(this->g_HandlingFlag)	//等待处理完成
		Sleep(500);

	AbsAreaCombine.Stop();
	this->g_StartFlag = 0;
	this->g_FrameCounter = 0;
	this->g_SaveImageCounter = 0;
	this->g_BlackPixelRate = 0.0;

	cvDestroyWindow("ABS差值窗口");
	cvDestroyWindow("ABS原图");

	ReleaseIplImage(g_pSrcScaleImage);	
	ReleaseIplImage(g_pCurGrayImage);
	ReleaseIplImage(g_pLastGrayImage);
	ReleaseIplImage(g_pAbsGrayImage);
	ReleaseIplImage(g_pAbsBinImage);

}

void OpencvAbsDetection::Handle(IplImage *ParamImage)
{
	if(this->g_StartFlag == 0)
		return;

	if(this->g_HandlingFlag)	//正在处理中，丢掉该帧
		return ;

	unsigned int width = ParamImage->width;
	unsigned height = ParamImage->height;
	this->g_FrameCounter++;

	if(g_FrameCounter == 1){		//第一帧数据来创建图像

		g_pSrcScaleImage = cvCreateImage(cvSize(width/ABS_DETECT_SCALE,height/ABS_DETECT_SCALE), IPL_DEPTH_8U, 3);  
		g_pCurGrayImage = cvCreateImage(cvSize(width/ABS_DETECT_SCALE,height/ABS_DETECT_SCALE), IPL_DEPTH_8U, 1); 
		g_pLastGrayImage = cvCreateImage(cvSize(width/ABS_DETECT_SCALE,height/ABS_DETECT_SCALE), IPL_DEPTH_8U, 1); 
		g_pAbsGrayImage = cvCreateImage(cvSize(width/ABS_DETECT_SCALE,height/ABS_DETECT_SCALE), IPL_DEPTH_8U, 1); 
		g_pAbsBinImage = cvCreateImage(cvSize(width/ABS_DETECT_SCALE,height/ABS_DETECT_SCALE), IPL_DEPTH_8U, 1); 

		cvResize(ParamImage,g_pSrcScaleImage);
	}else{

		if(g_pSrcScaleImage == NULL)
			return ;

		cvResize(ParamImage,g_pSrcScaleImage);

	#if 1
		if(this->g_FrameCounter % 1 == 0){		//每5帧，更新一次检测
			ReleaseSemaphore(hSemaphore, 1, NULL);
		}
	#endif
		if(cvWaitKey(10)=='A'){

		}
	}
}

