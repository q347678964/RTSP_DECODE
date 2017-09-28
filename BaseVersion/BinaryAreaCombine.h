#ifndef BINARYAREA_COMBINE_H
#define BINARYAREA_COMBINE_H

#define BINARYAREA_DEBUG 0

#define IMAGELINE_SHORTLINE_MAX 720	//ÿ�а����У����̰�����
#define IMAGELINE_HEIGHT_MAX 1280	//�������,ʵ����ͼ�����������
#define IMAGEAREA_WHITEAREA_MAX	1000	//����ɫ������
#define BLACK_PIXEL 0
#define WHITE_PIXEL 255

typedef struct IMAGE_AREA
{
	int LineCounter;
	unsigned int AreaID;
	int StartX;
	int StartY;
	int EndX;
	int EndY;
	int Active;
}IMAGEAREA;

typedef struct SHORT_LINE{
 unsigned int StartX;
 unsigned int EndX;
 unsigned int Active;	// 0/1
 unsigned int AreaID;	//�̰������ڵ�AreaID
}SHORTLINE;

typedef struct LINE_DESC
{
	unsigned int ShortLineCounter;					//���ж̰�������
	SHORTLINE ShortLine[IMAGELINE_SHORTLINE_MAX];	//���ж̰���
	unsigned int AreaCounter;						//����������Area����
	unsigned int AreaID[IMAGELINE_SHORTLINE_MAX];	//���������ǵ�AreaID
}LINEDESC;

class BinaryAreaCombine{
public:
	CRITICAL_SECTION g_CS;              // �ٽ����ṹ����
	unsigned int g_FrameCounter;
	bool g_StartFlag;
	LINEDESC *g_LineDesc;
	IMAGEAREA *g_WhiteArea;

	unsigned int g_WhiteAreaTotal;
public:

	/*Function*/
	void BinaryAreaCombine::Start(void);
	void BinaryAreaCombine::Stop(void);
	int  BinaryAreaCombine::GetPixel(IplImage* BinaryImage,int x ,int y);
	unsigned int BinaryAreaCombine::GetWhiteLine(IplImage* BinaryImage,unsigned int LineNumber,SHORTLINE *Line);
	void BinaryAreaCombine::AreaAddLine(IMAGEAREA *Area,SHORTLINE *Line,int LineY);
	void BinaryAreaCombine::AreaAddArea(IMAGEAREA* Area1,IMAGEAREA *Area2);
	void BinaryAreaCombine::AreaCombine(IplImage* BinaryImage);
	void BinaryAreaCombine::AreaCombineSelect(void);
	void BinaryAreaCombine::AreaDraw(void);
	void BinaryAreaCombine::ShortLineDraw(void);
	unsigned int BinaryAreaCombine::GetAreaSize(IMAGEAREA Area);
	void BinaryAreaCombine::CombineAllArea(IMAGEAREA *MaxArea);
	void BinaryAreaCombine::GetMaxArea(IMAGEAREA *MaxArea);
	unsigned int BinaryAreaCombine::AreaMaxToMinSort(unsigned int *SortID,unsigned int SortNum);
	unsigned int BinaryAreaCombine::GetAreaCounter(void);
	void BinaryAreaCombine::GetAreaInfo(IMAGEAREA *MaxArea,unsigned int AreaID);
};

#endif