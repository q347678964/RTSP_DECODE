#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"
#include "Example.h"
#include "BaseVersionDlg.h"
/*************************************************************************************/
//构造、析构函数
Example::Example(){
	this->Printf((CString)"***********************\r\n");
}

Example::~Example(){

}
/*************************************************************************************/
//主窗体操作
void Example::Printf(CString Context){
	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
	CString m_strTime = time.Format("[%H:%M:%S]");

	Context = m_strTime + (CString)("[Example]") + Context;
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->Printf(Context);
}

void Example::ProcessCtrl(int Num){

    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->ProcessCtrl(Num);
}
/*************************************************************************************/
//线程测试
DWORD WINAPI ThreadProc(LPVOID pParam)
{
	Example *Ex = (Example *)pParam;
	Ex->Printf((CString)"线程休眠\r\n");
	Sleep(1000);
	Ex->Printf((CString)"线程结束\r\n");
	return 0;
}

void Example::ThreadTest(void){
	this->Printf((CString)"线程测试开始\r\n");
	AfxBeginThread((AFX_THREADPROC)ThreadProc,this,THREAD_PRIORITY_HIGHEST);
	this->Printf((CString)"线程已经创建\r\n");
}
/*************************************************************************************/
//时间操作测试
void Example::TimerTest(void){

	this->Printf((CString)"时间测试开始\r\n");

	clock_t TimeStart,TimeEnd;
	unsigned int TimeSpend;
	TimeStart = clock();
	Sleep(100);
	TimeEnd = clock();

	TimeSpend = TimeEnd - TimeStart;

	CString TempCString;
	TempCString.Format(_T("时间间隔:%u毫秒\r\n"),TimeSpend);
	this->Printf(TempCString);

	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
#if 0
	int m_nYear = time.GetYear();      ///年
	int m_nMonth = time.GetMonth();      ///月
	int m_nDay = time.GetDay();      ///日
	int m_nHour = time.GetHour();      ///小时
	int m_nMinute = time.GetMinute();   ///分钟
	int m_nSecond = time.GetSecond();   ///秒
#endif
	CString m_strTime = time.Format("%Y-%m-%d %H:%M:%S");
	m_strTime += (CString)("\r\n");
	this->Printf(m_strTime);
	this->Printf((CString)"时间测试完成\r\n");

}

/*************************************************************************************/
//文件操作测试
void Example::FileOperation(void){
	this->Printf((CString)"文件操作测试开始\r\n");

	const char data[] = {"This is a test program"};
	CFile mFile(_T("../Output/Test.txt"),CFile::modeCreate | CFile::modeReadWrite);
	mFile.Write(data,sizeof(data));
	mFile.Close();

	this->Printf((CString)"文件操作测试完成\r\n");

}

/*************************************************************************************/
//进度条测试
DWORD WINAPI ProcessTestThread(LPVOID pParam)
{
	Example *Ex = (Example *)pParam;
	Ex->Printf((CString)"进度条操作测试开始\r\n");
	for(int i=0;i<100;i++){
		Sleep(100);
		Ex->ProcessCtrl(i);
	}
	Ex->Printf((CString)"进度条操作测试结束\r\n");
	return 0;
}

void Example::ProcessTest(void){
	AfxBeginThread((AFX_THREADPROC)ProcessTestThread,this,THREAD_PRIORITY_HIGHEST);
}
/*************************************************************************************/
//获取本机IP地址
void Example::GetHostAddress(CString &strIPAddr)
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData);

	char    HostName[100];
	gethostname(HostName, sizeof(HostName));// 获得本机主机名.

	hostent* hn;
	hn = gethostbyname(HostName);//根据本机主机名得到本机ip

	strIPAddr=inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//把ip换成字符串形式

	WSACleanup();	//用完记得释放
}

void Example::NetTest(void){
	this->Printf((CString)"网络相关测试开始\r\n");
	CString IPCString;
	this->GetHostAddress(IPCString);
	IPCString = (CString)("本机IP地址:")+ IPCString + (CString)("\r\n");
	this->Printf(IPCString);
	this->Printf((CString)"网络相关测试结束\r\n");
}
/*************************************************************************************/
//Windows CMD命令操作测试
void Example::WinCMDTest(void){
	this->Printf((CString)"Windows CMD命令测试:ipconfig\r\n");
	WinExec("ipconfig", SW_HIDE);
}