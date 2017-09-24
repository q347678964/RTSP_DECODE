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
	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
	CString m_strTime = time.Format("[%H:%M:%S]");

	Context = m_strTime + (CString)("[opencv]") + Context;
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->Printf(Context);
}

void opencv::opencv_init(void)
{
	this->Printf(CString("Opencv Init\r\n"));

	using namespace cv; //下面的所有cv相关类型不用加上前缀了 

    Mat img = imread("../Input/opencvtest.jpg"); //声明Mat变量并调入lena小妞（老妞了现在？）的照片   
   
    if(!img.data) //判断图片调入是否成功   
        return ; //调入图片失败则退出   
   
    namedWindow("loveLena", CV_WINDOW_AUTOSIZE); //创建窗口,并确定其为大小不可变类型窗口   
   
    imshow("loveLena", img); //显示图片。如果你不介意窗口大小可变，可以直接注释掉上一句。因为imshow可以直接创建窗口   
   
    return ; 
}
