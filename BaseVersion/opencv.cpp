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

#if DEFINE_MOTION_DETECTION
#include "OpencvMotionDetection.h"
OpencvMotionDetection cvMotionDetect;

#elif DEFINE_GAUSS_MODE
#include "OpencvGaussMode.h"
OpencvGaussMode cvGaussMode;

#elif DEFINE_ABS_DETECTION
#include "OpencvAbsDetection.h"
OpencvAbsDetection cvAbsDetect;
#endif


IplImage *SrcImage;

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

void opencv::UpdateJPGInfo(unsigned int Num)
{
	CString TempCString;
	TempCString.Format(_T("%u"),Num);
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->UIOperationCB(IDC_EDIT_JPGShow,TempCString);
}

void opencv::UpdateBlackPixelRate(float Num)
{
	CString TempCString;
	TempCString.Format(_T("%.04f%%"),Num);
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->UIOperationCB(IDC_EDIT_BlackPixelShow,TempCString);
}

DWORD WINAPI ImageHandleThread(LPVOID pParam)
{
	opencv* popencv = (opencv*)(pParam);
	while(popencv->g_StartFlag){
		WaitForSingleObject(popencv->hSemaphore, INFINITE);

		if(popencv->g_StartFlag == 0){
			return 0;
		}

		popencv->g_HandlingFlag = 1;
#if OPENCV_SRC_WINDOW_SHOW
		cvShowImage("Opencv源窗口",SrcImage);
#endif
	
#if DEFINE_MOTION_DETECTION
		cvMotionDetect.Handle(SrcImage);
		popencv->UpdateJPGInfo(cvMotionDetect.g_SaveImageCounter);
		popencv->UpdateBlackPixelRate(cvMotionDetect.g_BlackPixelRate);
#elif DEFINE_GAUSS_MODE
		cvGaussMode.Handle(SrcImage);
		popencv->UpdateBlackPixelRate(cvGaussMode.g_BlackPixelRate);
#elif DEFINE_ABS_DETECTION
		cvAbsDetect.Handle(SrcImage);
		popencv->UpdateJPGInfo(cvAbsDetect.g_SaveImageCounter);
		popencv->UpdateBlackPixelRate(cvAbsDetect.g_BlackPixelRate);
#endif
		popencv->g_HandlingFlag = 0;
	}
	return 0;
}
void opencv::HandleImage(int width ,int height, unsigned char *rgbdata)
{

	if(this->g_StartFlag == 0){
		return ;
	}
	if(this->g_HandlingFlag)		//正在处理上一帧，丢弃帧
		return ;

	this->g_FrameCounter++;

	if(this->g_FrameCounter == 1){
		SrcImage =  cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
	}else{
		memcpy(SrcImage->imageData,rgbdata,width*height*3);
		ReleaseSemaphore(hSemaphore, 1, NULL);
	}
}


//开始需要先开始底层线程，防止底层操作空的底层
void opencv::Start(void)
{
#if OPENCV_SRC_WINDOW_SHOW
	cvNamedWindow("Opencv源窗口",CV_WINDOW_AUTOSIZE);
#endif

#if DEFINE_MOTION_DETECTION
	cvMotionDetect.Start();
#elif DEFINE_GAUSS_MODE
	cvGaussMode.Start();
#elif DEFINE_ABS_DETECTION
	cvAbsDetect.Start();
#endif

	this->g_FrameCounter = 0;

	this->g_StartFlag = 1;

	this->g_HandlingFlag = 0;

	hSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

	AfxBeginThread((AFX_THREADPROC)ImageHandleThread,this,THREAD_PRIORITY_HIGHEST);

    return ;
}

//停止先要顶层先停下来，防止顶层线程继续操作底层函数
void opencv::Stop(void){
	while(this->g_HandlingFlag)		//等待底层处理完成
		Sleep(500) ;

#if DEFINE_MOTION_DETECTION
	cvMotionDetect.Stop();
#elif DEFINE_GAUSS_MODE
	cvGaussMode.Stop();
#elif DEFINE_ABS_DETECTION
	cvAbsDetect.Stop();
#endif

#if OPENCV_SRC_WINDOW_SHOW
	cvDestroyWindow("Opencv源窗口");
#endif

	this->g_StartFlag = 0;

	if(SrcImage!=NULL){
		cvReleaseImage(&SrcImage);
		SrcImage = NULL;
	}

	return;
}