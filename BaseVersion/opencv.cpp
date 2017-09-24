#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"
#include "BaseVersionDlg.h"
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

static int g_ThreadCandy = 70;
IplImage *g_pSrcImage;
IplImage *g_pCannySrcImg;
IplImage *g_pCannyImg;

opencv::opencv()
{

}
opencv::~opencv()
{

}
void opencv::Printf(CString Context)
{
	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
	CString m_strTime = time.Format("[%H:%M:%S]");

	Context = m_strTime + (CString)("[opencv]") + Context;
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->Printf(Context);
}

DWORD WINAPI CandyThread(LPVOID pParam)
{
	cvCanny(g_pCannySrcImg, g_pCannyImg, g_ThreadCandy, g_ThreadCandy * 3, 3);  
	cvShowImage("边缘检测窗口", g_pCannyImg);
	return 0;
}

void opencv::opencv_showRGB(int width ,int height, unsigned char *rgbdata)
{
	g_FrameCounter++;

	if(g_FrameCounter == 1){		//第一帧数据来创建图像
		g_pSrcImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
		g_pCannySrcImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 3);  
		g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);  
	}

	memcpy(g_pSrcImage->imageData,rgbdata,width*height*3);

	cvShowImage(CFG_OPENCV_RTSP_WIN,g_pSrcImage);
#if 1
	if(g_FrameCounter % 60 == 0){		//每60帧，更新一次Candy检测
		memcpy(g_pCannySrcImg->imageData,rgbdata,width*height*3);
		AfxBeginThread((AFX_THREADPROC)CandyThread,this,THREAD_PRIORITY_NORMAL);
	}
#endif
	if(cvWaitKey(10)=='A'){
		TRACE("Test");
	}
	//cvReleaseImage(&g_pSrcImage);
}

void on_trackbar(int threshold)  
{  
   g_ThreadCandy = threshold;
} 

void opencv::opencv_init(void)
{
	this->Printf(CString("Opencv Init\r\n"));

	IplImage *pImage = cvLoadImage("../Input/opencvtest.jpg");
	cvDestroyWindow(CFG_OPENCV_RTSP_WIN);
	cvNamedWindow(CFG_OPENCV_RTSP_WIN,CV_WINDOW_AUTOSIZE);
	cvShowImage(CFG_OPENCV_RTSP_WIN,pImage);

	cvDestroyWindow("边缘检测窗口");
	cvNamedWindow("边缘检测窗口",CV_WINDOW_AUTOSIZE);
	cvShowImage("边缘检测窗口",pImage);

	g_ThreadCandy = 75;
	//int nThresholdEdge = 1;  
    //cvCreateTrackbar("Candy阈值",CFG_OPENCV_RTSP_WIN, &nThresholdEdge, 100, on_trackbar);	//增加边缘检测阈值到窗体内，回调函数是on_trackbar
	//on_trackbar(1); 
	cvReleaseImage(&pImage);

	this->g_FrameCounter = 0;

    return ; 
}

void opencv::opencv_stop(void)
{
	cvDestroyWindow(CFG_OPENCV_RTSP_WIN);
	cvDestroyWindow("边缘检测窗口");
}