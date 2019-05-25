#ifndef _FPSCLASS_H_
#define _FPSCLASS_H_
/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")
//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>

class FPSClass
{
public:
	FPSClass();
	FPSClass(const FPSClass&);
	~FPSClass();
	void Initialize();
	void Frame();
	int GetFPS();
private:
	int m_Fps, m_count;
	unsigned long m_startTime;
};
#endif