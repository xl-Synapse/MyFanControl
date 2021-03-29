
// MyFanControlDlg.h : 头文件
//

#pragma once

#include "Core.h"
#include "afxcmn.h"
#include "afxwin.h"

//#define MY_DEBUG//调试模式，此模式下，不会以隐藏模式启动


// CMyFanControlDlg 对话框
class CMyFanControlDlg : public CDialogEx
{
// 构造
public:
	CMyFanControlDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMyFanControlDlg();

// 对话框数据
	enum { IDD = IDD_MYFANCONTROL_DIALOG };

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
public:
	BOOL m_bForceHideWindow;//启动时强制隐藏窗口
	CCore m_core;//温度控制内核
	HANDLE m_hCoreThread;//内核线程
	int m_nLastCoreUpdateTime;//内核最后更新时间
	CListCtrl m_ctlStatus;// 状态表控件
	CButton m_ctlTakeOver;
	CButton m_ctlForcedCooling;
	CButton m_ctlLinear;
	CEdit m_ctlInterval;
	CEdit m_ctlTransition;
	CEdit m_ctlForceTemp;//强制冷却至温度
	int m_nDutyEditCtlID[2][10];//转速设置控件的id
	BOOL m_bWindowVisible;//窗口是否显示
	int m_nWindowSize[2];//完整窗口尺寸
	BOOL m_bAdvancedMode;//高级模式

public:
	static DWORD WINAPI CoreThread(LPVOID lParam);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void UpdateGui(BOOL bFull);//更新界面，bFull为是否完整更新，包括自定义转速设置，如果为否，只更新风扇状态
	BOOL CheckAndSave();//检查表单填写是否正确，如果正确则保存返回TRUE，不正确则返回FALSE
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonLoad();
	void SetTray(PCSTR string);//设置托盘，鼠标放置显示的内容，如果传入NULL，则删除图标
	afx_msg LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);//系统托盘事件函数
	afx_msg void OnBnClickedCheckTakeover();
	afx_msg void OnBnClickedCheckForce();
	afx_msg void OnBnClickedCheckLinear();
	void SetAdvancedMode(BOOL bAdvanced);//设置高级模式
	afx_msg void OnBnClickedButtonAdvanced();
	CButton m_ctlAutorun;
	afx_msg void OnBnClickedCheckAutorun();
	BOOL SetAutorunReg(BOOL bWrite = FALSE, BOOL bAutorun = FALSE);//设置自动运行的注册表项，bWrite写入或读取，bAutorun是写入是写入执行还是不执行
	BOOL SetAutorunTask(BOOL bWrite = FALSE, BOOL bAutorun = FALSE);//设置自动运行的计划任务，bWrite写入或读取，bAutorun是写入是写入执行还是不执行
	CString ExecuteCmd(CString str);//创建新进程执行命令行代码
	BOOL CreateTaskXml(PCSTR strXmlPath, PCSTR strTargetPath);//创建计划任务所需的模板XML文件
	afx_msg void OnBnClickedCheckLockGpuFrequancy();
	BOOL CheckInputFrequency(int frequency);//检查输入的频率是否合适
	CEdit m_ctlFrequency;
	CButton m_ctlLockGpuFrequancy;
};
