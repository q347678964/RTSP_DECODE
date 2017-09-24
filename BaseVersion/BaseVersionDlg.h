
// BaseVersionDlg.h : 头文件
//

#pragma once


// CBaseVersionDlg 对话框
class CBaseVersionDlg : public CDialog
{
// 构造
public:
	CBaseVersionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BASEVERSION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BOOL CBaseVersionDlg::PreTranslateMessage(MSG* pMsg);			//重载消息传输函数
	HBRUSH CBaseVersionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);	//重载颜色控制函数
public:
	CString g_Path;

	void CBaseVersionDlg::DlgPaintInit(void);
	BOOL CBaseVersionDlg::Printf(CString string);
	void CBaseVersionDlg::UIOperationCB(int ControlID,int Num);
	void CBaseVersionDlg::UIOperationCB(int ControlID,CString CStringData);

	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonRtspstart();
	afx_msg void OnBnClickedButtonRtspend();
};
