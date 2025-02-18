#ifndef _CPUCLASS_H_
#define _CPUCLASS_H_
/////////////
// LINKING //
/////////////
#pragma comment(lib, "pdh.lib")
//////////////
// INCLUDES //
//////////////
#include <pdh.h>
///////////////////////////////////////////////////////////////////////////////
// Class name: CpuClass
///////////////////////////////////////////////////////////////////////////////
class CPUClass
{
public:
	CPUClass();
	CPUClass(const CPUClass&);
	~CPUClass();
	void Initialize();
	void Shutdown();
	void Frame();
	int GetCPUPercentage();
private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};
#endif