
// BaseVersionDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CBaseVersionDlg 对话框




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


// CBaseVersionDlg 消息处理程序

BOOL CBaseVersionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	//SetIcon(m_hIcon, TRUE); // Set big icon  
	//SetIcon(m_hIcon, FALSE); // Set small icon; 

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码
	this->Printf((CString)("[Dialog]初始化窗体完成!\r\n"));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
        //这里让窗口保持和背景图一致 
		int WinDlgWidth = mImage.GetWidth();
		int WinDlgHeight = mImage.GetHeight();
		SetWindowPos(NULL,0,0,WinDlgWidth,WinDlgHeight,SWP_NOMOVE);
		pWnd[0] = GetDlgItem(IDC_EDIT_Debug);
		pWnd[0]->SetWindowPos( NULL,10,10,WinDlgWidth/3,WinDlgHeight/3,SWP_NOZORDER);//调试窗口,根据窗体大小来变换
		pWnd[0]->GetWindowRect(RectTemp);//获取目标在屏幕上的坐标，需要转换到窗体坐标
		ScreenToClient(RectTemp);

		pWnd[1] = GetDlgItem(IDC_EDIT_Path);
		pWnd[1]->SetWindowPos( NULL,RectTemp.right+10,10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//路径窗口，只改变坐标，不改变大小
		pWnd[1]->GetWindowRect(RectTemp);//获取目标在屏幕上的坐标，需要转换到窗体坐标
		ScreenToClient(RectTemp);

		pWnd[2] = GetDlgItem(IDC_BUTTON_OpenFile);
		pWnd[2]->SetWindowPos( NULL,RectTemp.right+10,10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//路径打开按钮，只改变坐标，不改变大小
		((CButton *)GetDlgItem(IDC_BUTTON_OpenFile))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1)); 
		pWnd[2]->GetWindowRect(RectTemp);//获取目标在屏幕上的坐标，需要转换到窗体坐标
		ScreenToClient(RectTemp);

		pWnd[3] = GetDlgItem(IDC_BUTTON_Test);
		pWnd[3]->SetWindowPos( NULL,RectTemp.right+10,10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//测试按钮，只改变坐标，不改变大小
		((CButton *)GetDlgItem(IDC_BUTTON_Test))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON1)); 

		pWnd[1]->GetWindowRect(RectTemp);//进度条放在路径窗口下面
		ScreenToClient(RectTemp);
		pWnd[3] = GetDlgItem(IDC_PROGRESS1);
		pWnd[3]->SetWindowPos( NULL,RectTemp.left,RectTemp.bottom+10,0,0,SWP_NOZORDER|SWP_NOSIZE);	//进度条，只改变坐标，不改变大小
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetRange(0,100);//进度条数值范围0~100
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetStep(1);//进度条步进1
		((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetPos(0);//当前进度0

		//等效于这三句AfxGetApp()->LoadIcon(IDI_ICON1) 加载icon
		//((CMFCButton *)GetDlgItem(IDC_BUTTON_OpenFile)) 获取按钮句柄
		//SetIcon 按钮操作函数

		//mImage.Draw(GetDC()->GetSafeHdc(),CRect(0,0,WinDlgWidth,WinDlgHeight));//背景，不能用这种方法绘制，会闪烁

		{	//背景绘制
			CBitmap	bmpBackground;		//声明一个位图句柄
			FormatChange FC;
			FC.CImage2CBitmap(mImage,bmpBackground);
			//bmpBackground.LoadBitmap(IDB_BITMAP1);   //加载图片到位图句柄	

			CRect   WinDlg;   
			GetClientRect(&WinDlg);			//获取窗体的大小

			CDC *BGPDCMem = new CDC;;		//申请内存图片CDC
			CPaintDC WinDlgDc(this);					//这段代码用于设置背景图，初始化DC绘制对象为窗体本身
			BGPDCMem->CreateCompatibleDC(&WinDlgDc);   //创建一个与显示器设备内容兼容的内存设备内容
			BGPDCMem->SetBkMode(TRANSPARENT);
			BGPDCMem->SelectObject(&bmpBackground); //用SelectObject将位图选入内存设备内容  
#if 0	//拉伸方式绘制
			BITMAP   bitmap;	//BITMAP结构用于存放位图信息
			bmpBackground.GetBitmap(&bitmap);	//从图片中获取图片的宽高到bitmap
			WinDlgDc.StretchBlt(0,0,WinDlg.Width(),WinDlg.Height(),&g_BGPDCMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY); //将DC内的图片内容拉伸之后PO窗体上,Stretch:拉伸
#else
			WinDlgDc.BitBlt(0,0,WinDlg.Width(),WinDlg.Height(),BGPDCMem,0,0,SRCCOPY);	//将内存的图片po到窗体上
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
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	this->Printf((CString)("[Dialog]重新绘制完成!\r\n"));
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBaseVersionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//按键监听 
BOOL CBaseVersionDlg::PreTranslateMessage(MSG* pMsg)  
{  
    // TODO:  在此添加专用代码和/或调用基类  
    if (pMsg->message == WM_KEYDOWN)  
    {  
        switch (pMsg->wParam)  
        {  
        case'I':  
            //if (::GetKeyState(VK_CONTROL) < 0)//如果是Shift+X这里就  
                //改成VK_SHIFT  
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
			pDC->SetTextColor(EDIT_PRINT_TEXT_RGB); //设置字体颜色
			pDC->SetBkMode(TRANSPARENT);	//设置字体背景为透明，这样才能直接带看Edit的颜色
			HBRUSH hbr= CreateSolidBrush(EDIT_PRINT_BG_RGB);// 设置背景色画刷
			return hbr;
		break;
	 }
	// TODO: Change any attributes of the DC here
	if (nCtlColor==CTLCOLOR_STATIC || nCtlColor==CTLCOLOR_EDIT)//如果当前控件属于静态文本
	{ 
		pDC->SetTextColor(EDIT_PRINT_TEXT_RGB); //设置字体颜色
		pDC->SetBkMode(TRANSPARENT);	//设置字体背景为透明，这样才能直接带看Edit的颜色
		HBRUSH hbr= CreateSolidBrush(EDIT_PRINT_BG_RGB);// 设置背景色画刷
		return hbr;
	} 
	else if (nCtlColor==CTLCOLOR_BTN) //如果当前控件属于按钮
	{ 

	} 
	// TODO: Return a different brush if the default is not desired
	return hbr; 
}


void CBaseVersionDlg::ProcessCtrl(int Num)
{
	((CProgressCtrl *)GetDlgItem(IDC_PROGRESS1))->SetPos(Num);//进度条数值范围0~100
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
	//   TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");  
	//	注意;代表两个过滤器并行,结尾是双||
	//  注释1|过滤器1.2;过滤器1.2|注释2|过滤器2||
	TCHAR szFilter[] = _T("过滤器名字|*.jpg;*.bmp;*.png||");
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("xls"), NULL, 0, szFilter, this);   

    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())   
    {   
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
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
