#ifndef CONFIG_H
#define CONFIG_H

//UI
#define CFG_CSTRING_BGP					"../MyResource/����1.jpg"
#define EDIT_PRINT_BG_RGB				RGB(172,252,150)
#define EDIT_PRINT_TEXT_RGB				RGB(89,89,255)

//FFMPEG ����
#define URL "rtsp://192.168.0.103:8554/live.sdp"
#define CFG_FFMPEG_SAVE_FILE			0
#define CFG_SAVE_H264_PATH				"../Output/Temp.H264"
#define CFG_SAVE_H264_PATH2				"../Output/Temp2.H264"
#define CFG_RTSP_METHOD					"tcp"
#define CFG_LOSE_FRAME_RESTART					120
//OPENCV ����
#define CFG_OPENCV_MOTION_DETECT			1000
#define CFG_OPENCV_RTSP_WIN					"RTSP��"
#define CFG_OPENCV_OUTPUT_WIN				"ͼ������"

//ͼ��ʶ���

#define DEFINE_MOTION_DETECTION			0
#define DEFINE_GAUSS_MODE				1
#endif
