
// BaseVersionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BaseVersion.h"
#include "BaseVersionDlg.h"
#include "afxdialogex.h"
#include "Config.h"
#include "FormatChange.h"
#include "Example.h"
#include "ffmpeg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBaseVersionDlg �Ի���




CBaseVersionDlg::CBaseVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBaseVersionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CBaseVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaseVersionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_OpenFile, &CBaseVersionDlg::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_Test, &CBaseVersionDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_RTSPStart, &CBaseVersionDlg::OnBnClickedButtonRtspstart)
	ON_BN_CLICKED(IDC_BUTTON_RTSPEnd, &CBaseVersionDlg::OnBnClickedButtonRtspend)
END_MESSAGE_MAP()


// CBaseVersionDlg ��Ϣ��������

BOOL CBaseVersionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	//SetIcon(m_hIcon, TRUE); // Set big icon  
	//SetIcon(m_hIcon, FALSE); // Set small icon; 

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	this->Printf((CString)("[Dialog]��ʼ���������!\r\n"));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CBaseVersionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CBaseVersionDlg::DlgPaintInit(void)
{
	CImage mImage;  
    if(mImage.Load(_T(CFG_CSTRING_BGP)) == S_OK)  {
		CWnd *pWnd[20];  
		CRect RectTemp;
        //�����ô��ڱ��ֺͱ���ͼһ�� 
		int WinDlgWidth = mImage.GetWidth();
		int WinDlgHeight = mImage.GetHeight();
		SetWindowPos(NULL,0,0,WinDlgWidth,WinDlgHeight,SWP_NOMOVE);
		pWnd[0] = GetDlgItem(IDC_EDIT_Debug);
		pWnd[0]->SetWindowPos( NULL,10,10,WinDlgWidth/3,WinDlgHeight/3,SWP_NOZORDER);//���Դ���,���ݴ����С���任
		pWnd[0]->GetWindowRect(RectTemp);//��ȡĿ������Ļ�ϵ����꣬��Ҫת������������
		ScreenToClient(RectTemp);

		pWnd[1] = GetDlgItem(IDC_EDIT_Path);
		pWnd[1]->SetWindowPos( NULL,RectTemp.right+10,10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//·�����ڣ�ֻ�ı����꣬���ı��С
		pWnd[1]->GetWindowRect(RectTemp);//��ȡĿ������Ļ�ϵ����꣬��Ҫת������������
		ScreenToClient(RectTemp);

		pWnd[2] = GetDlgItem(IDC_BUTTON_OpenFile);
		pWnd[2]->SetWindowPos( NULL,RectTemp.right+10,10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//·���򿪰�ť��ֻ�ı����꣬���ı��С
		((CButton *)GetDlgItem(IDC_BUTTON_OpenFile))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1)); 
		pWnd[2]->GetWindowRect(RectTemp);//��ȡĿ������Ļ�ϵ����꣬��Ҫת������������
		ScreenToClient(RectTemp);

		pWnd[3] = GetDlgItem(IDC_BUTTON_Test);
		pWnd[3]->SetWindowPos( NULL,RectTemp.right+10,10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//���԰�ť��ֻ�ı����꣬���ı��С
		((CButton *)GetDlgItem(IDC_BUTTON_Test))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1)); 

		pWnd[1]->GetWindowRect(RectTemp);//����������·����������
		ScreenToClient(RectTemp);
		pWnd[3] = GetDlgItem(IDC_PROGRESS1);
		pWnd[3]->SetWindowPos( NULL,RectTemp.left,RectTemp.bottom+10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//��������ֻ�ı����꣬���ı��С
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetRange(0,100);//��������ֵ��Χ0~100
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetStep(1);//����������1
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetPos(0);//��ǰ����0

		//��Ч��������AfxGetApp()->LoadIcon(IDI_ICON1) ����icon
		//((CMFCButton *)GetDlgItem(IDC_BUTTON_OpenFile)) ��ȡ��ť���
		//SetIcon ��ť��������

		//mImage.Draw(GetDC()->GetSafeHdc(),CRect(0,0,WinDlgWidth,WinDlgHeight));//���������������ַ������ƣ�����˸

		{	//��������
			CBitmap	bmpBackground;		//����һ��λͼ���
			FormatChange FC;
			FC.CImage2CBitmap(mImage,bmpBackground);
			//bmpBackground.LoadBitmap(IDB_BITMAP1);   //����ͼƬ��λͼ���	

			CRect   WinDlg;   
			GetClientRect(&WinDlg);			//��ȡ����Ĵ�С

			CDC *BGPDCMem = new CDC;;		//�����ڴ�ͼƬCDC
			CPaintDC WinDlgDc(this);					//��δ����������ñ���ͼ����ʼ��DC���ƶ���Ϊ���屾��
			BGPDCMem->CreateCompatibleDC(&WinDlgDc);   //����һ������ʾ���豸���ݼ��ݵ��ڴ��豸����
			BGPDCMem->SetBkMode(TRANSPARENT);
			BGPDCMem->SelectObject(&bmpBackground); //��SelectObject��λͼѡ���ڴ��豸����  
#if 0	//���췽ʽ����
			BITMAP   bitmap;	//BITMAP�ṹ���ڴ��λͼ��Ϣ
			bmpBackground.GetBitmap(&bitmap);	//��ͼƬ�л�ȡͼƬ�Ŀ��ߵ�bitmap
			WinDlgDc.StretchBlt(0,0,WinDlg.Width(),WinDlg.Height(),&g_BGPDCMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY); //��DC�ڵ�ͼƬ��������֮��PO������,Stretch:����
#else
			WinDlgDc.BitBlt(0,0,WinDlg.Width(),WinDlg.Height(),BGPDCMem,0,0,SRCCOPY);	//���ڴ��ͼƬpo��������
#endif
			delete BGPDCMem;
		}
	}



}

void CBaseVersionDlg::OnPaint()
{
	this->DlgPaintInit();
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	this->Printf((CString)("[Dialog]���»������!\r\n"));
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBaseVersionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�������� 
BOOL CBaseVersionDlg::PreTranslateMessage(MSG* pMsg)  
{  
    // TODO:  �ڴ�����ר�ô����/����û���  
    if (pMsg->message == WM_KEYDOWN)  
    {  
        switch (pMsg->wParam)  
        {  
        case'I':  
            //if (::GetKeyState(VK_CONTROL) < 0)//�����Shift+X�����  
                //�ĳ�VK_SHIFT  
                MessageBox(_T("Hello"));  
            return TRUE;  
        }  
    }  
    return CDialog::PreTranslateMessage(pMsg);  
}  

HBRUSH CBaseVersionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{ 
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); 
	 switch (pWnd->GetDlgCtrlID()){
		case IDC_EDIT_Debug:
			pDC->SetTextColor(EDIT_PRINT_TEXT_RGB); //����������ɫ
			pDC->SetBkMode(TRANSPARENT);	//�������屳��Ϊ͸������������ֱ�Ӵ���Edit����ɫ
			HBRUSH hbr= CreateSolidBrush(EDIT_PRINT_BG_RGB);// ���ñ���ɫ��ˢ
			return hbr;
		break;
	 }
	// TODO: Change any attributes of the DC here
	if (nCtlColor==CTLCOLOR_STATIC || nCtlColor==CTLCOLOR_EDIT)//�����ǰ�ؼ����ھ�̬�ı�
	{ 
		pDC->SetTextColor(EDIT_PRINT_TEXT_RGB); //����������ɫ
		pDC->SetBkMode(TRANSPARENT);	//�������屳��Ϊ͸������������ֱ�Ӵ���Edit����ɫ
		HBRUSH hbr= CreateSolidBrush(EDIT_PRINT_BG_RGB);// ���ñ���ɫ��ˢ
		return hbr;
	} 
	else if (nCtlColor==CTLCOLOR_BTN) //�����ǰ�ؼ����ڰ�ť
	{ 

	} 
	// TODO: Return a different brush if the default is not desired
	return hbr; 
}


void CBaseVersionDlg::ProcessCtrl(int Num)
{
	((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetPos(Num);//��������ֵ��Χ0~100
}

BOOL CBaseVersionDlg::Printf(CString string){
	static CString DebugCStringAll;
	DebugCStringAll += string;
	SetDlgItemText(IDC_EDIT_Debug,DebugCStringAll);
	CEdit *DebugEdit = (CEdit*)GetDlgItem(IDC_EDIT_Debug);
	DebugEdit->LineScroll(DebugEdit->GetLineCount()-1,0); 
	return 0;
}

void CBaseVersionDlg::OnBnClickedButtonOpenfile()
{
	//   TCHAR szFilter[] = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");  
	//	ע��;������������������,��β��˫||
	//  ע��1|������1.2;������1.2|ע��2|������2||
	TCHAR szFilter[] = _T("����������|*.jpg;*.bmp;*.png||");
    // ������ļ��Ի���   
    CFileDialog fileDlg(TRUE, _T("xls"), NULL, 0, szFilter, this);   

    // ��ʾ���ļ��Ի���   
    if (IDOK == fileDlg.DoModal())   
    {   
        // ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
        g_Path = fileDlg.GetPathName();   
        SetDlgItemText(IDC_EDIT_Path, g_Path);  
		UpdateWindow();
    }		
}


void CBaseVersionDlg::OnBnClickedButtonTest()
{
	Example Ex;
	Ex.FileOperation();
	Ex.TimerTest();
	Ex.ThreadTest();
	Ex.ProcessTest();
	Ex.NetTest();
	Ex.WinCMDTest();
}

void CBaseVersionDlg::OnBnClickedButtonRtspstart()
{
	ffmpeg FFMPEGHdlr;
	FFMPEGHdlr.ffmpeg_init();
	FFMPEGHdlr.ffmpeg_start();
}


void CBaseVersionDlg::OnBnClickedButtonRtspend()
{
	
ffmpeg FFMPEGHdlr;
	FFMPEGHdlr.ffmpeg_end();
}