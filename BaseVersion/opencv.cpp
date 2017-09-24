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

void opencv::opencv_init(void)
{
	this->Printf(CString("Opencv Init\r\n"));

	using namespace cv; //���������cv������Ͳ��ü���ǰ׺�� 

    Mat img = imread("../Input/opencvtest.jpg"); //����Mat����������lenaС椣���������ڣ�������Ƭ   
   
    if(!img.data) //�ж�ͼƬ�����Ƿ�ɹ�   
        return ; //����ͼƬʧ�����˳�   
   
    namedWindow("loveLena", CV_WINDOW_AUTOSIZE); //��������,��ȷ����Ϊ��С���ɱ����ʹ���   
   
    imshow("loveLena", img); //��ʾͼƬ������㲻���ⴰ�ڴ�С�ɱ䣬����ֱ��ע�͵���һ�䡣��Ϊimshow����ֱ�Ӵ�������   
   
    return ; 
}
