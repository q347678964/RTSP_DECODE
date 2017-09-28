#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "BinaryAreaCombine.h"

IplImage* g_InputImage;
IplImage* g_LineImage;
IplImage* g_AreaImage;
void BinaryAreaCombine::Start(void){
	g_LineDesc = NULL;
	g_WhiteArea = NULL;
	g_InputImage = NULL;
	g_WhiteAreaTotal = 0;
	g_FrameCounter = 0;
	g_StartFlag = 1;

	InitializeCriticalSection(&g_CS);
#if BINARYAREA_DEBUG
	cvNamedWindow("区域连通输入图", CV_WINDOW_AUTOSIZE );  
    cvNamedWindow("区域连通短线标记", CV_WINDOW_AUTOSIZE );  
    cvNamedWindow("区域连通标记", CV_WINDOW_AUTOSIZE );  
#endif
}

void BinaryAreaCombine::Stop(void){
	EnterCriticalSection(&g_CS);		// 进入临界区
	g_StartFlag = 0;
	if(g_InputImage != NULL){
		g_InputImage =NULL;
		cvReleaseImage(&g_InputImage);
	}
	if(g_LineImage != NULL){
		g_LineImage =NULL;
		cvReleaseImage(&g_LineImage);
	}
	if(g_AreaImage != NULL){
		g_AreaImage =NULL;
		cvReleaseImage(&g_AreaImage);
	}

	if(g_LineDesc != NULL){
		free(g_LineDesc);
	}
	if(g_WhiteArea != NULL){
		free(g_WhiteArea);
	}
#if BINARYAREA_DEBUG
	cvDestroyWindow("区域连通输入图");
	cvDestroyWindow("区域连通短线标记");
	cvDestroyWindow("区域连通标记");
#endif
	LeaveCriticalSection(&g_CS);		// 离开临界区
}

/*
*BinaryImage: 二进制图像
*x:横坐标
*y:纵坐标
*Ret:Pixel值
*/
//获取二进制图像某个点的像素值0~255
int BinaryAreaCombine::GetPixel(IplImage* BinaryImage,int x ,int y)
{
	unsigned char *ImageData = (unsigned char*)BinaryImage->imageData;
	int ImageWidth = BinaryImage->width;
	int ImageHeight = BinaryImage->height;
	int step = BinaryImage->widthStep/sizeof(uchar); //图像数据有4字节对齐操作，所以需要用step，不能直接用width

	if(x<0||x>=ImageWidth||y<0||y>=ImageHeight)
		return -1;
	else
		return ImageData[y*step+x];
		//return cvGet2D(BinaryImage,y,x).val[0];
		//return *(ImageData+y*step+x);
}
/*
*获取图像某一行中白线的数量和描述,忽略一个点的情况。
*BinaryImage 二进制图像
*LineNumber 行号
*Line 白线描述
*Ret 白线数
*/
unsigned int BinaryAreaCombine::GetWhiteLine(IplImage* BinaryImage,unsigned int LineNumber,SHORTLINE *Line)
{
	int ImageWidth = BinaryImage->width;
	int ImageHeight = BinaryImage->height;
	int i = 0;
	unsigned int LineCounter = 0;
	unsigned char CurPixel = 0;

	memset(Line,0,sizeof(SHORTLINE));

	for(i=0;i<ImageWidth;i++){
			CurPixel = BinaryAreaCombine::GetPixel(BinaryImage,i,LineNumber);
			if(CurPixel == WHITE_PIXEL){	//找到白色起点'
				Line[LineCounter].StartX = i;
				if(i != (ImageWidth-1)){	//判断不是当前行的最后一个点
					while(1){			//循环直到结束或者下一个黑点
						i++;
						CurPixel = BinaryAreaCombine::GetPixel(BinaryImage,i,LineNumber);
						if(CurPixel == BLACK_PIXEL ){		//找到结束黑点或者结束
							Line[LineCounter].EndX = i-1;
							break;
						}else if(i == (ImageWidth-1)){
							Line[LineCounter].EndX = i;
							break;
						}
					}
				}else{
					Line[LineCounter].EndX = i;
				}
			}//一条白线结束

			if(Line[LineCounter].StartX != Line[LineCounter].EndX){	//排除单个白点的情况
				Line[LineCounter].Active = 1;
				LineCounter++;
			}
	}

	return LineCounter;
}

#define MIN(x1,x2) ((x1)>=(x2)?(x2):(x1))
#define MAX(x1,x2) ((x1)>=(x2)?(x1):(x2))

/*
给某个区域增加一个短白线，组成新的区域
Area:区域
Line:短白线
LineY:短白线所在的Y值
*/
void BinaryAreaCombine::AreaAddLine(IMAGEAREA* Area,SHORTLINE *Line,int LineY)
{
	int XMin,XMax,YMin,YMax;

	XMin = MIN(Area->StartX,Line->StartX);
	XMax = MAX(Area->EndX,Line->EndX);
	YMin = MIN(Area->StartY,LineY);
	YMax = MAX(Area->EndY,LineY);

	Area->StartX = XMin;
	Area->EndX = XMax;
	Area->StartY = YMin;
	Area->EndY = YMax;
	Area->LineCounter++;

	Line->AreaID = Area->AreaID;
}

/*
给某个区域增加一个区域，组成新的区域
Area:区域
Line:短白线
LineY:短白线所在的Y值
*/
void BinaryAreaCombine::AreaAddArea(IMAGEAREA* Area1,IMAGEAREA *Area2)
{
	int XMin,XMax,YMin,YMax;

	XMin = MIN(Area1->StartX,Area2->StartX);
	XMax = MAX(Area1->EndX,Area2->EndX);
	YMin = MIN(Area1->StartY,Area2->StartY);
	YMax = MAX(Area1->EndY,Area2->EndY);

	Area1->StartX = XMin;
	Area1->EndX = XMax;
	Area1->StartY = YMin;
	Area1->EndY = YMax;
	Area1->LineCounter = Area1->LineCounter + Area2->LineCounter;
	Area2->Active = 0;
}

/*两个彼此分离的目标， 但是画框的时候还是有可能有重复部分，可选的调用该函数进行合并*/
void BinaryAreaCombine::AreaCombineSelect(void)
{
	unsigned int UsefulArea = 0;
	for(int i=0;i<g_WhiteAreaTotal;i++){
		if(g_WhiteArea[i].Active == 1){
			UsefulArea++;
		}
	}
	//TRACE(_T("g_WhiteAreaTotal = %u\r\n"),g_WhiteAreaTotal);
	//TRACE(_T("UsefulArea1 = %u\r\n"),UsefulArea);

    for(int i=0;i<g_WhiteAreaTotal;i++)  
    {  

		if(g_WhiteArea[i].Active == 0){		//区域已经失效,被之前的兼并了
			continue;
		}

		for(int j=0;j<g_WhiteAreaTotal;j++){
			if(j==i)
				continue;

			if(g_WhiteArea[j].Active == 0){		//区域已经失效,被之前的兼并了
				continue;
			}

			if(g_WhiteArea[i].StartX>g_WhiteArea[j].EndX || g_WhiteArea[i].EndX < g_WhiteArea[j].StartX){		//X无交集
				continue;	
			}
			if(g_WhiteArea[i].StartY>g_WhiteArea[j].EndY || g_WhiteArea[i].EndY < g_WhiteArea[j].StartY){		//Y无交集
				continue;	
			}
			//X,Y都有交集
			BinaryAreaCombine::AreaAddArea(&g_WhiteArea[i],&g_WhiteArea[j]);//g_WhiteArea[j].Active = 0; 
			j=0;//吞并之后区域变大了，需要重新循环是否有新的小区域被吞并
			continue;
			//g_WhiteAreaTotal = g_WhiteAreaTotal>0?g_WhiteAreaTotal-1:0;
		}
    }  
}
/*
BinaryImage:二进制图像，保证目标是白色255，无用区域是0
*/

void BinaryAreaCombine::AreaCombine(IplImage* BinaryImage){ 
	EnterCriticalSection(&g_CS);		// 进入临界区
	if(this->g_StartFlag == 0)
		return;

	unsigned int ImageWidth = BinaryImage->width;
	unsigned int ImageHeight = BinaryImage->height;
	unsigned char *ImageData = (unsigned char*)BinaryImage->imageData;
	unsigned char CurPixel = 0;
	int i = 0, j = 0, k = 0;
	unsigned int AreaNumber = 0;	//用于Area计数
	unsigned int ShortLineInAreaCounter = 0;//用于Area增加Line，Or Area增加Area判断

	this->g_FrameCounter++;
	if(this->g_FrameCounter == 1){
		g_InputImage = cvCreateImage(cvSize(ImageWidth,ImageHeight),IPL_DEPTH_8U ,1);
		g_AreaImage = cvCreateImage(cvGetSize(g_InputImage),IPL_DEPTH_8U,1);  
		g_LineImage = cvCreateImage(cvGetSize(g_InputImage),8,1);

		g_LineDesc = (LINEDESC *)malloc(sizeof(LINEDESC)*ImageHeight);
		g_WhiteArea = (IMAGEAREA *)malloc(sizeof(IMAGEAREA)*IMAGEAREA_WHITEAREA_MAX);
	}else{
		memset(g_LineDesc,0,sizeof(LINEDESC)*ImageHeight);		//全局变量初始化 
		memset(g_WhiteArea,0,sizeof(IMAGEAREA)*IMAGEAREA_WHITEAREA_MAX);
		g_WhiteAreaTotal = 0;

		memcpy(g_InputImage->imageData,BinaryImage->imageData,ImageWidth*ImageHeight);	//拷贝图像到本层
#if BINARYAREA_DEBUG
		cvShowImage( "区域连通输入图", g_InputImage );  
#endif

		for(i=0;i<ImageHeight;i++){			//1.获取每一行的白短线描述
			g_LineDesc[i].ShortLineCounter = BinaryAreaCombine::GetWhiteLine(BinaryImage,i,g_LineDesc[i].ShortLine);
 		}
#if BINARYAREA_DEBUG
		BinaryAreaCombine::ShortLineDraw();
#endif

		for(i=1;i<ImageHeight;i++)		//遍历该行所有行i
		{
			for(j=0;j<g_LineDesc[i].ShortLineCounter;j++){	//遍历该行所有短白线j
				ShortLineInAreaCounter = 0;//初始化，该短白线不在Area上的数量为0
				if(g_LineDesc[i].ShortLine[j].Active == 1){	//判断短白线是否有效
					if(g_LineDesc[i-1].AreaCounter == 0){	//*该短白线上一行没有Area,开辟一个新的Area
						g_WhiteArea[AreaNumber].AreaID = AreaNumber;	//该AreaID初始化
						g_WhiteArea[AreaNumber].LineCounter = 1;		//该Area包含短白线数量
						g_WhiteArea[AreaNumber].StartX = g_LineDesc[i].ShortLine[j].StartX;
						g_WhiteArea[AreaNumber].EndX = g_LineDesc[i].ShortLine[j].EndX;
						g_WhiteArea[AreaNumber].StartY = i;
						g_WhiteArea[AreaNumber].EndY = i;
						g_WhiteArea[AreaNumber].Active = 1;
						g_LineDesc[i].AreaID[g_LineDesc[i].AreaCounter] = AreaNumber;		//该行涵盖的AreaID

						AreaNumber++;									//增加一个AreaID
						g_WhiteAreaTotal++;
						g_LineDesc[i].AreaCounter++;//该行涵盖的Area计数
					}else{
						for(k=0;k<g_LineDesc[i-1].AreaCounter;k++){	//遍历上一行所有Area区域k
							if((g_WhiteArea[g_LineDesc[i-1].AreaID[k]].EndX<g_LineDesc[i].ShortLine[j].StartX)||(g_WhiteArea[g_LineDesc[i-1].AreaID[k]].StartX>g_LineDesc[i].ShortLine[j].EndX)){ //上一行的第K个Area和本行短线不接触
								//非0行，区域的结束小于短线的开始，区域的开始大于短线的结束
							}else{		//如果有接触的地方，加该白短线加入到上一条白线所在的Area中
								ShortLineInAreaCounter++;
								if(ShortLineInAreaCounter == 1){		//该白色短线在第一个Area中，调用Area增加Line
									BinaryAreaCombine::AreaAddLine(&g_WhiteArea[g_LineDesc[i-1].AreaID[k]],&g_LineDesc[i].ShortLine[j],i);//Area增加该Line，该短白线的AreaID=该AreaID
									g_LineDesc[i].AreaID[g_LineDesc[i].AreaCounter] = g_LineDesc[i-1].AreaID[k];		//该行的AreaID组加上接触到的AreaID
									g_LineDesc[i].AreaCounter++;//该行涵盖的Area计数
								}else if(g_WhiteArea[g_LineDesc[i-1].AreaID[k]].EndY == i){	//同行后面的短白线和同行前面的短白线的Area是同一个Area，第一次已经更新了这个Area的EndY
									BinaryAreaCombine::AreaAddLine(&g_WhiteArea[g_LineDesc[i-1].AreaID[k]],&g_LineDesc[i].ShortLine[j],i);//Area增加该Line，该短白线的AreaID=该AreaID
								}else {	//该白色短线在其他Area中，调用该短白线所在Area(该短白线的Area)增加新接触的Area
									BinaryAreaCombine::AreaAddArea(&g_WhiteArea[g_LineDesc[i].ShortLine[j].AreaID],&g_WhiteArea[g_LineDesc[i-1].AreaID[k]]);//Area合并
									g_LineDesc[i-1].AreaID[k] = g_LineDesc[i].ShortLine[j].AreaID; //上一行该Area的ID更改为当前白线所在区域ID
									
								}
							}

							/*上一行有Area，但是与该短白线没有接触*/
							if((k == g_LineDesc[i-1].AreaCounter-1) && (ShortLineInAreaCounter == 0)){	//达到上行最后一个Area
								g_WhiteArea[AreaNumber].AreaID = AreaNumber;	//该AreaID初始化
								g_WhiteArea[AreaNumber].LineCounter = 1;		//该Area包含短白线数量
								g_WhiteArea[AreaNumber].StartX = g_LineDesc[i].ShortLine[j].StartX;
								g_WhiteArea[AreaNumber].EndX = g_LineDesc[i].ShortLine[j].EndX;
								g_WhiteArea[AreaNumber].StartY = i;
								g_WhiteArea[AreaNumber].EndY = i;
								g_WhiteArea[AreaNumber].Active = 1;
								g_LineDesc[i].AreaID[g_LineDesc[i].AreaCounter] = AreaNumber;		//该行涵盖的AreaID

								AreaNumber++;									//增加一个AreaID
								g_WhiteAreaTotal++;
								g_LineDesc[i].AreaCounter++;//该行涵盖的Area计数
							}
						
						}
					}

				}
			}

		}
#if BINARYAREA_DEBUG
		BinaryAreaCombine::AreaDraw();
#endif
	}
	BinaryAreaCombine::AreaCombineSelect();
	LeaveCriticalSection(&g_CS);		// 离开临界区
}

/*
画出已经标记的短线，理论上和输入图一样
*/

void on_mouse2( int event, int x, int y, int flags, void* ustc)    
{    
    CvFont font;    
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);    
    if( event == CV_EVENT_LBUTTONDOWN )           //点击鼠标显示位置信息  
    {    
          
        CvPoint pt = cvPoint(x,y);    
        char temp[16];    
        sprintf(temp,"(%d,%d)",pt.x,pt.y);    
        cvPutText(g_LineImage,temp, pt, &font, CV_RGB(255,0,0));    
        cvCircle(g_LineImage, pt, 2,CV_RGB(255,0,0) ,CV_FILLED, CV_AA, 0 );    
        cvShowImage("区域连通短线标记", g_LineImage );    
    }     
} 

void BinaryAreaCombine::ShortLineDraw(void){ 
	int i = 0;
	int j = 0;
	int ImageWidth = g_LineImage->width;
	int ImageHeight = g_LineImage->height;

    cvZero(g_LineImage);  

    for(int i=0;i<ImageHeight;i++)  
    {	
		for(j=0;j<g_LineDesc[i].ShortLineCounter;j++){
			cvLine(g_LineImage,cvPoint(g_LineDesc[i].ShortLine[j].StartX,i),cvPoint(g_LineDesc[i].ShortLine[j].EndX,i),CV_RGB(255,255,255));  
		}
    }  

	cvSetMouseCallback("区域连通短线标记", on_mouse2, 0 );  
    cvShowImage("区域连通短线标记", g_LineImage );  
}

/*
画出已经连通的区域
*/

void on_mouse( int event, int x, int y, int flags, void* ustc)    
{    
    CvFont font;    
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);    
    if( event == CV_EVENT_LBUTTONDOWN )           //点击鼠标显示位置信息  
    {    
          
        CvPoint pt = cvPoint(x,y);    
        char temp[16];    
        sprintf(temp,"(%d,%d)",pt.x,pt.y);    
        cvPutText(g_AreaImage,temp, pt, &font, CV_RGB(255,0,0));    
        cvCircle(g_AreaImage, pt, 2,CV_RGB(255,0,0) ,CV_FILLED, CV_AA, 0 );    
        cvShowImage("区域连通标记", g_AreaImage );    
    }     
} 

void BinaryAreaCombine::AreaDraw(void){ 
    //创建一张一维直方图的“图”，横坐标为灰度级，纵坐标为像素个数（*scale）  
    cvZero(g_AreaImage);  
#if 0
    CvFont font;    
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);    
	cvPutText(g_mouseImage,"Test",cvPoint(100,100), &font, cvScalar(255, 0, 0, 0));  
#endif
    //分别将每个直方块的值绘制到图中  
    for(int i=0;i<g_WhiteAreaTotal;i++)  
    {  
		if(g_WhiteArea[i].Active)
			cvRectangle(g_AreaImage,  
			cvPoint(g_WhiteArea[i].StartX,g_WhiteArea[i].StartY),  
            cvPoint(g_WhiteArea[i].EndX,g_WhiteArea[i].EndY),  
            CV_RGB(255,255,255));    
    }  

	cvSetMouseCallback("区域连通标记", on_mouse, 0 );  
	cvShowImage("区域连通标记", g_AreaImage );  
}

unsigned int BinaryAreaCombine::GetAreaSize(IMAGEAREA Area)
{
	return (Area.EndX-Area.StartX)*(Area.EndY-Area.StartY);
}
void BinaryAreaCombine::CombineAllArea(IMAGEAREA *MaxArea){

	unsigned int MaxLeft = 0xffff,MaxRight = 0,MaxTop = 0xffff,MaxBottom = 0;
    for(int i=0;i<g_WhiteAreaTotal;i++)  
    {
		if(g_WhiteArea[i].Active){
			if(g_WhiteArea[i].StartX<MaxLeft)
				MaxLeft = g_WhiteArea[i].StartX;
			if(g_WhiteArea[i].EndY>MaxRight)
				MaxRight = g_WhiteArea[i].EndY;
			if(g_WhiteArea[i].StartY<MaxTop)
				MaxTop = g_WhiteArea[i].StartY;
			if(g_WhiteArea[i].EndY>MaxBottom)
				MaxBottom = g_WhiteArea[i].EndY;
		}
	}

	MaxArea->StartX = MaxLeft;
	MaxArea->EndX = MaxRight;
	MaxArea->StartY = MaxTop;
	MaxArea->EndY = MaxBottom;

}
void BinaryAreaCombine::GetMaxArea(IMAGEAREA *MaxArea){  
	unsigned int MaxAreaSize = 0;
	unsigned int CurAreaSize = 0;
	IMAGEAREA *MaxAreaAddr = NULL;
    //分别将每个直方块的值绘制到图中  
    for(int i=0;i<g_WhiteAreaTotal;i++)  
    {  
		if(g_WhiteArea[i].Active){
			CurAreaSize = BinaryAreaCombine::GetAreaSize(g_WhiteArea[i]);
			if(CurAreaSize>MaxAreaSize){
				MaxAreaAddr = &g_WhiteArea[i];
				MaxAreaSize = CurAreaSize;
			}
		}
			  
    }  

	memcpy(MaxArea,MaxAreaAddr,sizeof(IMAGEAREA));
}

unsigned int BinaryAreaCombine::GetAreaCounter(void)
{
	return g_WhiteAreaTotal;
}

void BinaryAreaCombine::GetAreaInfo(IMAGEAREA *MaxArea,unsigned int AreaID)
{
	memcpy(MaxArea,&g_WhiteArea[AreaID],sizeof(IMAGEAREA));
}
/*
 区域从大到小排序
*/
unsigned int BinaryAreaCombine::AreaMaxToMinSort(unsigned int *SortID,unsigned int SortNum){  
	unsigned int MaxAreaSize = 0;
	unsigned int CurAreaSize = 0;
	IMAGEAREA *MaxAreaAddr = NULL;
    //分别将每个直方块的值绘制到图中
	int i = 0, j = 0;
	unsigned char *ActiveTemp;
	unsigned char UsefulArea = 0;
	ActiveTemp = (unsigned char *)malloc(g_WhiteAreaTotal);
	memset(ActiveTemp,0,g_WhiteAreaTotal);

	for(i=0;i<g_WhiteAreaTotal;i++){
		if(g_WhiteArea[i].Active == 1){
			ActiveTemp[i] = 1;
			UsefulArea++;
		}
	}
	//TRACE(_T("Sort UsefulArea = %u\r\n"),UsefulArea);

    for(i=0;i<SortNum;i++)  //找SortNum个最大值
    {  
		MaxAreaAddr = NULL;
		MaxAreaSize = 0;
		for(j=0;j<g_WhiteAreaTotal;j++)  {
			if(ActiveTemp[j]){		//未激活的兼并Area不考虑
				CurAreaSize = BinaryAreaCombine::GetAreaSize(g_WhiteArea[j]);
				if(CurAreaSize>MaxAreaSize){
					MaxAreaAddr = &g_WhiteArea[j];
					MaxAreaSize = CurAreaSize;
				}
			 }
		}
		if(MaxAreaAddr != NULL){
			SortID[i] = MaxAreaAddr->AreaID;
			//TRACE(_T("SortID[i] = %d\r\n"),SortID[i]);
			ActiveTemp[MaxAreaAddr->AreaID] = 0;//UnActive the Max Area.
		}else{
			free(ActiveTemp);
			return i;		//没有更多Area了
		}
    }  

	free(ActiveTemp);
	return SortNum;
}
