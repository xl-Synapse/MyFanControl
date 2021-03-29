
// MyFanControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyFanControl.h"
#include "MyFanControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SHOWTASK (WM_USER +1)
#define IDR_SHOW 11
#define IDR_EXIT 12

__int64 CompareFileTime(FILETIME time1, FILETIME time2)
{
	__int64 a = __int64(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
	__int64 b = __int64(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;

	return (b - a);
}
int GetCpuClock(int *CPU_usage)
{
	LARGE_INTEGER c1;
	QueryPerformanceCounter(&c1);
	LARGE_INTEGER c2;
	QueryPerformanceCounter(&c2);
	unsigned __int64 start = __rdtsc();
	///
	FILETIME preidleTime, prekernelTime, preuserTime;
	BOOL res = GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);
	///
	DWORD startTickCount = GetTickCount();
	while (GetTickCount() - startTickCount < static_cast<DWORD>(100)) {}
	LARGE_INTEGER c3;
	QueryPerformanceCounter(&c3);
	unsigned __int64 end = __rdtsc();
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	//
	FILETIME idleTime, kernelTime, userTime;
	res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
	__int64 idle = CompareFileTime(preidleTime, idleTime);
	__int64 kernel = CompareFileTime(prekernelTime, kernelTime);
	__int64 user = CompareFileTime(preuserTime, userTime);

	int cpu = int(((kernel - idle + user) * 100) / (kernel + user));
	if (CPU_usage)
		*CPU_usage = cpu;
	//
	unsigned __int64 e = (c3.QuadPart - c2.QuadPart) - (c2.QuadPart - c1.QuadPart);
	double elapsed = static_cast<double>(e) / static_cast<double>(freq.QuadPart);
	return int((static_cast<double>((end - start) / elapsed)) / 1000000);
}
////

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


// CMyFanControlDlg 对话框



CMyFanControlDlg::CMyFanControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyFanControlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bForceHideWindow = TRUE;//启动时强制隐藏窗口
#ifdef MY_DEBUG
	m_bForceHideWindow = FALSE;//启动时强制隐藏窗口
#endif
	m_hCoreThread = NULL;
	m_nLastCoreUpdateTime = -1;
	m_bWindowVisible = FALSE;
	m_bAdvancedMode = TRUE;
	m_nWindowSize[0] = 0;
	m_nWindowSize[1] = 0;

	m_nDutyEditCtlID[0][0] = IDC_EDIT_CPU0;
	m_nDutyEditCtlID[0][1] = IDC_EDIT_CPU1;
	m_nDutyEditCtlID[0][2] = IDC_EDIT_CPU2;
	m_nDutyEditCtlID[0][3] = IDC_EDIT_CPU3;
	m_nDutyEditCtlID[0][4] = IDC_EDIT_CPU4;
	m_nDutyEditCtlID[0][5] = IDC_EDIT_CPU5;
	m_nDutyEditCtlID[0][6] = IDC_EDIT_CPU6;
	m_nDutyEditCtlID[0][7] = IDC_EDIT_CPU7;
	m_nDutyEditCtlID[0][8] = IDC_EDIT_CPU8;
	m_nDutyEditCtlID[0][9] = IDC_EDIT_CPU9;
	m_nDutyEditCtlID[1][0] = IDC_EDIT_GPU0;
	m_nDutyEditCtlID[1][1] = IDC_EDIT_GPU1;
	m_nDutyEditCtlID[1][2] = IDC_EDIT_GPU2;
	m_nDutyEditCtlID[1][3] = IDC_EDIT_GPU3;
	m_nDutyEditCtlID[1][4] = IDC_EDIT_GPU4;
	m_nDutyEditCtlID[1][5] = IDC_EDIT_GPU5;
	m_nDutyEditCtlID[1][6] = IDC_EDIT_GPU6;
	m_nDutyEditCtlID[1][7] = IDC_EDIT_GPU7;
	m_nDutyEditCtlID[1][8] = IDC_EDIT_GPU8;
	m_nDutyEditCtlID[1][9] = IDC_EDIT_GPU9;
}

CMyFanControlDlg::~CMyFanControlDlg()
{
	DestroyWindow();
}

void CMyFanControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATUS, m_ctlStatus);
	DDX_Control(pDX, IDC_CHECK_TAKEOVER, m_ctlTakeOver);
	DDX_Control(pDX, IDC_CHECK_FORCE, m_ctlForcedCooling);
	DDX_Control(pDX, IDC_CHECK_LINEAR, m_ctlLinear);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_ctlInterval);
	DDX_Control(pDX, IDC_EDIT_TREANSITION, m_ctlTransition);
	DDX_Control(pDX, IDC_EDIT_FORCE_TEMP, m_ctlForceTemp);
	DDX_Control(pDX, IDC_CHECK_AUTORUN, m_ctlAutorun);
	DDX_Control(pDX, IDC_EDIT_GPU_FREQUENCY, m_ctlFrequency);
	DDX_Control(pDX, IDC_CHECK_LOCK_GPU_FREQUANCY, m_ctlLockGpuFrequancy);
}

BEGIN_MESSAGE_MAP(CMyFanControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CMyFanControlDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CMyFanControlDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CMyFanControlDlg::OnBnClickedButtonLoad)
	ON_MESSAGE(WM_SHOWTASK, &CMyFanControlDlg::OnShowTask)//消息映射
	ON_BN_CLICKED(IDC_CHECK_TAKEOVER, &CMyFanControlDlg::OnBnClickedCheckTakeover)
	ON_BN_CLICKED(IDC_CHECK_FORCE, &CMyFanControlDlg::OnBnClickedCheckForce)
	ON_BN_CLICKED(IDC_CHECK_LINEAR, &CMyFanControlDlg::OnBnClickedCheckLinear)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, &CMyFanControlDlg::OnBnClickedButtonAdvanced)
	ON_BN_CLICKED(IDC_CHECK_AUTORUN, &CMyFanControlDlg::OnBnClickedCheckAutorun)
	ON_BN_CLICKED(IDC_CHECK_LOCK_GPU_FREQUANCY, &CMyFanControlDlg::OnBnClickedCheckLockGpuFrequancy)
END_MESSAGE_MAP()


// CMyFanControlDlg 消息处理程序

BOOL CMyFanControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	//获取窗口完整尺寸
	CRect rect;
	this->GetWindowRect(rect);
	m_nWindowSize[0] = rect.Width();
	m_nWindowSize[1] = rect.Height();
	SetAdvancedMode(FALSE);

	SetTray("蓝天风扇监控");



	if (m_hCoreThread == NULL)
	{
		DWORD dwThreadID = 0;
		m_hCoreThread = CreateThread(NULL, NULL, CoreThread, this, NULL, &dwThreadID);
	}
	SetTimer(0, 100, NULL);
	m_ctlAutorun.SetCheck(SetAutorunReg(FALSE) || SetAutorunTask(FALSE));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMyFanControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyFanControlDlg::OnPaint()
{
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyFanControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD CMyFanControlDlg::CoreThread(LPVOID lParam)
{

	CMyFanControlDlg * pDlg = (CMyFanControlDlg *)lParam;
	pDlg->m_core.Run();

	return 0;
}

void CMyFanControlDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (m_bForceHideWindow)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}


	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO:  在此处添加消息处理程序代码
}


void CMyFanControlDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (!m_core.m_nExit)
		m_core.m_nExit = 1;

	if (m_core.m_nExit == 1)//等待内核线程结束
	{
		int count = 0;
		while (m_core.m_nExit == 1 && count++<100)
		{
			Sleep(100);
		}
	}
	if (m_core.m_nExit)
	{
		KillTimer(0);
		SetTray(NULL);
		CDialogEx::OnOK();
	}
}


void CMyFanControlDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_bWindowVisible)
	{
		this->ShowWindow(SW_HIDE);
	}
	else
	{
		this->ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}
	//CDialogEx::OnCancel();
}


void CMyFanControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	static int nCheckThreadCount = 0;//检查工作线程状态计数器，每100ms+1
	CDialogEx::OnTimer(nIDEvent);
	if (m_core.m_nExit == 2)
	{
		OnOK();
	}

	//检查工作线程是否卡死
	nCheckThreadCount++;
	if (nCheckThreadCount > 150)//内核已经15秒未完成一个循环，认为卡死，结束程序
	{
		KillTimer(0);
		m_core.m_nExit = 2;
		TerminateThread(m_hCoreThread, -1);//强制结束进程
		CloseHandle(m_hCoreThread);
		m_hCoreThread = NULL;
		MessageBox("检测到工作线程卡死，程序将立刻结束，如果重试后问题仍然存在，说明本程序可能不适用于此电脑。");
		OnOK();
	}

	if (m_core.m_nInit != 1)
		return;


	//根据窗口显示状态来刷新
	static BOOL LastVisible = FALSE;
	m_bWindowVisible = IsWindowVisible();
	if (m_bWindowVisible && !LastVisible)
	{
		m_core.m_bUpdateRPM = TRUE;//更新风扇转速
		UpdateGui(TRUE);
	}
	else if (!m_bWindowVisible && LastVisible)
	{
		m_core.m_bUpdateRPM = FALSE;
		//AfxMessageBox("窗口已隐藏");
	}
	LastVisible = m_bWindowVisible;
	// 内核已经更新数据，需要更新界面
	if (m_nLastCoreUpdateTime != m_core.m_nLastUpdateTime)
	{
		if (m_bWindowVisible)
			UpdateGui(FALSE);
		nCheckThreadCount = 0;
		m_nLastCoreUpdateTime = m_core.m_nLastUpdateTime;
	}
	//
}

void CMyFanControlDlg::UpdateGui(BOOL bFull)
{
	int row = m_ctlStatus.GetItemCount();
	int col = m_ctlStatus.GetHeaderCtrl()->GetItemCount();
	if (row != 6 || col != 3)
	{
		if (!bFull)
		{
			UpdateGui(TRUE);//需要完整更新界面
			return;
		}
		//得到控件大小
		CRect rect;
		
		m_ctlStatus.GetWindowRect(rect);
		int width = rect.Width();
		//设置行列数
		m_ctlStatus.DeleteAllItems();//删除所有单元格
		while (m_ctlStatus.DeleteColumn(0));//删除所有列
		int i = 0;
		m_ctlStatus.InsertColumn(i++, "", LVCFMT_CENTER, int(width * 0.32));
		m_ctlStatus.InsertColumn(i++, "CPU", LVCFMT_CENTER, int(width * 0.33));
		m_ctlStatus.InsertColumn(i++, "GPU", LVCFMT_CENTER, int(width * 0.33));
		i = 0;
		m_ctlStatus.InsertItem(i++, "当前温度");
		m_ctlStatus.InsertItem(i++, "设定挡位");
		m_ctlStatus.InsertItem(i++, "转速%");
		//m_ctlStatus.InsertItem(i++, "设定负载");
		m_ctlStatus.InsertItem(i++, "转速RPM");
		m_ctlStatus.InsertItem(i++, "运行频率");
		m_ctlStatus.InsertItem(i++, "使用率%");
	}
	//显示状态信息
	char str[256];
	for (int i = 0; i < 2; i++)
	{
		sprintf_s(str, 256, "%d", m_core.m_nCurTemp[i]);
		m_ctlStatus.SetItemText(0, i + 1, str);
		sprintf_s(str, 256, "%d", m_core.m_nSetDutyLevel[i]);
		m_ctlStatus.SetItemText(1, i + 1, str);
		sprintf_s(str, 256, "%d", m_core.m_nCurDuty[i]);
		m_ctlStatus.SetItemText(2, i + 1, str);
		//sprintf_s(str, 256, "%d", m_core.m_nSetDuty[i]);
		//m_ctlStatus.SetItemText(3, i + 1, str);
		if (m_core.m_nCurRPM[i]>=0)
		{
			sprintf_s(str, 256, "%d", m_core.m_nCurRPM[i]);
			m_ctlStatus.SetItemText(3, i + 1, str);
		}
		else
		{
			m_ctlStatus.SetItemText(3, i + 1, "-");
		}
		if (i == 1)
		{
			sprintf_s(str, 256, "%d/%d", m_core.m_GpuInfo.m_nGraphicsClock, m_core.m_GpuInfo.m_nMemoryClock);
			m_ctlStatus.SetItemText(4, i + 1, str);
			sprintf_s(str, 256, "%d", m_core.m_GpuInfo.m_nUsage);
			m_ctlStatus.SetItemText(5, i + 1, str);
		}
		/*
		else if (i == 0)
		{
			int cpu_usage;
			int CpuClock = GetCpuClock(&cpu_usage);
			sprintf_s(str, 256, "%d", CpuClock);
			m_ctlStatus.SetItemText(4, i + 1, str);
			sprintf_s(str, 256, "%d", cpu_usage);
			m_ctlStatus.SetItemText(5, i + 1, str);
		}*/

	}
	//强制冷却状态
	int fc = m_ctlForcedCooling.GetCheck();
	if (fc ^ m_core.m_bForcedCooling)
	{
		m_ctlForcedCooling.SetCheck(m_core.m_bForcedCooling);
	}
	//////////////////////////////////////////////////////////
	if (!bFull)
		return;
	//接管控制
	int to = m_ctlTakeOver.GetCheck();
	if (to ^ m_core.m_config.TakeOver)
	{
		m_ctlTakeOver.SetCheck(m_core.m_config.TakeOver);
	}
	//线性控制
	int lc = m_ctlLinear.GetCheck();
	if (lc ^ m_core.m_config.Linear)
	{
		m_ctlLinear.SetCheck(m_core.m_config.Linear);
	}
	//GPU限频
	int lf = m_ctlLockGpuFrequancy.GetCheck();
	if (lf ^ m_core.m_config.LockGPUFrequency)
	{
		m_ctlLockGpuFrequancy.SetCheck(m_core.m_config.LockGPUFrequency);
	}
	//更新间隔
	sprintf_s(str, 256, "%d", m_core.m_config.UpdateInterval);
	m_ctlInterval.SetWindowTextA(str);
	//过渡温度
	sprintf_s(str, 256, "%d", m_core.m_config.TransitionTemp);
	m_ctlTransition.SetWindowTextA(str);
	//强制冷却温度
	sprintf_s(str, 256, "%d", m_core.m_config.ForceTemp);
	m_ctlForceTemp.SetWindowTextA(str);
	//GPU频率
	sprintf_s(str, 256, "%d", m_core.m_config.GPUFrequency);
	m_ctlFrequency.SetWindowTextA(str);
	//自定义转速控制
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			sprintf_s(str, 256, "%d", m_core.m_config.DutyList[i][j]);
			GetDlgItem(m_nDutyEditCtlID[i][j])->SetWindowTextA(str);
		}
	}
}

BOOL CMyFanControlDlg::CheckAndSave()
{
	//检查设置
	char str[256];
	m_ctlInterval.GetWindowTextA(str,256);
	int nInterval = atoi(str);
	if (nInterval < 1 || nInterval > 5)
	{
		AfxMessageBox("更新间隔必须为1-5");
		return FALSE;
	}
	//
	m_ctlTransition.GetWindowTextA(str, 256);
	int nTransition = atoi(str);
	if (nTransition < 0 || nTransition > 10)
	{
		AfxMessageBox("过渡温度必须为0-10");
		return FALSE;
	}
	//
	m_ctlForceTemp.GetWindowTextA(str, 256);
	int nForceTemp = atoi(str);
	if (nForceTemp < 40 || nForceTemp > 90)
	{
		AfxMessageBox("强制冷却温度必须为40-90");
		return FALSE;
	}
	//
	m_ctlFrequency.GetWindowTextA(str, 256);
	int nFrequency = atoi(str);
	if (!CheckInputFrequency(nFrequency))
		return FALSE;

	if (nFrequency == 0)
		nFrequency = m_core.m_GpuInfo.m_nStandardFrequency;
	//
	int nDutyList[2][10];
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			GetDlgItem(m_nDutyEditCtlID[i][j])->GetWindowTextA(str, 256);
			nDutyList[i][j] = atoi(str);
			if (nDutyList[i][j]<0 || nDutyList[i][j]>100)
			{
				char str2[256];
				sprintf_s(str2, 256, "%s风扇转速设定错误，必须为0-100",i?"GPU":"CPU");
				AfxMessageBox(str2);
				return FALSE;
			}
		}
	}
	//应用设置
	m_core.m_config.UpdateInterval = nInterval;
	m_core.m_config.TransitionTemp = nTransition;
	m_core.m_config.ForceTemp = nForceTemp;
	m_core.m_config.GPUFrequency = nFrequency;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			m_core.m_config.DutyList[i][j] = nDutyList[i][j];
		}
	}
	//保存
	m_core.m_config.SaveConfig();
	return TRUE;
}

void CMyFanControlDlg::OnBnClickedButtonSave()
{
	// TODO:  在此添加控件通知处理程序代码
	if (CheckAndSave())
		UpdateGui(TRUE);
}


void CMyFanControlDlg::OnBnClickedButtonReset()
{
	// TODO:  在此添加控件通知处理程序代码
	m_core.m_config.LoadDefault();
	UpdateGui(TRUE);
}


void CMyFanControlDlg::OnBnClickedButtonLoad()
{
	// TODO:  在此添加控件通知处理程序代码
	m_core.m_config.LoadConfig();
	UpdateGui(TRUE);
}

void CMyFanControlDlg::SetTray(PCSTR string)//设置托盘图标
{
	static BOOL added = FALSE;
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;//自定义的消息名称  
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	if (string)
	{
		strcpy_s(nid.szTip, 128, string);//信息提示内容  
		if (!added)
		{
			Shell_NotifyIcon(NIM_ADD, &nid);//在托盘区添加图标
			added = TRUE;
		}
		else
		{
			Shell_NotifyIcon(NIM_MODIFY, &nid);//在托盘区添加图标
		}
	}
	else
		Shell_NotifyIcon(NIM_DELETE, &nid);
}



LRESULT CMyFanControlDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{//wParam接收的是图标的ID，而lParam接收的是鼠标的行为 
	if (wParam != IDR_MAINFRAME)
		return 1;
	switch (lParam)
	{
	case WM_LBUTTONUP://左键单击显示主界面
	{
						  
	}break;
	case WM_RBUTTONUP://右击弹出菜单
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);//得到鼠标位置
		CMenu menu;
		menu.CreatePopupMenu();
		if (m_bWindowVisible)
			menu.AppendMenu(MFT_STRING, IDR_SHOW, "隐藏");
		else
			menu.AppendMenu(MFT_STRING, IDR_SHOW, "显示");
		menu.AppendMenu(MFT_SEPARATOR);
		menu.AppendMenu(MFT_STRING, IDR_EXIT, "退出");
		SetForegroundWindow();//不加此行在菜单外点击菜单不销毁
		int xx = TrackPopupMenu(menu, TPM_RETURNCMD, lpoint->x, lpoint->y, NULL, this->m_hWnd, NULL);//显示菜单并获取选项ID
		if (xx == IDR_SHOW)
		{
			OnCancel();

		}
		else if (xx == IDR_EXIT) 
		{
			OnOK(); 
		}
		HMENU hmenu = menu.Detach();
		menu.DestroyMenu();
		delete lpoint;
	}break;
	case WM_LBUTTONDBLCLK:
	{
		this->ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}break;
	case WM_MOUSEMOVE:
	{
		static int LastUpdate = -1;
		if (LastUpdate != m_core.m_nLastUpdateTime)
		{
			char str[128];
			sprintf_s(str, 128, "CPU：%d℃，%d%%\nGPU：%d℃，%d%%", m_core.m_nCurTemp[0], m_core.m_nCurDuty[0], m_core.m_nCurTemp[1], m_core.m_nCurDuty[1]);
			SetTray(str);
			LastUpdate = m_core.m_nLastUpdateTime;
		}

	}break;
	}
	return 0;
}

void CMyFanControlDlg::OnBnClickedCheckTakeover()
{
	// TODO:  在此添加控件通知处理程序代码
	int val = m_ctlTakeOver.GetCheck();
	m_core.m_config.TakeOver = val;
}


void CMyFanControlDlg::OnBnClickedCheckForce()
{
	// TODO:  在此添加控件通知处理程序代码
	int val = m_ctlForcedCooling.GetCheck();
	m_core.m_bForcedCooling = val;
}


void CMyFanControlDlg::OnBnClickedCheckLinear()
{
	// TODO:  在此添加控件通知处理程序代码
	int val = m_ctlLinear.GetCheck();
	m_core.m_config.Linear = val;
}

void CMyFanControlDlg::SetAdvancedMode(BOOL bAdvanced)
{
	CRect rect;
	this->GetWindowRect(rect);
	if (bAdvanced)
	{
		MoveWindow(rect.left, rect.top,  m_nWindowSize[0], m_nWindowSize[1], TRUE);
		GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowTextA("简单模式");
	}
	else
	{
		MoveWindow(rect.left, rect.top, m_nWindowSize[0] * 335 / 582, m_nWindowSize[1] * 283 / 463, FALSE);
		GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowTextA("高级模式");
	}
	m_bAdvancedMode = !m_bAdvancedMode;
}

void CMyFanControlDlg::OnBnClickedButtonAdvanced()
{
	// TODO:  在此添加控件通知处理程序代码
	SetAdvancedMode(!m_bAdvancedMode);
}


void CMyFanControlDlg::OnBnClickedCheckAutorun()
{
	int val = m_ctlAutorun.GetCheck();
	int set_rv;
	if (val)
	{
		int rv = MessageBox("请选择开机自动启动方式：\r\n\r\n按\"是\"：注册表启动项自启动\r\n按\"否\"：任务计划自启动（管理员权限）\r\n按\"取消\"：放弃操作", "开机自动启动", MB_YESNOCANCEL);
		
		if (IDYES == rv)
		{
			set_rv = SetAutorunReg(TRUE, TRUE);
			set_rv = SetAutorunReg(FALSE);
		}
		else if (IDNO == rv)
		{
			set_rv = SetAutorunTask(TRUE, TRUE);
			set_rv = SetAutorunTask(FALSE);
		}
		else
		{
			set_rv = FALSE;
		}
		m_ctlAutorun.SetCheck(set_rv);
	}
	else
	{
		if (SetAutorunReg(FALSE))
			set_rv = SetAutorunReg(TRUE, FALSE);
		if (SetAutorunTask(FALSE))
			set_rv = SetAutorunTask(TRUE, FALSE);
		m_ctlAutorun.SetCheck(SetAutorunReg(FALSE) || SetAutorunTask(FALSE));
	}
	return;


}

BOOL CMyFanControlDlg::SetAutorunReg(BOOL bWrite, BOOL bAutorun)
{
	HKEY hKey;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) != ERROR_SUCCESS)
	{
		AfxMessageBox("无法打开注册表");
		return FALSE;
	}
	PCSTR strProduct = "LanTianFanMonitor";

	if (bWrite)
	{
		if (bAutorun)
		{
			CString			strPath = GetExePath() + "\\MyFanControl.exe";
			unsigned long	nSize = 0;


			nSize = strPath.GetLength();
			if (RegSetValueEx(hKey, strProduct, 0, REG_SZ,
				(unsigned char *)strPath.GetBuffer(strPath.GetLength()), nSize) != ERROR_SUCCESS)
			{
				AfxMessageBox("无法写入注册表启动项，需要用管理员权限运行");
				RegCloseKey(hKey);
				return FALSE;
			}
		}
		else
		{
			if (RegDeleteValue(hKey, strProduct) != ERROR_SUCCESS)
			{
				AfxMessageBox("无法删除注册表启动项，需要用管理员权限运行");
				RegCloseKey(hKey);
				return FALSE;
			}
		}

		RegCloseKey(hKey);
	}
	else
	{
		//检查注册表项
		BOOL		  bRet = FALSE;
		unsigned long lSize = sizeof(bRet);
		if (RegQueryValueEx(hKey, strProduct, NULL, NULL, NULL, &lSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		RegCloseKey(hKey);
		return lSize > 0 ? TRUE : FALSE;
	}

	return TRUE;
}

BOOL CMyFanControlDlg::SetAutorunTask(BOOL bWrite, BOOL bAutorun)
{
	CString strTaskName = "蓝天风扇监控";
	CString strPath = GetExePath() + "\\MyFanControl.exe";
	CString strcmd;
	CString strXmlPath = GetExePath() + "\\task.xml";
	if (bWrite)
	{
		if (bAutorun)
		{
			BOOL  rv = CMyFanControlDlg::CreateTaskXml(strXmlPath, strPath);
			if (!rv)
			{
				AfxMessageBox("无法创建任务计划程序xml文件");
				return FALSE;
			}
			strcmd.Format("SCHTASKS /Create /F /XML %s /TN %s", strXmlPath, strTaskName);
			
		}
		else
		{
			strcmd = "SCHTASKS /Delete /F /TN " + strTaskName;
		}
	}
	else
	{
		strcmd = "SCHTASKS /Query /TN " + strTaskName;
	}
	CString rs = ExecuteCmd(strcmd);
	if (bWrite && bAutorun)
		remove(strXmlPath);
	if (rs == "[执行失败]" || rs.Find("拒绝访问") >= 0)
	{
		CString str;
		str.Format("无法%s任务计划程序，需要用管理员权限运行", bWrite ? (bAutorun ? "创建" : "删除") : ("读取"));
		AfxMessageBox(str);
		return FALSE;
	}
	PCTSTR strFind = bWrite ? (bAutorun ? "成功创建" : "成功删除") : strTaskName;
	if (rs.Find(strFind)>=0)
		return TRUE;
	return FALSE;
}

CString CMyFanControlDlg::ExecuteCmd(CString str)
{
	SECURITY_ATTRIBUTES sa;
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
#ifdef MY_DEBUG
		AfxMessageBox("无法创建管道");
#endif
		return "[执行失败]";
	}
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	char cmdline[1024];
	strcpy_s(cmdline, 1024, str);
	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
	{
#ifdef MY_DEBUG
		AfxMessageBox("无法创建命令行进程");
#endif
		return "[执行失败]";
	}
	CloseHandle(hWrite);

	char buffer[4096]="";
	memset(buffer, 0, 4096);
	CString output;
	DWORD byteRead;
	int i = 0;
	while (true)
	{
		Sleep(100);
		if (ReadFile(hRead, buffer, 4095, &byteRead, NULL) == NULL)
			break;
		if (byteRead)
			output += buffer;
		if (i++ >= 50)
			break;
	}
	return output;
}

BOOL CMyFanControlDlg::CreateTaskXml(PCSTR strXmlPath, PCSTR strTargetPath)
{
	//运行权限
	//最高权限<RunLevel>HighestAvailable</RunLevel>\r\n
	//普通权限<RunLevel>LeastPrivilege</RunLevel>\r\n
	//用户组
	//SYSTEM<UserId>S-1-5-18</UserId>\r\n
	//USERS<GroupId>S-1-5-32-545</GroupId>\r\n
	PCSTR XmlStr = "\
<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n\
<Task version=\"1.2\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\r\n\
  <RegistrationInfo>\r\n\
    <Date>2018-11-16T16:16:29</Date>\r\n\
    <Author>HQ</Author>\r\n\
    <URI>\\蓝天风扇监控</URI>\r\n\
  </RegistrationInfo>\r\n\
  <Triggers>\r\n\
    <LogonTrigger>\r\n\
      <Enabled>true</Enabled>\r\n\
    </LogonTrigger>\r\n\
  </Triggers>\r\n\
  <Principals>\r\n\
    <Principal id=\"Author\">\r\n\
      <GroupId>S-1-5-32-545</GroupId>\r\n\
      <RunLevel>HighestAvailable</RunLevel>\r\n\
    </Principal>\r\n\
  </Principals>\r\n\
  <Settings>\r\n\
    <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\r\n\
    <DisallowStartIfOnBatteries>false</DisallowStartIfOnBatteries>\r\n\
    <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\r\n\
    <AllowHardTerminate>false</AllowHardTerminate>\r\n\
    <StartWhenAvailable>false</StartWhenAvailable>\r\n\
    <RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\r\n\
    <IdleSettings>\r\n\
      <StopOnIdleEnd>false</StopOnIdleEnd>\r\n\
      <RestartOnIdle>false</RestartOnIdle>\r\n\
    </IdleSettings>\r\n\
    <AllowStartOnDemand>true</AllowStartOnDemand>\r\n\
    <Enabled>true</Enabled>\r\n\
    <Hidden>false</Hidden>\r\n\
    <RunOnlyIfIdle>false</RunOnlyIfIdle>\r\n\
    <WakeToRun>false</WakeToRun>\r\n\
    <ExecutionTimeLimit>PT0S</ExecutionTimeLimit>\r\n\
    <Priority>7</Priority>\r\n\
  </Settings>\r\n\
  <Actions Context=\"Author\">\r\n\
    <Exec>\r\n\
      <Command>%s</Command>\r\n\
    </Exec>\r\n\
  </Actions>\r\n\
</Task>\r\n";
	char str[10240];
	sprintf_s(str, 10240, XmlStr, strTargetPath);
	FILE *fp;
	fp = fopen(strXmlPath, "wt");
	if (!fp)
		return FALSE;
	fwrite(str, strlen(str), 1,fp);

	fclose(fp);
	return TRUE;
}

void CMyFanControlDlg::OnBnClickedCheckLockGpuFrequancy()
{
	// TODO:  在此添加控件通知处理程序代码
	int val = m_ctlLockGpuFrequancy.GetCheck();
	if (val)
	{
		char str[256];
		m_ctlFrequency.GetWindowTextA(str, 256);
		int nFrequency = atoi(str);
		if (!CheckInputFrequency(nFrequency))
			m_ctlLockGpuFrequancy.SetCheck(FALSE);
	}

	m_core.m_config.LockGPUFrequency = m_ctlLockGpuFrequancy.GetCheck();
}

BOOL CMyFanControlDlg::CheckInputFrequency(int nFrequency)
{
	char str[256];
	if (nFrequency < 0 || nFrequency > m_core.m_GpuInfo.m_nMaxFrequency)
	{
		char str2[256];
		sprintf_s(str2, 256, "GPU频率限制必须为0-%d，默认频率为%d，0为默认频率", m_core.m_GpuInfo.m_nMaxFrequency, m_core.m_GpuInfo.m_nStandardFrequency);
		AfxMessageBox(str2);
		return FALSE;
	}
	else
	{
		if (nFrequency > m_core.m_GpuInfo.m_nStandardFrequency)
		{
			char str2[256];
			sprintf_s(str2, 256, "GPU默认频率为%d，超频会降低系统稳定性，并会增加发热量。\n注意：由于功率限制，可能无法达到设定的频率。\n是否确认要超频？", m_core.m_GpuInfo.m_nStandardFrequency);
			int rv = MessageBox(str2, "确认要超频？", MB_YESNO);

			if (IDYES == rv)
			{
			}
			else if (IDNO == rv)
			{
				return FALSE;
			}
		}
		else if (nFrequency == 0)
		{
			nFrequency = m_core.m_GpuInfo.m_nStandardFrequency;
			m_core.m_config.GPUFrequency = m_core.m_GpuInfo.m_nStandardFrequency;
			sprintf_s(str, 256, "%d", m_core.m_config.GPUFrequency);
			m_ctlFrequency.SetWindowTextA(str);
		}
	}


	return TRUE;
}
