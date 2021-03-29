#include "stdafx.h"
#include "Core.h"


int TEMP_LIST[10] = { 90, 85, 80, 75, 70, 65, 60, 55, 50, 45 };


int GetTime(tm *pt, int offset)
{
	tm t = { 0 };
	time_t tt;
	if (!pt)
		pt = &t;
	time(&tt);
	tt += offset;
	localtime_s(pt, &tt);
	return (pt->tm_hour * 10000 + pt->tm_min * 100 + pt->tm_sec);
}
int GetTimeInterval(int a, int b, int *p)
{
	//时间差，输入两个6位数时间，如开盘时间91500，得到a-b，并转化为6位数时间，指针p接受以秒计的时间差
	int a1 = a / 10000;
	int a2 = (a % 10000) / 100;
	int a3 = a % 100;

	int b1 = b / 10000;
	int b2 = (b % 10000) / 100;
	int b3 = b % 100;

	int c = (a1 - b1) * 3600 + (a2 - b2) * 60 + a3 - b3;
	if (p)
		*p = c;
	int sgn = (c >= 0) ? 1 : -1;
	c = abs(c);

	int d = (c / 3600) * 10000 + (c % 3600) / 60 * 100 + c % 60;
	return d*sgn;
}
CString GetExePath(){
	char pathbuf[1024] = { 0 };
	int pathlen = ::GetModuleFileName(NULL, pathbuf, 1024);

	while (TRUE)
	{
		if (pathbuf[pathlen--] == '\\')
		{
			break;
		}
	}
	pathbuf[++pathlen] = 0x0;
	CString fname = pathbuf;
	return fname;
}


CGPUInfo::CGPUInfo()
{
	TRACE0("开始加载NVGPU_DLL.dll。\n");
	m_hGPUdll = NULL;
	CString dllpth = GetExePath() + "\\NVGPU_DLL.dll";
	m_hGPUdll = LoadLibrary(dllpth);
	if (m_hGPUdll == NULL)
	{
		TRACE0("无法加载" + dllpth+"\n");
		return;
	}

	m_pfnInitGPU_API = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "InitGPU_API");
	m_pfnSet_GPU_Number = (In_1_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Set_GPU_Number");
	m_pfnGet_GPU_Base_Clock = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Base_Clock");
	m_pfnGet_GPU_Boost_Clock = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Boost_Clock");
	m_pfnCheck_GPU_VRAM_Clock = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Check_GPU_VRAM_Clock");
	m_pfnGet_GPU_Graphics_Clock = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Graphics_Clock");
	m_pfnGet_GPU_Memory_Clock = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Memory_Clock");
	m_pfnGet_Memory_OC_max = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_Memory_OC_max");
	m_pfnGet_GPU_Util = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Util");
	m_pfnGet_GPU_name = (In_0_Out_s_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_name");
	m_pfnGet_GPU_TotalNumber = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_TotalNumber");
	m_pfnGet_GPU_Overclock_range = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Overclock_range");
	m_pfnGet_Memory_range = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_Memory_range");
	m_pfnGet_GPU_Overclock_rangeMax = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Overclock_rangeMax");
	m_pfnGet_GPU_Overclock_rangeMin = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_GPU_Overclock_rangeMin");
	m_pfnGet_Memory_range_max = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_Memory_range_max");
	m_pfnGet_Memory_range_min = (In_0_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_Memory_range_min");
	m_pfnGet_NVDeviceID = (In_1_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Get_NVDeviceID");
	m_pfnLock_Frequency = (In_2_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Lock_Frequency");
	m_pfnLock_Frequency_MEM = (In_2_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Lock_Frequency_MEM");
	m_pfnSet_CoreOC = (In_2_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Set_CoreOC");
	m_pfnSet_MEMOC = (In_2_Out_n_Func *)::GetProcAddress(m_hGPUdll, "Set_MEMOC");
	m_pfnCloseGPU_API = (In_0_Out_0_Func *)::GetProcAddress(m_hGPUdll, "CloseGPU_API");

	//
	if (m_pfnInitGPU_API())
	{
		TRACE0("InitGPU_API初始化失败。\n");
		FreeLibrary(m_hGPUdll);
		m_hGPUdll = NULL;
		return;
	}
	m_pfnSet_GPU_Number(0);
	m_nBaseClock = m_pfnGet_GPU_Base_Clock();
	m_nBoostClock = m_pfnGet_GPU_Boost_Clock();
	m_sName = m_pfnGet_GPU_name();
	m_nDeviceID = m_pfnGet_NVDeviceID(0);
	m_nGraphicsRangeMax = m_pfnGet_GPU_Overclock_rangeMax();
	m_nGraphicsRangeMin = m_pfnGet_GPU_Overclock_rangeMin();
	m_nMemoryRangeMax = m_pfnGet_Memory_range_max();
	m_nMemoryRangeMin = m_pfnGet_Memory_range_min();

	m_nStandardFrequency = m_nBoostClock - m_nGraphicsRangeMin;
	m_nMaxFrequency = m_nStandardFrequency + m_nGraphicsRangeMax;
	m_nLockClock = -1;

	Update();

	TRACE0("成功加载NVGPU_DLL.dll。\n");
}
CGPUInfo::~CGPUInfo()
{
	if (m_hGPUdll != NULL)
	{
		LockFrequency();//还原GPU频率设置
		m_pfnCloseGPU_API();
		FreeLibrary(m_hGPUdll);
		m_hGPUdll = NULL;
	}
}
BOOL CGPUInfo::Update()
{
	if (!m_hGPUdll)
		return FALSE;
	if (!m_pfnCheck_GPU_VRAM_Clock())
		return FALSE;
	m_nGraphicsClock = m_pfnGet_GPU_Graphics_Clock();
	m_nMemoryClock = m_pfnGet_GPU_Memory_Clock();
	m_nUsage = m_pfnGet_GPU_Util();
	return TRUE;
}
BOOL CGPUInfo::LockFrequency(int frequency)
{
	if (!m_hGPUdll)
		return FALSE;
	if (frequency < 0 || frequency > m_nMaxFrequency)
		return FALSE;
	if (frequency == 0)
		frequency = m_nStandardFrequency;
	if (m_nLockClock == frequency)
		return TRUE;
	m_nLockClock = frequency;

	int GpuOverclock = 0;
	int MemOverclock = 0;
	int GpuClock = 0;
	//int MemClock = 0;
	if (frequency > 0 && frequency < m_nStandardFrequency)
	{
		//降频
		GpuClock = frequency;
	}
	else if (frequency > m_nStandardFrequency)
	{
		//超频
		GpuOverclock = frequency - m_nStandardFrequency;
		MemOverclock = GpuOverclock * m_nMemoryRangeMax / m_nGraphicsRangeMax;//按照比例进行显存超频
	}

	//
	int rv1 = (m_pfnSet_CoreOC(0, GpuOverclock) == 0);
	if (!rv1)
		AfxMessageBox("Set_CoreOC失败");
	//
	int rv2 = (m_pfnSet_MEMOC(0, MemOverclock) == 0);
	if (!rv2)
		AfxMessageBox("Set_MEMOC失败");
	//
	int rv3 = (m_pfnLock_Frequency(0, GpuClock) == 0x19);
	if (!rv3)
		AfxMessageBox("Lock_Frequency失败");
	//
	int rv4 = 1;
	//int rv4 = (m_pfnLock_Frequency_MEM(0, MemClock) == 0x19);
	if (!rv4)
		AfxMessageBox("Lock_Frequency_MEM失败");
	//
	return (rv1 && rv2 && rv3 && rv4);
}

CConfig::CConfig()
{
	LoadDefault();
}
void CConfig::LoadDefault()
{
	int i = 0;
	DutyList[0][i++] = 95;//90+
	DutyList[0][i++] = 80;//85+
	DutyList[0][i++] = 70;//80+
	DutyList[0][i++] = 55;//75+
	DutyList[0][i++] = 35;//70+
	DutyList[0][i++] = 30;//65+
	DutyList[0][i++] = 25;//60+
	DutyList[0][i++] = 18;//55+
	DutyList[0][i++] = 18;//50+
	DutyList[0][i++] = 18;//50-
	for (int i = 0; i < 10; i++)
		DutyList[1][i] = DutyList[0][i];

	TransitionTemp = 3;
	UpdateInterval = 2;
	Linear = FALSE;
	TakeOver = FALSE;
	ForceTemp = 50;
	LockGPUFrequency = FALSE;
	GPUFrequency = 0;
}
void CConfig::LoadConfig()
{
	CString strPath = GetExePath() + "\\MyFanControl.cfg";
	CFile file;
	if (!file.Open(strPath, CFile::modeRead | CFile::shareDenyNone))
	{
		SaveConfig();
		if (!file.Open(strPath, CFile::modeRead | CFile::shareDenyNone))
		{
			AfxMessageBox("无法载入配置文件");
			return;
		}
	}
	if (file.GetLength() != sizeof(*this))
	{
		file.Close();
		SaveConfig();
		if (!file.Open(strPath, CFile::modeRead | CFile::shareDenyNone))
		{
			AfxMessageBox("重置后仍然无法载入配置文件");
			return;
		}
		if (file.GetLength() != sizeof(*this))
		{
			AfxMessageBox("配置文件格式不正确");
			file.Close();
			return;
		}
	}
	file.Read(this, sizeof(*this));
	file.Close();
}
void CConfig::SaveConfig()
{
	FILE *fp = NULL;
	CString strPath = GetExePath() + "\\MyFanControl.cfg";
	fp = fopen(strPath, "wb");
	if (fp == NULL)
	{
		AfxMessageBox("无法保存配置文件");
		return;
	}
	fwrite(this, sizeof(*this), 1, fp);
	fclose(fp);
}

//////////////////////////////////////////////////

CCore::CCore()
{
	m_pfnInitIo = NULL;
	m_pfnSetFanDuty = NULL;
	m_pfnSetFANDutyAuto = NULL;
	m_pfnGetTempFanDuty = NULL;
	m_pfnGetFANCounter = NULL;
	m_pfnGetECVersion = NULL;
	m_pfnGetFANRPM[0] = NULL;
	m_pfnGetFANRPM[1] = NULL;
	//
	m_nInit = 0;
	m_nExit = 0;
	m_hInstDLL = NULL;
	for (int i = 0; i < 2; i++)
	{
		m_nCurTemp[i]=0;//当前温度
		m_nLastTemp[i]=0;//上一次温度
		m_nSetDuty[i]=0;//设置的负载
		m_nSetDutyLevel[i] = 0;//设置的转速挡位，最低速档为1，最高速档为10
		m_nCurDuty[i]=0;//当前负载
		m_nCurRPM[i]=0;//当前转速
	}
	m_bUpdateRPM=0;//是否更新转速，如果为0，只更新风扇温度和负载
	m_nLastUpdateTime = GetTime(0, -5);
	m_bForcedCooling = FALSE;
	m_bTakeOverStatus = FALSE;
	m_bForcedRefresh = FALSE;
}
CCore::~CCore()
{
	Uninit();
}

BOOL CCore::Init()
{


	if (m_hInstDLL)
	{
		return TRUE;
	}

	TRACE0("内核开始初始化。\n");
	m_nInit = -1;
	//
	CString dllpth = GetExePath() + "\\ClevoEcInfo.dll";

	m_hInstDLL = LoadLibrary(dllpth);
	if (m_hInstDLL == NULL)
	{
		AfxMessageBox("无法加载" + dllpth + "，请确保该文件在程序目录下，并且已安装NTPortDrv。");
		return FALSE;
	}

	m_pfnInitIo = (InitIo *)::GetProcAddress(m_hInstDLL, "InitIo");
	m_pfnSetFanDuty = (::SetFanDuty *)::GetProcAddress(m_hInstDLL, "SetFanDuty");
	m_pfnSetFANDutyAuto = (SetFANDutyAuto *)::GetProcAddress(m_hInstDLL, "SetFanDutyAuto");
	m_pfnGetTempFanDuty = (GetTempFanDuty *)::GetProcAddress(m_hInstDLL, "GetTempFanDuty");
	m_pfnGetFANCounter = (GetFANCounter *)::GetProcAddress(m_hInstDLL, "GetFanCount");

	m_pfnGetECVersion = (GetECVersion *)::GetProcAddress(m_hInstDLL, "GetECVersion");
	m_pfnGetFANRPM[0] = (GetFanRpm *)::GetProcAddress(m_hInstDLL, "GetCpuFanRpm");
	m_pfnGetFANRPM[1] = (GetFanRpm *)::GetProcAddress(m_hInstDLL, "GetGpuFanRpm");
	//m_pfnGetFANRPM[2] = (GetFanRpm *)::GetProcAddress(m_hInstDLL, "GetGpu1FanRpm");
	//m_pfnGetFANRPM[3] = (GetFanRpm *)::GetProcAddress(m_hInstDLL, "GetX72FanRpm");

	if (m_pfnInitIo == NULL)
	{
		FreeLibrary(m_hInstDLL);
		m_hInstDLL = NULL;
		AfxMessageBox("错误的ClevoEcInfo.dll");
		return FALSE;
	}

	if (m_pfnInitIo() != 1)
	{
		FreeLibrary(m_hInstDLL);
		m_hInstDLL = NULL;
		AfxMessageBox("接口初始化返回值错误！");
		return FALSE;
	}

	/*
	int aa = 0, bb = 0;
	const char *str = m_pfnGetECVersion();
	int a = m_pfnGetCPUFANRPM();
	int b = m_pfnGetGPUFANRPM();
	ECData data1 = m_pfnGetTempFanDuty(1);
	ECData data2 = m_pfnGetTempFanDuty(2);
	m_pfnSetFanDuty(1, 128);
	m_pfnSetFANDutyAuto(1);
	m_pfnSetFanDuty(2, 128);
	m_pfnSetFANDutyAuto(2);
	*/
	//
	TRACE0("内核初始化成功。\n");
	m_nInit = 1;
	return TRUE;
}
void CCore::Uninit()
{
	ResetFan();
	if (m_hInstDLL != NULL)
	{
		FreeLibrary(m_hInstDLL);
		m_hInstDLL = NULL;
	}
	m_nInit = 0;
}
void CCore::Run()
{
	static int nNextChecktTime = 0;
	static BOOL bSetPriority = FALSE;
	m_config.LoadConfig();
	//m_nInit = 2;
	//Sleep(3000);
	if (!m_nInit)
		Init();

	if (m_nInit == 1)
	{
		TRACE0("内核开始运行。\n");
		int curtime;
		while (!m_nExit)
		{
			curtime = GetTime();
			if (curtime >= nNextChecktTime || m_bForcedRefresh)
			{
				//MessageBox(NULL , "工作中...", "MyFunColtrol" , 0);
				Work();
				m_nLastUpdateTime = curtime;//更新时间
				nNextChecktTime = GetTime(NULL, m_config.UpdateInterval);//下一个更新时间
				m_bForcedRefresh = FALSE;
				if (!bSetPriority)
				{
					bSetPriority = TRUE;
					SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);//在首次更新成功后才设置高优先级
				}
			}
			Sleep(100);
		}
		TRACE0("内核结束运行。\n");
	}
	m_nExit = 2;
}
void CCore::Work()
{
	Update();
	if (m_bForcedCooling)//强制冷却
	{
		if (m_nCurTemp[0] >= m_config.ForceTemp || m_nCurTemp[1] >= m_config.ForceTemp)
		{
			if (m_nSetDuty[0] < 95 || m_nSetDuty[1] < 95)
			{
				m_nSetDuty[0] = 95;
				m_nSetDutyLevel[0] = 10;
				m_nSetDuty[1] = 95;
				m_nSetDutyLevel[1] = 10;
				SetFanDuty();
			}
			return;
		}
		else
			m_bForcedCooling = FALSE;
	}
	if (m_config.TakeOver)
	{
		Control();
	}
	else
		ResetFan();

	//锁定GPU频率
	if (m_config.LockGPUFrequency)
		m_GpuInfo.LockFrequency(m_config.GPUFrequency);
	else
		m_GpuInfo.LockFrequency(0);
}
void CCore::Update()
{
	ECData data;
	int TempErr = 0;
	for (int i = 0; i < 2; i++)
	{
		data = m_pfnGetTempFanDuty(i+1);
		if (abs(data.Remote - this->m_nCurTemp[i]) > 30)
		{
			//AfxMessageBox("获取温度有误");
			//温度获取可能有误，重试一次
			if (TempErr++ == 0)
			{
				Sleep(1000);
				i--;
				continue;//重试一次
			}
		}
		this->m_nLastTemp[i] = this->m_nCurTemp[i];
		this->m_nCurTemp[i] = data.Remote;
		this->m_nCurDuty[i] = int(data.FanDuty * 100 / 255.0 + 0.5);

		if (m_bUpdateRPM)//获取风扇转速
		{
			int val = m_pfnGetFANRPM[i]();
			if (val == 0)
				this->m_nCurRPM[i] = 0;
			if (val > 300 && val < 5000)
				this->m_nCurRPM[i] = 2100000 / val;
		}
		else
		{
			this->m_nCurRPM[i] = -1;
		}
		TempErr = 0;
	}
	if (m_bUpdateRPM)
		m_GpuInfo.Update();
}
void CCore::Control()
{
	if (m_config.Linear)
		CalcLinearDuty();
	else
		CalcStdDuty();
	//设定转速
	SetFanDuty();

}
void CCore::CalcLinearDuty()
{
	static int nLastTemp[2] = { 0, 0 };//每次用于计算转速的温度

	int duty,dl;
	int j;
	for (int i = 0; i < 2; i++)
	{
		nLastTemp[i] = max(nLastTemp[i], m_nCurTemp[i]);//温度上升时立刻以当前温度计算转速
		nLastTemp[i] = min(nLastTemp[i], m_nCurTemp[i] + m_config.TransitionTemp);//温度下降时以当前温度+过渡温度来计算转速

		j = nLastTemp[i];//计算转速使用的温度

		if (j < 45)
		{
			duty = m_config.DutyList[i][9];
			dl = 0;
		}
		else if (j >= 90)
		{
			duty = m_config.DutyList[i][0];
			dl = 10;
		}
		else
		{
			int idx = 0;
			if (j < 50)
				idx = 8;
			else if (j < 55)
				idx = 7;
			else if (j < 60)
				idx = 6;
			else if (j < 65)
				idx = 5;
			else if (j < 70)
				idx = 4;
			else if (j < 75)
				idx = 3;
			else if (j < 80)
				idx = 2;
			else if (j < 85)
				idx = 1;
			else// if (j < 90)
				idx = 0;

			int temp_l = TEMP_LIST[idx + 1];
			int temp_h = TEMP_LIST[idx];
			int duty_l = m_config.DutyList[i][idx + 1];
			int duty_h = m_config.DutyList[i][idx];
			duty = int((duty_h - duty_l) / double(temp_h - temp_l) * (j - temp_l) + 0.5) + duty_l;
			dl = 9 - idx;
		}
		m_nSetDuty[i] = duty;
		m_nSetDutyLevel[i] = dl;
	}
}
void CCore::CalcStdDuty()
{
	int dl;
	int last_dl;
	int j,k;
	for (int i = 0; i < 2; i++)
	{
		j = m_nCurTemp[i];
		last_dl = m_nSetDutyLevel[i];//上一次的负载等级
		for (k = 0; k < 10; k++)
		{
			dl = 10 - k;
			if (j >= TEMP_LIST[k])
			{
				break;
			}
			else if (j < TEMP_LIST[k] - m_config.TransitionTemp)
			{
				continue;
			}
			else
			{
				//根据上一次的负载挡位决定
				if (last_dl >= dl)
				{
					break;
				}
				continue;
			}
		}
		k = min(9, k);
		m_nSetDuty[i] = m_config.DutyList[i][k];
		m_nSetDutyLevel[i] = dl;
	}
}
void CCore::ResetFan()
{
	if (m_bTakeOverStatus)
	{
		m_nSetDuty[0] = 0;
		m_nSetDutyLevel[0] = 0;
		m_nSetDuty[1] = 0;
		m_nSetDutyLevel[1] = 0;
		m_pfnSetFANDutyAuto(1);
		m_pfnSetFANDutyAuto(2);
		m_pfnSetFANDutyAuto(3);
		m_bTakeOverStatus = FALSE;
	}
}
void CCore::SetFanDuty()
{
	int duty;
	for (int i = 0; i < 2; i++)
	{
		if (m_nCurDuty[i] == m_nSetDuty[i])
			continue;
		duty = int(m_nSetDuty[i] * 255.0 / 100 + 0.5);
		m_pfnSetFanDuty(i + 1, duty);
		if (i == 1)
			m_pfnSetFanDuty(i + 2, duty);//如果存在第3个风扇
	}
	m_bTakeOverStatus = TRUE;
}