#pragma once
using namespace std;
#include <string>


int GetTime(tm *pt = 0, int offset = 0);//�õ���ǰʱ�䣬6λ��ʱ��92500,offset���ڵõ�offset֮�����ʱ��
int GetTimeInterval(int a, int b, int *p = 0);//ʱ����������6λ��ʱ�䣬�翪��ʱ��91500���õ�a-b����ת��Ϊ6λ��ʱ�䣬ָ��p��������Ƶ�ʱ���
CString GetExePath();//���exe��ǰ����·��


struct ECData
{
	byte Remote;//�¶�
	byte Local;
	byte FanDuty;//���ȸ��أ�0-255
	byte Reserve;
};

//�¶ȿ�����غ�����������
typedef  BOOL(InitIo)(void);//��ʼ���ӿ�
typedef  void(SetFanDuty)(int fan_id, int duty);//���÷��ȸ��أ�fan_idΪ��������dutyΪ0-255����
typedef  int(SetFANDutyAuto)(int fan_id);//���÷����Զ�
typedef  ECData(GetTempFanDuty)(int fan_id);//�õ�����״̬
typedef  int(GetFANCounter)(void);//���ط�������
typedef  const char*(GetECVersion)(void);//���ذ汾��Ϣ
typedef  int(GetFanRpm)(void);//�õ�����ת��

//GPU������غ�����������
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

	int m_nStandardFrequency;//Ĭ��Ƶ�ʣ��������²���ƵҲ����Ƶ��Ƶ��
	int m_nMaxFrequency;//���Ƶ�ʣ����������Ƶ��Ƶ��

	int m_nGraphicsClock;
	int m_nMemoryClock;
	int m_nUsage;//ʹ����
public:
	BOOL Update();//����GPUƵ�ʺ�ʹ����
	BOOL LockFrequency(int frequency = 0);//�������Ƶ�ʣ��������ڳ�Ƶ��ӦС��m_nBoostClock+(m_nGraphicsRangeMax - m_nGraphicsRangeMin)��������0����ԭ����

protected:
	HMODULE m_hGPUdll;
	int m_nLockClock;//�����õ�������Ƶ��
	//�ӿں���
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
	int DutyList[2][10];//2�����ȣ�cpu��gpu��ÿ������10�����֣��ֱ��Ӧ90+��85+��80+��75+��70+��65+��60+��55+��50+��50-�¶�ʱ�ķ��ȸ���
	int TransitionTemp;//�����¶ȣ������Ƚ���ʱ����Ҫ����ֵ�¶��ٵ�TransitionTemp���Żή�ͷ���ת��
	int UpdateInterval;//���¼��
	BOOL Linear;//���ԣ����Ե��ڷ���ת�٣������ǽ��ݵ���
	BOOL TakeOver;//�ӹܿ��ƣ����Ϊ����ֻ��ȡ״̬���������п���
	int ForceTemp;//ǿ����ȴĿ���¶�
	//GPU����
	BOOL LockGPUFrequency;//�Ƿ����GPUƵ��
	int GPUFrequency;//GPUƵ������

public:
	void LoadDefault();//����Ĭ������
	void LoadConfig();//��������
	void SaveConfig();//��������
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
	BOOL m_nInit;//�Ƿ��ʼ����0δ��ʼ����1��ʼ���ɹ���2��ʼ��ʧ��
	int m_nExit;//�˳��źţ�0�����˳���1����Ҫ�˳���2���ں����˳�
	HINSTANCE m_hInstDLL;//ģ��dll
	CConfig m_config;//�����ļ�
	CGPUInfo m_GpuInfo;//gpuƵ�ʿ��ƶ���
	int m_nCurTemp[2];//��ǰ�¶�
	int m_nLastTemp[2];//��һ���¶�
	int m_nSetDuty[2];//���õĸ���
	int m_nSetDutyLevel[2];//���õ�ת�ٵ�λ������ٵ�Ϊ1������ٵ�Ϊ10
	int m_nCurDuty[2];//��ǰ����
	int m_nCurRPM[2];//��ǰת��
	BOOL m_bUpdateRPM;//�Ƿ����ת�٣����Ϊ0��ֻ���·����¶Ⱥ͸���
	int m_nLastUpdateTime;//������ʱ��
	BOOL m_bForcedCooling;//ǿ����ȴ
	BOOL m_bTakeOverStatus;//�ӹܿ���״̬���������һ�ε��õ���m_pfnSetFanDuty��TRUE������m_pfnSetFANDutyAuto��FALSE��
	BOOL m_bForcedRefresh;//����ˢ��

public:
	BOOL Init();
	void Uninit();
	void Run();//�ں���ѭ��
	void Work();//�����¶ȡ����أ����㸺�ز���������
	void Update();//���·���״̬
	void Control();//����
	void CalcLinearDuty();//�������ƽ����ĸ��أ�����ƽ�����ƣ�
	void CalcStdDuty();//��õ�ǰ�¶ȶ�Ӧ�ĸ��أ����ݿ��ƣ�
	void ResetFan();//���÷���Ϊ�Զ�����
	void SetFanDuty();//���÷���ת��
};

