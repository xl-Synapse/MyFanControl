#pragma once
using namespace std;
#include <string>


int GetTime(tm *pt = 0, int offset = 0);//得到当前时间，6位数时间92500,offset用于得到offset之后秒的时间
int GetTimeInterval(int a, int b, int *p = 0);//时间差，输入两个6位数时间，如开盘时间91500，得到a-b，并转化为6位数时间，指针p接受以秒计的时间差
CString GetExePath();//获得exe当前运行路径


struct ECData
{
	byte Remote;//温度
	byte Local;
	byte FanDuty;//风扇负载，0-255
	byte Reserve;
};

//温度控制相关函数类型声明
typedef  BOOL(InitIo)(void);//初始化接口
typedef  void(SetFanDuty)(int fan_id, int duty);//设置风扇负载，fan_id为正整数，duty为0-255负载
typedef  int(SetFANDutyAuto)(int fan_id);//设置风扇自动
typedef  ECData(GetTempFanDuty)(int fan_id);//得到风扇状态
typedef  int(GetFANCounter)(void);//返回风扇数量
typedef  const char*(GetECVersion)(void);//返回版本信息
typedef  int(GetFanRpm)(void);//得到风扇转速

//GPU控制相关函数类型声明
typedef void(__stdcall In_0_Out_0_Func)(void);
typedef int(__stdcall In_0_Out_n_Func)(void);
typedef int(__stdcall In_1_Out_n_Func)(int);
typedef int(__stdcall In_2_Out_n_Func)(int, int);
typedef PCWSTR(__stdcall In_0_Out_s_Func)(void);

class CGPUInfo
{
public:
	CGPUInfo();
	~CGPUInfo();

	int m_nBaseClock;
	int m_nBoostClock;
	PCWSTR m_sName;
	int m_nDeviceID;
	int m_nGraphicsRangeMax;
	int m_nGraphicsRangeMin;
	int m_nMemoryRangeMax;
	int m_nMemoryRangeMin;

	int m_nStandardFrequency;//默认频率，即正常下不超频也不降频的频率
	int m_nMaxFrequency;//最大频率，即最大允许超频的频率

	int m_nGraphicsClock;
	int m_nMemoryClock;
	int m_nUsage;//使用率
public:
	BOOL Update();//更新GPU频率和使用率
	BOOL LockFrequency(int frequency = 0);//锁定最大频率，可以用于超频，应小于m_nBoostClock+(m_nGraphicsRangeMax - m_nGraphicsRangeMin)，若设置0，则还原设置

protected:
	HMODULE m_hGPUdll;
	int m_nLockClock;//已设置的最大核心频率
	//接口函数
	In_0_Out_n_Func *m_pfnInitGPU_API;
	In_1_Out_n_Func *m_pfnSet_GPU_Number;
	In_0_Out_n_Func *m_pfnGet_GPU_Base_Clock;
	In_0_Out_n_Func *m_pfnGet_GPU_Boost_Clock;
	In_0_Out_n_Func *m_pfnCheck_GPU_VRAM_Clock;
	In_0_Out_n_Func *m_pfnGet_GPU_Graphics_Clock;
	In_0_Out_n_Func *m_pfnGet_GPU_Memory_Clock;
	In_0_Out_n_Func *m_pfnGet_Memory_OC_max;
	In_0_Out_n_Func *m_pfnGet_GPU_Util;
	In_0_Out_s_Func *m_pfnGet_GPU_name;
	In_0_Out_n_Func *m_pfnGet_GPU_TotalNumber;
	In_0_Out_n_Func *m_pfnGet_GPU_Overclock_range;
	In_0_Out_n_Func *m_pfnGet_Memory_range;
	In_0_Out_n_Func *m_pfnGet_GPU_Overclock_rangeMax;
	In_0_Out_n_Func *m_pfnGet_GPU_Overclock_rangeMin;
	In_0_Out_n_Func *m_pfnGet_Memory_range_max;
	In_0_Out_n_Func *m_pfnGet_Memory_range_min;
	In_1_Out_n_Func *m_pfnGet_NVDeviceID;
	In_2_Out_n_Func *m_pfnLock_Frequency;
	In_2_Out_n_Func *m_pfnLock_Frequency_MEM;
	In_2_Out_n_Func *m_pfnSet_CoreOC;
	In_2_Out_n_Func *m_pfnSet_MEMOC;
	In_0_Out_0_Func *m_pfnCloseGPU_API;
};


class CConfig
{
public:
	CConfig();
public:
	int DutyList[2][10];//2个风扇，cpu，gpu，每个风扇10组数字，分别对应90+，85+，80+，75+，70+，65+，60+，55+，50+，50-温度时的风扇负载
	int TransitionTemp;//过渡温度，当风扇降温时，需要比阈值温度再低TransitionTemp，才会降低风扇转速
	int UpdateInterval;//更新间隔
	BOOL Linear;//线性，线性调节风扇转速，而不是阶梯调节
	BOOL TakeOver;//接管控制，如果为否，则只获取状态，并不进行控制
	int ForceTemp;//强制冷却目标温度
	//GPU控制
	BOOL LockGPUFrequency;//是否控制GPU频率
	int GPUFrequency;//GPU频率限制

public:
	void LoadDefault();//载入默认设置
	void LoadConfig();//载入配置
	void SaveConfig();//保存配置
};


class CCore
{
public:
	CCore();
	~CCore();
protected:

	InitIo			*	m_pfnInitIo;
	SetFanDuty		*	m_pfnSetFanDuty;
	SetFANDutyAuto	*	m_pfnSetFANDutyAuto;
	GetTempFanDuty	*	m_pfnGetTempFanDuty;
	GetFANCounter	*	m_pfnGetFANCounter;
	GetECVersion	*	m_pfnGetECVersion;
	GetFanRpm		*	m_pfnGetFANRPM[2];

public:
	BOOL m_nInit;//是否初始化，0未初始化，1初始化成功，2初始化失败
	int m_nExit;//退出信号，0，不退出，1，需要退出，2，内核已退出
	HINSTANCE m_hInstDLL;//模块dll
	CConfig m_config;//配置文件
	CGPUInfo m_GpuInfo;//gpu频率控制对象
	int m_nCurTemp[2];//当前温度
	int m_nLastTemp[2];//上一次温度
	int m_nSetDuty[2];//设置的负载
	int m_nSetDutyLevel[2];//设置的转速挡位，最低速档为1，最高速档为10
	int m_nCurDuty[2];//当前负载
	int m_nCurRPM[2];//当前转速
	BOOL m_bUpdateRPM;//是否更新转速，如果为0，只更新风扇温度和负载
	int m_nLastUpdateTime;//最后更新时间
	BOOL m_bForcedCooling;//强制冷却
	BOOL m_bTakeOverStatus;//接管控制状态，描述最后一次调用的是m_pfnSetFanDuty（TRUE）还是m_pfnSetFANDutyAuto（FALSE）
	BOOL m_bForcedRefresh;//立即刷新

public:
	BOOL Init();
	void Uninit();
	void Run();//内核主循环
	void Work();//更新温度、负载，计算负载并进行设置
	void Update();//更新风扇状态
	void Control();//控制
	void CalcLinearDuty();//求得线性平滑后的负载（线性平滑控制）
	void CalcStdDuty();//求得当前温度对应的负载（阶梯控制）
	void ResetFan();//重置风扇为自动控制
	void SetFanDuty();//设置风扇转速
};

