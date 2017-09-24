#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"
#include "Example.h"
#include "BaseVersionDlg.h"
/*************************************************************************************/
//���졢��������
Example::Example(){
	this->Printf((CString)"***********************\r\n");
}

Example::~Example(){

}
/*************************************************************************************/
//���������
void Example::Printf(CString Context){
	CTime time = CTime::GetCurrentTime();   ///����CTime����
	CString m_strTime = time.Format("[%H:%M:%S]");

	Context = m_strTime + (CString)("[Example]") + Context;
    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->Printf(Context);
}

void Example::ProcessCtrl(int Num){

    CBaseVersionDlg *MainDlg = (CBaseVersionDlg *)AfxGetMainWnd();
	MainDlg->UIOperationCB(IDC_PROGRESS1,Num);
}
/*************************************************************************************/
//�̲߳���
DWORD WINAPI ThreadProc(LPVOID pParam)
{
	Example *Ex = (Example *)pParam;
	Ex->Printf((CString)"�߳�����\r\n");
	Sleep(1000);
	Ex->Printf((CString)"�߳̽���\r\n");
	return 0;
}

void Example::ThreadTest(void){
	this->Printf((CString)"�̲߳��Կ�ʼ\r\n");
	AfxBeginThread((AFX_THREADPROC)ThreadProc,this,THREAD_PRIORITY_HIGHEST);
	this->Printf((CString)"�߳��Ѿ�����\r\n");
}
/*************************************************************************************/
//ʱ���������
void Example::TimerTest(void){

	this->Printf((CString)"ʱ����Կ�ʼ\r\n");

	clock_t TimeStart,TimeEnd;
	unsigned int TimeSpend;
	TimeStart = clock();
	Sleep(100);
	TimeEnd = clock();

	TimeSpend = TimeEnd - TimeStart;

	CString TempCString;
	TempCString.Format(_T("ʱ����:%u����\r\n"),TimeSpend);
	this->Printf(TempCString);

	CTime time = CTime::GetCurrentTime();   ///����CTime����
#if 0
	int m_nYear = time.GetYear();      ///��
	int m_nMonth = time.GetMonth();      ///��
	int m_nDay = time.GetDay();      ///��
	int m_nHour = time.GetHour();      ///Сʱ
	int m_nMinute = time.GetMinute();   ///����
	int m_nSecond = time.GetSecond();   ///��
#endif
	CString m_strTime = time.Format("%Y-%m-%d %H:%M:%S");
	m_strTime += (CString)("\r\n");
	this->Printf(m_strTime);
	this->Printf((CString)"ʱ��������\r\n");

}

/*************************************************************************************/
//�ļ���������
void Example::FileOperation(void){
	this->Printf((CString)"�ļ��������Կ�ʼ\r\n");

	const char data[] = {"This is a test program"};
	CFile mFile(_T("../Output/Test.txt"),CFile::modeCreate | CFile::modeReadWrite);
	mFile.Write(data,sizeof(data));
	mFile.Close();

	this->Printf((CString)"�ļ������������\r\n");

}

/*************************************************************************************/
//����������
DWORD WINAPI ProcessTestThread(LPVOID pParam)
{
	Example *Ex = (Example *)pParam;
	Ex->Printf((CString)"�������������Կ�ʼ\r\n");
	for(int i=0;i<100;i++){
		Sleep(100);
		Ex->ProcessCtrl(i);
	}
	Ex->Printf((CString)"�������������Խ���\r\n");
	return 0;
}

void Example::ProcessTest(void){
	AfxBeginThread((AFX_THREADPROC)ProcessTestThread,this,THREAD_PRIORITY_HIGHEST);
}
/*************************************************************************************/
//��ȡ����IP��ַ
void Example::GetHostAddress(CString &strIPAddr)
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData);

	char    HostName[100];
	gethostname(HostName, sizeof(HostName));// ��ñ���������.

	hostent* hn;
	hn = gethostbyname(HostName);//���ݱ����������õ�����ip

	strIPAddr=inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//��ip�����ַ�����ʽ

	WSACleanup();	//����ǵ��ͷ�
}

void Example::NetTest(void){
	this->Printf((CString)"������ز��Կ�ʼ\r\n");
	CString IPCString;
	this->GetHostAddress(IPCString);
	IPCString = (CString)("����IP��ַ:")+ IPCString + (CString)("\r\n");
	this->Printf(IPCString);
	this->Printf((CString)"������ز��Խ���\r\n");
}
/*************************************************************************************/
//Windows CMD�����������
void Example::WinCMDTest(void){
	this->Printf((CString)"Windows CMD�������:ipconfig\r\n");
	WinExec("ipconfig", SW_HIDE);
}