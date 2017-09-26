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
OpencvMotionDetection cvMotionDetect

#elif DEFINE_GAUSS_MODE
#include "OpencvGaussMode.h"
OpencvGaussMode cvGaussMode;

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


void opencv::HandleImage(int width ,int height, unsigned char *rgbdata)
{
	if(this->g_StartFlag == 0)
		return ;

	this->g_FrameCounter++;

	if(this->g_FrameCounter == 1){
		SrcImage =  cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
	}else{
		memcpy(SrcImage->imageData,rgbdata,width*height*3);

		cvShowImage("Opencv源窗口",SrcImage);

#if DEFINE_MOTION_DETECTION
		cvMotionDetect.Handle(SrcImage);
		this->UpdateJPGInfo(cvMotionDetect.g_SaveImageCounter);
		this->UpdateBlackPixelRate(cvMotionDetect.g_BlackPixelRate);
#elif DEFINE_GAUSS_MODE
		cvGaussMode.Handle(SrcImage);
		this->UpdateBlackPixelRate(cvGaussMode.g_BlackPixelRate);
#endif

	}

	//cvReleaseImage(&g_pSrcImage);
}


void opencv::Start(void)
{
	this->g_FrameCounter = 0;

	this->g_StartFlag = 1;

	cvNamedWindow("Opencv源窗口",CV_WINDOW_AUTOSIZE);

#if DEFINE_MOTION_DETECTION
	cvMotionDetect.Start();
#elif DEFINE_GAUSS_MODE
	cvGaussMode.Start();
#endif
    return ;
}

void opencv::Stop(void){

#if DEFINE_MOTION_DETECTION
	cvMotionDetect.Stop();
#elif DEFINE_GAUSS_MODE
	cvGaussMode.Stop();
#endif

	cvDestroyWindow("Opencv源窗口");
	this->g_StartFlag = 0;

	if(SrcImage!=NULL){
		cvReleaseImage(&SrcImage);
		SrcImage = NULL;
	}
	return;
}