#pragma once

#include "stdafx.h"

const ULONG MAX_SAMPLE_COUNT = 50;

class CTimer {
private :
	bool m_bHardware_Has_Performance_Counter;
	float m_fTime_Scale;
	float m_fElapsed_Time;
	__int64 m_nCurrent_Time;
	__int64 m_nLast_Time;
	__int64 m_nPerformance_Frequency;

	float m_fFrame_Time[MAX_SAMPLE_COUNT];
	ULONG m_nSample_Count;

	unsigned long m_nCurrent_FrameRate;
	unsigned long m_nFPS;
	float m_fTime_Cal_FPS;

	bool m_bStopped;

public :
	CTimer();
	virtual ~CTimer();

	void Start() {}
	void Stop() {}
	void Reset();
	void Tick(float fLock_FPS = 0.0f);
	unsigned long Get_FrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float Get_Elapsed_Time() { return m_fElapsed_Time; }
};

