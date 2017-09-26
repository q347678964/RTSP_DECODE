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
#include "OpencvMotionDetection.h"


OpencvMotionDetection cvMotionDetect;

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

		cvMotionDetect.Handle(SrcImage);

		this->UpdateJPGInfo(cvMotionDetect.g_SaveImageCounter);
		this->UpdateBlackPixelRate(cvMotionDetect.g_BlackPixelRate);
	}

	//cvReleaseImage(&g_pSrcImage);
}


void opencv::Start(void)
{
	this->g_FrameCounter = 0;

	this->g_StartFlag = 1;

	cvMotionDetect.Start();

    return ;
}

void opencv::Stop(void){

	cvMotionDetect.Stop();

	this->g_StartFlag = 0;

	if(SrcImage!=NULL){
		cvReleaseImage(&SrcImage);
		SrcImage = NULL;
	}
	return;
}