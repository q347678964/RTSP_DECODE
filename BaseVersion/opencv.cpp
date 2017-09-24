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

void opencv::opencv_showRGB(int width ,int height, unsigned char *rgbdata)
{
	IplImage *pImage = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U ,3);

	memcpy(pImage->imageData,rgbdata,width*height*3);

	cvShowImage(CFG_OPENCV_RTSP_WIN,pImage);

	if(cvWaitKey(10)=='A'){
		TRACE("Test");
	}

	cvReleaseImage(&pImage);
}

void opencv::opencv_init(void)
{
	this->Printf(CString("Opencv Init\r\n"));
#if 0
	using namespace cv; //���������cv������Ͳ��ü���ǰ׺��
    Mat img = imread("../Input/opencvtest.jpg"); //����Mat����������lenaС椣���������ڣ�������Ƭ   
    if(!img.data) //�ж�ͼƬ�����Ƿ�ɹ�   
        return ; //����ͼƬʧ�����˳�    
    namedWindow("loveLena", CV_WINDOW_AUTOSIZE); //��������,��ȷ����Ϊ��С���ɱ����ʹ���   
    imshow("loveLena", img); //��ʾͼƬ������㲻���ⴰ�ڴ�С�ɱ䣬����ֱ��ע�͵���һ�䡣��Ϊimshow����ֱ�Ӵ�������   
#else 
	IplImage *pImage = cvLoadImage("../Input/opencvtest.jpg");
	cvNamedWindow(CFG_OPENCV_RTSP_WIN,CV_WINDOW_AUTOSIZE);
	cvShowImage(CFG_OPENCV_RTSP_WIN,pImage);
	cvReleaseImage(&pImage);
#endif
    return ; 
}

void opencv::opencv_stop(void)
{
	cvDestroyWindow(CFG_OPENCV_RTSP_WIN);
}