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

CRITICAL_SECTION g_cs;              // �ٽ����ṹ����
static int g_ThreadCandy = 70;
IplImage *g_pSrcImage = NULL;
IplImage *g_pCannySrcImg = NULL;
IplImage *g_pCannyImg1st = NULL;
IplImage *g_pCannyImg2nd = NULL;
IplImage *g_pCannyImg3rd = NULL;
IplImage *g_pCandyImgErode = NULL;
opencv::opencv()
{

}
opencv::~opencv()
{

}
void opencv::Printf(CString Context)
{
	CTime time = CTime::GetCurrentTime();   ///����CTime����
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

void opencv::UpdateBlackPixelInfo(unsigned int Num)
{
	CString TempCString;
	TempCString.Format(_T("%u"),Num);
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->UIOperationCB(IDC_EDIT_BlackPixelShow,TempCString);
}
DWORD WINAPI CandyThread(LPVOID pParam)
{
	EnterCriticalSection(&g_cs);		// �����ٽ���

	opencv *pOpencv = (opencv*)(pParam);

	if(pOpencv->g_startflag == 0)
		return 0;

	cvCanny(g_pCannySrcImg, g_pCannyImg1st, g_ThreadCandy, g_ThreadCandy * 3, 3);  
	cvAbsDiff(g_pCannyImg1st,g_pCannyImg2nd,g_pCannyImg3rd);
	memcpy(g_pCannyImg2nd->imageData,g_pCannyImg1st->imageData,g_pCannyImg1st->width*g_pCannyImg1st->height);

	//cvErode( g_pCannyImg3rd,g_pCandyImgErode, NULL,1); //��ʴ  
	//cvSmooth(g_pCannyImg3rd,g_pCandyImgErode,CV_BLUR,3,1); //ƽ���˲�
	cvSmooth(g_pCannyImg3rd,g_pCandyImgErode,CV_MEDIAN,3,1);  //  ��ֵ�˲�

	cvShowImage(CFG_OPENCV_OUTPUT_WIN, g_pCandyImgErode);

	float BlackPixelRate = pOpencv->OpencvSelf.BinBlackPixelRate(g_pCandyImgErode);
	unsigned int BlackPixelCounter = pOpencv->OpencvSelf.BinBlackPixelCounter(g_pCandyImgErode);

	TRACE("BlackPixelRate = %f",BlackPixelRate);

//	if(BlackPixelCounter>CFG_OPENCV_MOTION_DETECT){
	if(BlackPixelRate>1.0){		//�ڵ��������ٷ�֮һ�ж�Ϊmotion detect
		pOpencv->g_SaveImageCounter++;
		CString TempCString;
		CString DirCString;
		CString TimeCString = pOpencv->GetTimeCString();

		DirCString.Format(_T("../Output/%s"),TimeCString);
		if (!PathIsDirectory(DirCString))  
		{  
			::CreateDirectory(DirCString, NULL);//����Ŀ¼,���еĻ���Ӱ��  
		}  

		TempCString.Format(_T("../Output/%s/%u.jpg"),TimeCString,pOpencv->g_SaveImageCounter);
		char *SavePath = pOpencv->CStringToChar(TempCString);
		cvSaveImage(SavePath,g_pCannySrcImg,0);

		pOpencv->UpdateJPGInfo(pOpencv->g_SaveImageCounter);		//����UI
	}

	//pOpencv->UpdateBlackPixelInfo(BlackPixelCounter);//����UI
	pOpencv->UpdateBlackPixelInfo((unsigned int)(BlackPixelRate*1000));//����UI
	LeaveCriticalSection(&g_cs);		// �뿪�ٽ���
	return 0;
}

void opencv::opencv_showRGB(int width ,int height, unsigned char *rgbdata)
{
	EnterCriticalSection(&g_cs);		// �����ٽ���

	if(this->g_startflag == 0)
		return;

	g_FrameCounter++;

	if(g_FrameCounter == 1){		//��һ֡����������ͼ��
		g_pSrcImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);
		g_pCannySrcImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 3);  
		g_pCannyImg1st = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1); 
		g_pCannyImg2nd = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1); 
		g_pCannyImg3rd = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1); 
		g_pCandyImgErode = cvCreateImage(cvGetSize(g_pCannyImg1st), IPL_DEPTH_8U, 1);

		cvCanny(g_pCannySrcImg, g_pCannyImg1st, g_ThreadCandy, g_ThreadCandy * 3, 3);  
		memcpy(g_pCannyImg2nd->imageData,g_pCannyImg1st->imageData,width*height);
		 
	}

	if(g_pSrcImage == NULL)
		return ;

	memcpy(g_pSrcImage->imageData,rgbdata,width*height*3);

	cvShowImage(CFG_OPENCV_RTSP_WIN,g_pSrcImage);
#if 1
	if(g_FrameCounter % 10 == 0){		//ÿ60֡������һ��Candy���
		memcpy(g_pCannySrcImg->imageData,rgbdata,width*height*3);
		AfxBeginThread((AFX_THREADPROC)CandyThread,this,THREAD_PRIORITY_NORMAL);
	}
#endif
	if(cvWaitKey(10)=='A'){
		TRACE("Test");
	}

	LeaveCriticalSection(&g_cs);		// �뿪�ٽ���
	//cvReleaseImage(&g_pSrcImage);
}

void on_trackbar(int threshold)  
{  
   g_ThreadCandy = threshold;
} 

void opencv::opencv_init(void)
{
	InitializeCriticalSection(&g_cs);		// ��ʼ���ٽ���

	this->Printf(CString("��ʼ��\r\n"));

	unsigned int Temp = CFG_OPENCV_MOTION_DETECT;
	CString TempCString;
	TempCString.Format(_T("���ú�ɫ������ֵ:%u\r\n"),Temp);
	this->Printf(TempCString);
#if 0
	IplImage *pImage = cvLoadImage("../Input/opencvtest.jpg");

	cvNamedWindow(CFG_OPENCV_RTSP_WIN,CV_WINDOW_AUTOSIZE);
	cvShowImage(CFG_OPENCV_RTSP_WIN,pImage);

	cvNamedWindow(CFG_OPENCV_OUTPUT_WIN,CV_WINDOW_AUTOSIZE);
	cvShowImage(CFG_OPENCV_OUTPUT_WIN,pImage);

	g_ThreadCandy = 75;
	//int nThresholdEdge = 1;  
    //cvCreateTrackbar("Candy��ֵ",CFG_OPENCV_RTSP_WIN, &nThresholdEdge, 100, on_trackbar);	//���ӱ�Ե�����ֵ�������ڣ��ص�������on_trackbar
	//on_trackbar(1); 
	cvReleaseImage(&pImage);
#else
	cvNamedWindow(CFG_OPENCV_RTSP_WIN,CV_WINDOW_AUTOSIZE);
	cvNamedWindow(CFG_OPENCV_OUTPUT_WIN,CV_WINDOW_AUTOSIZE);
#endif
	this->g_FrameCounter = 0;

	this->g_SaveImageCounter = 0;

	this->g_startflag = 1;
    return ; 

}

void opencv::ReleaseIplImage(IplImage* &IplIma)
{
	if(IplIma!=NULL){
		cvReleaseImage(&IplIma);
		IplIma = NULL;
	}
}
void opencv::opencv_stop(void)
{
	EnterCriticalSection(&g_cs);		// �����ٽ���
	this->g_startflag = 0;
	cvDestroyWindow(CFG_OPENCV_RTSP_WIN);
	cvDestroyWindow(CFG_OPENCV_OUTPUT_WIN);

	ReleaseIplImage(g_pSrcImage);	
	ReleaseIplImage(g_pCannySrcImg);
	ReleaseIplImage(g_pCannyImg1st);
	ReleaseIplImage(g_pCannyImg2nd);
	ReleaseIplImage(g_pCannyImg3rd);
	ReleaseIplImage(g_pCandyImgErode);

	LeaveCriticalSection(&g_cs);		// �뿪�ٽ���
}