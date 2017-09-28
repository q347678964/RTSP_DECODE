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
	cvNamedWindow("������ͨ����ͼ", CV_WINDOW_AUTOSIZE );  
    cvNamedWindow("������ͨ���߱��", CV_WINDOW_AUTOSIZE );  
    cvNamedWindow("������ͨ���", CV_WINDOW_AUTOSIZE );  
#endif
}

void BinaryAreaCombine::Stop(void){
	EnterCriticalSection(&g_CS);		// �����ٽ���
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
	cvDestroyWindow("������ͨ����ͼ");
	cvDestroyWindow("������ͨ���߱��");
	cvDestroyWindow("������ͨ���");
#endif
	LeaveCriticalSection(&g_CS);		// �뿪�ٽ���
}

/*
*BinaryImage: ������ͼ��
*x:������
*y:������
*Ret:Pixelֵ
*/
//��ȡ������ͼ��ĳ���������ֵ0~255
int BinaryAreaCombine::GetPixel(IplImage* BinaryImage,int x ,int y)
{
	unsigned char *ImageData = (unsigned char*)BinaryImage->imageData;
	int ImageWidth = BinaryImage->width;
	int ImageHeight = BinaryImage->height;
	int step = BinaryImage->widthStep/sizeof(uchar); //ͼ��������4�ֽڶ��������������Ҫ��step������ֱ����width

	if(x<0||x>=ImageWidth||y<0||y>=ImageHeight)
		return -1;
	else
		return ImageData[y*step+x];
		//return cvGet2D(BinaryImage,y,x).val[0];
		//return *(ImageData+y*step+x);
}
/*
*��ȡͼ��ĳһ���а��ߵ�����������,����һ����������
*BinaryImage ������ͼ��
*LineNumber �к�
*Line ��������
*Ret ������
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
			if(CurPixel == WHITE_PIXEL){	//�ҵ���ɫ���'
				Line[LineCounter].StartX = i;
				if(i != (ImageWidth-1)){	//�жϲ��ǵ�ǰ�е����һ����
					while(1){			//ѭ��ֱ������������һ���ڵ�
						i++;
						CurPixel = BinaryAreaCombine::GetPixel(BinaryImage,i,LineNumber);
						if(CurPixel == BLACK_PIXEL ){		//�ҵ������ڵ���߽���
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
			}//һ�����߽���

			if(Line[LineCounter].StartX != Line[LineCounter].EndX){	//�ų������׵�����
				Line[LineCounter].Active = 1;
				LineCounter++;
			}
	}

	return LineCounter;
}

#define MIN(x1,x2) ((x1)>=(x2)?(x2):(x1))
#define MAX(x1,x2) ((x1)>=(x2)?(x1):(x2))

/*
��ĳ����������һ���̰��ߣ�����µ�����
Area:����
Line:�̰���
LineY:�̰������ڵ�Yֵ
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
��ĳ����������һ����������µ�����
Area:����
Line:�̰���
LineY:�̰������ڵ�Yֵ
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

/*�����˴˷����Ŀ�꣬ ���ǻ����ʱ�����п������ظ����֣���ѡ�ĵ��øú������кϲ�*/
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

		if(g_WhiteArea[i].Active == 0){		//�����Ѿ�ʧЧ,��֮ǰ�ļ沢��
			continue;
		}

		for(int j=0;j<g_WhiteAreaTotal;j++){
			if(j==i)
				continue;

			if(g_WhiteArea[j].Active == 0){		//�����Ѿ�ʧЧ,��֮ǰ�ļ沢��
				continue;
			}

			if(g_WhiteArea[i].StartX>g_WhiteArea[j].EndX || g_WhiteArea[i].EndX < g_WhiteArea[j].StartX){		//X�޽���
				continue;	
			}
			if(g_WhiteArea[i].StartY>g_WhiteArea[j].EndY || g_WhiteArea[i].EndY < g_WhiteArea[j].StartY){		//Y�޽���
				continue;	
			}
			//X,Y���н���
			BinaryAreaCombine::AreaAddArea(&g_WhiteArea[i],&g_WhiteArea[j]);//g_WhiteArea[j].Active = 0; 
			j=0;//�̲�֮���������ˣ���Ҫ����ѭ���Ƿ����µ�С�����̲�
			continue;
			//g_WhiteAreaTotal = g_WhiteAreaTotal>0?g_WhiteAreaTotal-1:0;
		}
    }  
}
/*
BinaryImage:������ͼ�񣬱�֤Ŀ���ǰ�ɫ255������������0
*/

void BinaryAreaCombine::AreaCombine(IplImage* BinaryImage){ 
	EnterCriticalSection(&g_CS);		// �����ٽ���
	if(this->g_StartFlag == 0)
		return;

	unsigned int ImageWidth = BinaryImage->width;
	unsigned int ImageHeight = BinaryImage->height;
	unsigned char *ImageData = (unsigned char*)BinaryImage->imageData;
	unsigned char CurPixel = 0;
	int i = 0, j = 0, k = 0;
	unsigned int AreaNumber = 0;	//����Area����
	unsigned int ShortLineInAreaCounter = 0;//����Area����Line��Or Area����Area�ж�

	this->g_FrameCounter++;
	if(this->g_FrameCounter == 1){
		g_InputImage = cvCreateImage(cvSize(ImageWidth,ImageHeight),IPL_DEPTH_8U ,1);
		g_AreaImage = cvCreateImage(cvGetSize(g_InputImage),IPL_DEPTH_8U,1);  
		g_LineImage = cvCreateImage(cvGetSize(g_InputImage),8,1);

		g_LineDesc = (LINEDESC *)malloc(sizeof(LINEDESC)*ImageHeight);
		g_WhiteArea = (IMAGEAREA *)malloc(sizeof(IMAGEAREA)*IMAGEAREA_WHITEAREA_MAX);
	}else{
		memset(g_LineDesc,0,sizeof(LINEDESC)*ImageHeight);		//ȫ�ֱ�����ʼ�� 
		memset(g_WhiteArea,0,sizeof(IMAGEAREA)*IMAGEAREA_WHITEAREA_MAX);
		g_WhiteAreaTotal = 0;

		memcpy(g_InputImage->imageData,BinaryImage->imageData,ImageWidth*ImageHeight);	//����ͼ�񵽱���
#if BINARYAREA_DEBUG
		cvShowImage( "������ͨ����ͼ", g_InputImage );  
#endif

		for(i=0;i<ImageHeight;i++){			//1.��ȡÿһ�еİ׶�������
			g_LineDesc[i].ShortLineCounter = BinaryAreaCombine::GetWhiteLine(BinaryImage,i,g_LineDesc[i].ShortLine);
 		}
#if BINARYAREA_DEBUG
		BinaryAreaCombine::ShortLineDraw();
#endif

		for(i=1;i<ImageHeight;i++)		//��������������i
		{
			for(j=0;j<g_LineDesc[i].ShortLineCounter;j++){	//�����������ж̰���j
				ShortLineInAreaCounter = 0;//��ʼ�����ö̰��߲���Area�ϵ�����Ϊ0
				if(g_LineDesc[i].ShortLine[j].Active == 1){	//�ж϶̰����Ƿ���Ч
					if(g_LineDesc[i-1].AreaCounter == 0){	//*�ö̰�����һ��û��Area,����һ���µ�Area
						g_WhiteArea[AreaNumber].AreaID = AreaNumber;	//��AreaID��ʼ��
						g_WhiteArea[AreaNumber].LineCounter = 1;		//��Area�����̰�������
						g_WhiteArea[AreaNumber].StartX = g_LineDesc[i].ShortLine[j].StartX;
						g_WhiteArea[AreaNumber].EndX = g_LineDesc[i].ShortLine[j].EndX;
						g_WhiteArea[AreaNumber].StartY = i;
						g_WhiteArea[AreaNumber].EndY = i;
						g_WhiteArea[AreaNumber].Active = 1;
						g_LineDesc[i].AreaID[g_LineDesc[i].AreaCounter] = AreaNumber;		//���к��ǵ�AreaID

						AreaNumber++;									//����һ��AreaID
						g_WhiteAreaTotal++;
						g_LineDesc[i].AreaCounter++;//���к��ǵ�Area����
					}else{
						for(k=0;k<g_LineDesc[i-1].AreaCounter;k++){	//������һ������Area����k
							if((g_WhiteArea[g_LineDesc[i-1].AreaID[k]].EndX<g_LineDesc[i].ShortLine[j].StartX)||(g_WhiteArea[g_LineDesc[i-1].AreaID[k]].StartX>g_LineDesc[i].ShortLine[j].EndX)){ //��һ�еĵ�K��Area�ͱ��ж��߲��Ӵ�
								//��0�У�����Ľ���С�ڶ��ߵĿ�ʼ������Ŀ�ʼ���ڶ��ߵĽ���
							}else{		//����нӴ��ĵط����Ӹð׶��߼��뵽��һ���������ڵ�Area��
								ShortLineInAreaCounter++;
								if(ShortLineInAreaCounter == 1){		//�ð�ɫ�����ڵ�һ��Area�У�����Area����Line
									BinaryAreaCombine::AreaAddLine(&g_WhiteArea[g_LineDesc[i-1].AreaID[k]],&g_LineDesc[i].ShortLine[j],i);//Area���Ӹ�Line���ö̰��ߵ�AreaID=��AreaID
									g_LineDesc[i].AreaID[g_LineDesc[i].AreaCounter] = g_LineDesc[i-1].AreaID[k];		//���е�AreaID����ϽӴ�����AreaID
									g_LineDesc[i].AreaCounter++;//���к��ǵ�Area����
								}else if(g_WhiteArea[g_LineDesc[i-1].AreaID[k]].EndY == i){	//ͬ�к���Ķ̰��ߺ�ͬ��ǰ��Ķ̰��ߵ�Area��ͬһ��Area����һ���Ѿ����������Area��EndY
									BinaryAreaCombine::AreaAddLine(&g_WhiteArea[g_LineDesc[i-1].AreaID[k]],&g_LineDesc[i].ShortLine[j],i);//Area���Ӹ�Line���ö̰��ߵ�AreaID=��AreaID
								}else {	//�ð�ɫ����������Area�У����øö̰�������Area(�ö̰��ߵ�Area)�����½Ӵ���Area
									BinaryAreaCombine::AreaAddArea(&g_WhiteArea[g_LineDesc[i].ShortLine[j].AreaID],&g_WhiteArea[g_LineDesc[i-1].AreaID[k]]);//Area�ϲ�
									g_LineDesc[i-1].AreaID[k] = g_LineDesc[i].ShortLine[j].AreaID; //��һ�и�Area��ID����Ϊ��ǰ������������ID
									
								}
							}

							/*��һ����Area��������ö̰���û�нӴ�*/
							if((k == g_LineDesc[i-1].AreaCounter-1) && (ShortLineInAreaCounter == 0)){	//�ﵽ�������һ��Area
								g_WhiteArea[AreaNumber].AreaID = AreaNumber;	//��AreaID��ʼ��
								g_WhiteArea[AreaNumber].LineCounter = 1;		//��Area�����̰�������
								g_WhiteArea[AreaNumber].StartX = g_LineDesc[i].ShortLine[j].StartX;
								g_WhiteArea[AreaNumber].EndX = g_LineDesc[i].ShortLine[j].EndX;
								g_WhiteArea[AreaNumber].StartY = i;
								g_WhiteArea[AreaNumber].EndY = i;
								g_WhiteArea[AreaNumber].Active = 1;
								g_LineDesc[i].AreaID[g_LineDesc[i].AreaCounter] = AreaNumber;		//���к��ǵ�AreaID

								AreaNumber++;									//����һ��AreaID
								g_WhiteAreaTotal++;
								g_LineDesc[i].AreaCounter++;//���к��ǵ�Area����
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
	LeaveCriticalSection(&g_CS);		// �뿪�ٽ���
}

/*
�����Ѿ���ǵĶ��ߣ������Ϻ�����ͼһ��
*/

void on_mouse2( int event, int x, int y, int flags, void* ustc)    
{    
    CvFont font;    
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);    
    if( event == CV_EVENT_LBUTTONDOWN )           //��������ʾλ����Ϣ  
    {    
          
        CvPoint pt = cvPoint(x,y);    
        char temp[16];    
        sprintf(temp,"(%d,%d)",pt.x,pt.y);    
        cvPutText(g_LineImage,temp, pt, &font, CV_RGB(255,0,0));    
        cvCircle(g_LineImage, pt, 2,CV_RGB(255,0,0) ,CV_FILLED, CV_AA, 0 );    
        cvShowImage("������ͨ���߱��", g_LineImage );    
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

	cvSetMouseCallback("������ͨ���߱��", on_mouse2, 0 );  
    cvShowImage("������ͨ���߱��", g_LineImage );  
}

/*
�����Ѿ���ͨ������
*/

void on_mouse( int event, int x, int y, int flags, void* ustc)    
{    
    CvFont font;    
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);    
    if( event == CV_EVENT_LBUTTONDOWN )           //��������ʾλ����Ϣ  
    {    
          
        CvPoint pt = cvPoint(x,y);    
        char temp[16];    
        sprintf(temp,"(%d,%d)",pt.x,pt.y);    
        cvPutText(g_AreaImage,temp, pt, &font, CV_RGB(255,0,0));    
        cvCircle(g_AreaImage, pt, 2,CV_RGB(255,0,0) ,CV_FILLED, CV_AA, 0 );    
        cvShowImage("������ͨ���", g_AreaImage );    
    }     
} 

void BinaryAreaCombine::AreaDraw(void){ 
    //����һ��һάֱ��ͼ�ġ�ͼ����������Ϊ�Ҷȼ���������Ϊ���ظ�����*scale��  
    cvZero(g_AreaImage);  
#if 0
    CvFont font;    
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);    
	cvPutText(g_mouseImage,"Test",cvPoint(100,100), &font, cvScalar(255, 0, 0, 0));  
#endif
    //�ֱ�ÿ��ֱ�����ֵ���Ƶ�ͼ��  
    for(int i=0;i<g_WhiteAreaTotal;i++)  
    {  
		if(g_WhiteArea[i].Active)
			cvRectangle(g_AreaImage,  
			cvPoint(g_WhiteArea[i].StartX,g_WhiteArea[i].StartY),  
            cvPoint(g_WhiteArea[i].EndX,g_WhiteArea[i].EndY),  
            CV_RGB(255,255,255));    
    }  

	cvSetMouseCallback("������ͨ���", on_mouse, 0 );  
	cvShowImage("������ͨ���", g_AreaImage );  
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
    //�ֱ�ÿ��ֱ�����ֵ���Ƶ�ͼ��  
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
 ����Ӵ�С����
*/
unsigned int BinaryAreaCombine::AreaMaxToMinSort(unsigned int *SortID,unsigned int SortNum){  
	unsigned int MaxAreaSize = 0;
	unsigned int CurAreaSize = 0;
	IMAGEAREA *MaxAreaAddr = NULL;
    //�ֱ�ÿ��ֱ�����ֵ���Ƶ�ͼ��
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

    for(i=0;i<SortNum;i++)  //��SortNum�����ֵ
    {  
		MaxAreaAddr = NULL;
		MaxAreaSize = 0;
		for(j=0;j<g_WhiteAreaTotal;j++)  {
			if(ActiveTemp[j]){		//δ����ļ沢Area������
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
			return i;		//û�и���Area��
		}
    }  

	free(ActiveTemp);
	return SortNum;
}
